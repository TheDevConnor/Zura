#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <iostream>
#include <stdarg.h>
#include <stdint.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "../lib/colorize.hpp"
#include "../parser/parser.h"
#include "../compiler/object.h"
#include "../memory/memory.h"
#include "../compiler/value.h"
#include "../compiler/table.h"
#include "../parser/chunk.h"
#include "../debug/debug.h"
#include "../common.h"
#include "vm.h"

using namespace std;

VM vm;

void reset_stack() {
  vm.stack_top = vm.stack;
  vm.frame_count = 0;
  vm.open_upvalues = nullptr;
}

inline ObjFunction *get_frame_function(CallFrame *frame) {
  if (frame->function->type == OBJ_CLOSURE)
    return (ObjFunction *)frame->function;
  else
    return ((ObjClosure *)frame->function)->function;
}

void runtimeError(const char *format, ...) {
  size_t instruction = vm.frames->ip - vm.frames->closure->function->chunk.code - 1;
  int line = vm.frames->closure->function->chunk.lines[instruction];
  if (vm.frames->closure->function->name != nullptr) {
    cout << "[" << termcolor::yellow << "line" << termcolor::reset << " -> " 
         << termcolor::red << line << termcolor::reset << "][" << termcolor::yellow
         << "pos" << termcolor::reset << " -> " << termcolor::red << instruction
         << termcolor::reset << "][" << termcolor::red << "func" << termcolor::reset
         << " -> " << termcolor::red << vm.frames->closure->function->name->chars
         << termcolor::reset << "] in script \n";
  } else {
    cout << "[" << termcolor::yellow << "line" << termcolor::reset << " -> " 
         << termcolor::red << line << termcolor::reset << "][" << termcolor::yellow
         << "pos" << termcolor::reset << " -> " << termcolor::red << instruction
         << termcolor::reset << "] in script \n";
  }

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  reset_stack();
}

void init_vm() {
  reset_stack();
  vm.objects = nullptr;

  vm.bytes_allocated = 0;
  vm.next_gc = 1024 * 1024;

  vm.gray_count = 0;
  vm.gray_capacity = 0;
  vm.gray_stack = nullptr;

  init_table(&vm.globals);
  init_table(&vm.strings);
  init_table(&vm.arrays);

  init_value_array(&vm.array_values);

  vm.init_string = nullptr;
  vm.init_string = copy_string("init", 4);
}

void free_vm() {
  free_table(&vm.globals);
  free_table(&vm.strings);
  free_table(&vm.arrays);

  vm.init_string = nullptr;

  free_objects();
}

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  return *vm.stack_top;
}

Value peek(int distance) { return vm.stack_top[-1 - distance]; }

bool call(Obj *callee, ObjFunction *function, int arg_count) {
  if (arg_count != function->arity) {
    string message = "Expected -> ";
    message += to_string(function->arity);
    message += " arguments but got -> ";
    message += to_string(arg_count);
    runtimeError(message.c_str());
    return false;
  }

  if (vm.frame_count == FRAMES_MAX) {
    runtimeError("Stack overflow!🫃");
    return false;
  }

  CallFrame *frame = &vm.frames[vm.frame_count++];
  frame->closure = (ObjClosure *)callee;
  frame->ip = function->chunk.code;

  frame->slots = vm.stack_top - arg_count - 1;
  return true;
}

bool call_closure(ObjClosure *closure, int arg_count) {
  return call((Obj *)closure, closure->function, arg_count);
}

bool call_function(ObjFunction *function, int arg_count) {
  return call((Obj *)function, function, arg_count);
}

bool call_value(Value callee, int arg_count) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
    case OBJ_BOUND_METHOD: {
      ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
      vm.stack_top[-arg_count - 1] = bound->receiver;
      return call_closure(bound->method, arg_count);
    }
    case OBJ_CLASS: {
      ObjClass *klass = AS_CLASS(callee);
      vm.stack_top[-arg_count - 1] = OBJ_VAL(new_instance(klass));
      if(!IS_NIL(klass->initializer)) {
        return call_closure(AS_CLOSURE(klass->initializer), arg_count);
      } else if (arg_count != 0) {
        runtimeError("Expected 0 arguments but got %d.", arg_count);
        return false;
      }
      return true;
    }
    case OBJ_CLOSURE:
      return call_closure(AS_CLOSURE(callee), arg_count);
    case OBJ_FUNCTION:
      return call_function(AS_FUNCTION(callee), arg_count);
    case OBJ_NATIVE: {
      NativeFn native = AS_NATIVE(callee);
      Value result = native(arg_count, vm.stack_top - arg_count);
      vm.stack_top -= arg_count + 1;
      push(result);
      return true;
    }
    default:
      break; // Non-callable object type.
    }
  }
  runtimeError("Can only call functions and classes!");
  return false;
}

bool invoke_from_class(ObjClass *klass, ObjString *name, int arg_count) {
  Value method;
  if (!table_get(&klass->methods, name, &method)) {
    string message = "Undefined property ";
    message += name->chars;
    runtimeError(message.c_str());
    return false;
  }
  return call_closure(AS_CLOSURE(method), arg_count);
}

bool invoke(ObjString *name, int arg_count) {
  Value receiver = peek(arg_count);

  if (!IS_INSTANCE(receiver)) {
    runtimeError("Only instances have methods");
    return false;
  }

  ObjInstance *instance = AS_INSTANCE(receiver);
  return invoke_from_class(instance->klass, name, arg_count);
}

bool bind_method(ObjClass *klass, ObjString *name) {
  Value method;
  if (!table_get(&klass->methods, name, &method)) {
    string message = "Undefined property -> ";
    message += name->chars;
    runtimeError(message.c_str());
    return false;
  }

  ObjBoundMethod *bound = new_bound_method(peek(0), AS_CLOSURE(method));

  pop();
  push(OBJ_VAL(bound));
  return true;
}

ObjUpvalue *capture_upvalue(Value *local) {
  ObjUpvalue *prev_upvalue = nullptr;
  ObjUpvalue *upvalue = vm.open_upvalues;
  while (upvalue != nullptr && upvalue->location > local) {
    prev_upvalue = upvalue;
    upvalue = upvalue->next;
  }

  if (upvalue != nullptr && upvalue->location == local)
    return upvalue;

  ObjUpvalue *created_upvalue = new_upvalue(local);

  if (prev_upvalue == nullptr)
    vm.open_upvalues = created_upvalue;
  else
    prev_upvalue->next = created_upvalue;

  return created_upvalue;
}

void close_upvalues(Value *last) {
  while (vm.open_upvalues != nullptr && vm.open_upvalues->location >= last) {
    ObjUpvalue *upvalue = vm.open_upvalues;
    upvalue->closed = *upvalue->location;
    upvalue->location = &upvalue->closed;
    vm.open_upvalues = upvalue->next;
  }
}

void define_method(ObjString *name) {
  Value method = peek(0);
  ObjClass *klass = AS_CLASS(peek(1));
  table_set(&klass->methods, name, method);
  if (name == vm.init_string) klass->initializer = method;
  pop();
}

bool is_falsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concatenate() {
  ObjString *b = AS_STRING(peek(0));
  ObjString *a = AS_STRING(peek(1));

  int length = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString *result = take_string(chars, length);
  pop();
  pop();
  push(OBJ_VAL(result));
}

unordered_set<ObjString *> loadedModules;
unordered_set<ObjString *> loadingModules;
vector<ObjString *> circularDependence;

ObjModule *load_module(ObjString *name) {
// Check if the module is already being loaded
    if (loadingModules.find(name) != loadingModules.end()) {
        // Circular dependency detected, collect all modules involved
        circularDependence.push_back(name);
        for (const auto &module : loadingModules) {
            if (module != name) {
                circularDependence.push_back(module);
            }
        }

        string errorMessage = "Circular dependency detected in modules -> '";
        for (size_t i = 0; i < circularDependence.size(); i++) {
            errorMessage += circularDependence[i]->chars;
            if (i < circularDependence.size() - 1) {
                errorMessage += ", ";
            }
        }
        errorMessage += "'";
        runtimeError(errorMessage.c_str(), circularDependence.size());
        exit(1);
    }

    loadedModules.insert(name);

    // Mark the module as currently being loaded
    loadingModules.insert(name);

    string moduleFileName = string(name->chars, name->length);
    moduleFileName += ".zu";

    FILE *file = fopen(moduleFileName.c_str(), "rb");
    if (!file) {
        std::string errorMessage = "Could not load file -> '";
        errorMessage += moduleFileName;
        errorMessage += "'";
        runtimeError(errorMessage.c_str());
        exit(1);
    }

    std::vector<char> buffer;
    char chunk[1024];
    while (size_t bytesRead = fread(chunk, 1, sizeof(chunk), file)) {
        buffer.insert(buffer.end(), chunk, chunk + bytesRead);
    }
    fclose(file);

    string source(buffer.begin(), buffer.end());

    // ObjFunction* result = compile(source.c_str());
    // if (!result) {
    //     // Handle the error appropriately, such as returning an error value or
    //     // throwing an exception.
    //     runtimeError("Error loading module!");
    //     exit(1);
    // }

    InterpretResult result = interpret(source.c_str());
    if (result != INTERPRET_OK) {
        runtime_error("Error loading module!");
        exit(1);
    }

    // Finished loading the module, remove it from loadingModules
    loadingModules.erase(name);

    // Add the module globals to the global table
    ObjModule *module = AS_MODULE(pop());
    table_add_all(&module->variables, &vm.globals);

    return module;
}

static InterpretResult run() {
#ifndef DEBUG_TRACE_EXECUTION
  auto print_stack = []() {
    cout << "          ";
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    cout << "\n";
  };
#endif

  CallFrame *frame = &vm.frames[vm.frame_count - 1];

#define read_byte() (*frame->ip++)
#define read_short()                                                           \
  (frame->ip += 2,                                                             \
  (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define read_constant()                                                        \
  (frame->closure->function->chunk.constants.values[read_byte()])

#define BINARY_OP(value_type, op)                                              \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      runtimeError("Operands must be numbers\n");                            \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(value_type(a op b));                                                  \
  } while (false)

#define MODULO_OP(value_type, op)                                              \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      runtimeError("Operands must be numbers\n");                            \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(value_type(fmod(a, b)));                                              \
  } while (false)

#define POW_OP(value_type, op)                                                 \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      runtimeError("Operands must be numbers\n");                            \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(value_type(pow(a, b)));                                               \
  } while (false)

  for (;;) {
#ifndef DEBUG_TRACE_EXECUTION
    print_stack();
    disassemble_instruction(
        &frame->closure->function->chunk,
        (int)(frame->ip - frame->closure->function->chunk.code));
#endif

    uint8_t instruction;
    switch (instruction = read_byte()) {
    case OP_CONSTANT: {
      Value constant = read_constant();
      push(constant);
      break;
    }

    case OP_SLEEP: { 
      Value duration = peek(0);
      if (!IS_NUMBER(duration)) {
        runtimeError("Duration must be a number in seconds\n");
        return INTERPRET_RUNTIME_ERROR;
      } 
      sleep(AS_NUMBER(duration));
      table_add_all(&vm.globals, &vm.arrays);
      break;
    }

    // Global variable operation codes
    case OP_SET_GLOBAL: {
      ObjString *name = AS_STRING(read_constant());
      if (table_set(&vm.globals, name, peek(0))) {
        table_delete(&vm.globals, name);
        string message = "Undefined variable -> ";
        message += string(name->chars, name->length);
        runtimeError(message.c_str());
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_GET_GLOBAL: {
      ObjString *name = AS_STRING(read_constant());
      Value value;
      if (!table_get(&vm.globals, name, &value)) {
        string message = "Undefined variable -> ";
        message += string(name->chars, name->length);
        runtimeError(message.c_str());
        return INTERPRET_RUNTIME_ERROR;
      }
      push(value);
      break;
    }
    case OP_DEFINE_GLOBAL: {
      ObjString *name = AS_STRING(read_constant());
      table_set(&vm.globals, name, peek(0));
      pop();
      break;
    }
    // Local variable operation codes
    case OP_GET_LOCAL: {
      uint8_t slot = read_byte();
      push(frame->slots[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = read_byte();
      frame->slots[slot] = peek(0);
      break;
    }
    // Upvalue operation codes
    case OP_GET_UPVALUE: {
      uint8_t slot = read_byte();
      push(*frame->closure->upvalues[slot]->location);
      break;
    }
    case OP_SET_UPVALUE: {
      uint8_t slot = read_byte();
      *frame->closure->upvalues[slot]->location = peek(0);
      break;
    }
    // Property operations codes
    case OP_GET_PROPERTY: {
      if (!IS_INSTANCE(peek(0))) {
        runtimeError("Only instance have properties");
        return INTERPRET_RUNTIME_ERROR;
      }
      ObjInstance *instance = AS_INSTANCE(peek(0));
      ObjString *name = AS_STRING(read_constant());

      Value value;
      if (table_get(&instance->fields, name, &value)) {
        pop(); // Instance
        push(value);
        break;
      }

      if (!bind_method(instance->klass, name))
        return INTERPRET_RUNTIME_ERROR;
      break;
    }
    case OP_SET_PROPERTY: {
      if (!IS_INSTANCE(peek(1))) {
        runtimeError("Only instances have fields");
        return INTERPRET_RUNTIME_ERROR;
      }
      ObjInstance *instance = AS_INSTANCE(peek(1));
      table_set(&instance->fields, AS_STRING(read_constant()), peek(0));
      Value value = pop();
      pop();
      push(value);
      break;
    }
    // Super operation codes
    case OP_GET_SUPER: {
      ObjString *name = AS_STRING(read_constant());
      ObjClass *superclass = AS_CLASS(pop());
      if (!bind_method(superclass, name))
        return INTERPRET_RUNTIME_ERROR;
      break;
    }
    case OP_SUPER_INVOKE: {
      ObjString *method = AS_STRING(read_constant());
      int arg_count = read_byte();
      ObjClass *superclass = AS_CLASS(pop());
      if (!invoke_from_class(superclass, method, arg_count))
        return INTERPRET_RUNTIME_ERROR;
      frame = &vm.frames[vm.frame_count - 1];
      break;
    }
    // Array operation codes
    case OP_ARRAY: {
        uint8_t num_elements = read_byte();
        Value elements = peek(0);
        ValueArray array;
        init_value_array(&array);
        for (int i = 0; i < num_elements; i++) {
            write_value_array(&array, elements);
        }
        push(OBJ_VAL(&array));
        break;
    }
    // Bool operation codes
    case OP_TRUE:
      push(BOOL_VAL(true));
      break;
    case OP_FALSE:
      push(BOOL_VAL(false));
      break;
    case OP_NIL:
      push(NIL_VAL);
      break;
    case OP_POP:
      pop();
      break;

    // Comparison operation codes
    case OP_EQUAL: {
      Value b = pop();
      Value a = pop();
      push(BOOL_VAL(values_equal(a, b)));
      break;
    }
    case OP_GREATER:
      BINARY_OP(BOOL_VAL, >);
      break;
    case OP_LESS:
      BINARY_OP(BOOL_VAL, <);
      break;

    // Math operation codes
    case OP_ADD: {
      if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
        concatenate();
      } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
        double b = AS_NUMBER(pop());
        double a = AS_NUMBER(pop());
        push(NUMBER_VAL(a + b));
      } else {
        runtimeError("Operands must be two numbers or two strings\n");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_SUBTRACT:
      BINARY_OP(NUMBER_VAL, -);
      break;
    case OP_MULTIPLY:
      BINARY_OP(NUMBER_VAL, *);
      break;
    case OP_DIVIDE:
      BINARY_OP(NUMBER_VAL, /);
      break;
    case OP_MODULO:
      MODULO_OP(NUMBER_VAL, %);
      break;
    case OP_POWER:
      POW_OP(NUMBER_VAL, **);
      break;
    case OP_INCREMENT: {
      // ++2 and we have the value pushed on the stack
      double a = AS_NUMBER(pop());
      push(NUMBER_VAL(a + 1));
      break;
    }
    case OP_DECREMENT: {
      double a = AS_NUMBER(pop());
      push(NUMBER_VAL(a - 1));
      break;
    }

    case OP_NOT:
      push(BOOL_VAL(is_falsey(pop())));
      break;
    case OP_NEGATE: {
      if (!IS_NUMBER(peek(0))) {
        runtimeError("Operands must be numbers\n");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(NUMBER_VAL(-AS_NUMBER(pop())));
      break;
    }
    case OP_INVOKE: {
      ObjString *method = AS_STRING(read_constant());
      int arg_count = read_byte();
      if (!invoke(method, arg_count)) {
        return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm.frames[vm.frame_count - 1];
      break;
    }
    // Closure operation codes
    case OP_CLOSURE: {
      ObjFunction *function = AS_FUNCTION(read_constant());
      ObjClosure *closure = new_closure(function);
      push(OBJ_VAL(closure));
      for (int i = 0; i < closure->upvalue_count; i++) {
        uint8_t is_local = read_byte();
        uint8_t index = read_byte();
        if (is_local)
          closure->upvalues[i] = capture_upvalue(frame->slots + index);
        else
          closure->upvalues[i] = frame->closure->upvalues[index];
      }
      break;
    }
    case OP_CLOSE_UPVALUE:
      close_upvalues(vm.stack_top - 1);
      pop();
      break;
    // Jump operation codes for loops and if statements
    case OP_JUMP: {
      uint16_t offset = read_short();
      frame->ip += offset;
      break;
    }
    case OP_JUMP_IF_FALSE: {
      uint16_t offset = read_short();
      if (is_falsey(peek(0)))
        frame->ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = read_short();
      frame->ip -= offset;
      break;
    }
    case OP_BREAK: {
      uint16_t offset = read_short();
      frame->ip += offset;
      break;
    }
    // Call operation codes
    case OP_CALL: {
      int arg_count = read_byte();
      if (!call_value(peek(arg_count), arg_count))
        return INTERPRET_RUNTIME_ERROR;
      frame = &vm.frames[vm.frame_count - 1];
      break;
    }
    // Class operation codes
    case OP_CLASS: {
      push(OBJ_VAL(new_class(AS_STRING(read_constant()))));
      break;
    }
    case OP_INHERIT: {
      Value superclass = peek(1);
      ObjClass* subclass = AS_CLASS(peek(0));

      if(!IS_CLASS(superclass)) {
        runtimeError("Superclass must be a class");
        return INTERPRET_RUNTIME_ERROR;
      }

      table_add_all(&AS_CLASS(superclass)->methods, &subclass->methods);
      pop();
      break;
    }
    // Statement operation codes
    case OP_METHOD: {
      define_method(AS_STRING(read_constant()));
      break;
    }
    case OP_IMPORT: {
      ObjString *module_name = AS_STRING(pop());
      ObjModule *module = load_module(module_name);
      loadedModules.insert(module_name);
      table_add_all(&module->variables, &vm.globals);
      break;
    }
    case OP_STD: {
      cout << "Standard library loaded\n";
      ObjString* std_name = AS_STRING(pop()); // std/math -> math
      std_name->chars += 4; // math
      break;
    }
    case OP_INFO: {
      print_value(pop());
      cout << "\n";
      break;
    }

    case OP_RETURN: {
      Value result = pop();
      close_upvalues(frame->slots);
      vm.frame_count--;
      if (vm.frame_count == 0) {
          pop();
          return INTERPRET_OK;
      }
      vm.stack_top = frame->slots;
      push(result);
      frame = &vm.frames[vm.frame_count - 1];
      break;
    }
    default: {
      return INTERPRET_RUNTIME_ERROR;
    }
    }
  }
#undef BINARY_OP
#undef MODULO_OP
}

InterpretResult interpret(const char *source) {
  ObjFunction *function = compile(source);
  if (function == nullptr)
    return INTERPRET_COMPILE_ERROR;

  push(OBJ_VAL(function));
  ObjClosure *closure = new_closure(function);
  pop();
  push(OBJ_VAL(closure));
  call_value(OBJ_VAL(closure), 0);

  return run();
}

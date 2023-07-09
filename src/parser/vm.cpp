#include <unordered_map>
#include <stdarg.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>

#include "helper/terminal_color.h"
#include "../memory/memory.h"
#include "native_fn/native.h"
#include "../debug/debug.h"
#include "parser.h"
#include "../common.h"
#include "object.h"
#include "vm.h"

using namespace std;

VM vm;

void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
    vm.open_upvalues = nullptr;
}

inline ObjFunction* get_frame_function(CallFrame* frame) {
    if(frame->function->type == OBJ_CLOSURE) return (ObjFunction*)frame->function;
    else return ((ObjClosure*)frame->function)->function;
}

void _runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    cout << endl;

    for (int i = vm.frame_count - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = get_frame_function(frame);
        size_t instruction = frame->ip - function->chunk.code - 1;
        cout << set_color(RESET) << "["<< set_color(YELLOW) <<"line " << set_color(RESET) << "-> " 
                  << set_color(RED) << function->chunk.lines[instruction] << set_color(RESET) << "] in ";
        if(function->name == nullptr) {
            cout << set_color(RED) << "script" << set_color(RESET) << endl;
        } else {
            cout << set_color(RED) << function->name->chars << set_color(RESET) << endl;
        }
    }
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

    define_all_natives();
}

void free_vm() { 
    free_table(&vm.globals);
    free_table(&vm.strings);
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

bool call(Obj* callee, ObjFunction* function, int arg_count) {
    if(arg_count != function->arity) {
        string message = "Expected -> ";
        message += set_color(RED);
        message += to_string(function->arity);
        message += set_color(RESET);
        message += " arguments but got -> ";
        message += set_color(RED);
        message += to_string(arg_count);
        message += set_color(RESET);
        _runtime_error(message.c_str());
        return false;
    }

    if(vm.frame_count == FRAMES_MAX) {
        _runtime_error("Stack overflow!");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frame_count++];
    frame->closure = (ObjClosure*)callee;
    frame->ip = function->chunk.code;

    frame->slots = vm.stack_top - arg_count - 1;
    return true;
}

bool call_closure(ObjClosure* closure, int arg_count) {
    return call((Obj*)closure, closure->function, arg_count);
}

bool call_function(ObjFunction* function, int arg_count) {
    return call((Obj*)function, function, arg_count);
}

bool call_value(Value callee, int arg_count) {
    if(IS_OBJECT(callee)) {
        switch (OBJ_TYPE(callee)) {
            case OBJ_CLOSURE:  return call_closure(AS_CLOSURE(callee), arg_count);
            case OBJ_FUNCTION: return call_function(AS_FUNCTION(callee), arg_count);
            case OBJ_NATIVE: {
                NativeFn native = AS_NATIVE(callee);
                if (native(arg_count, vm.stack_top - arg_count)) {
                    vm.stack_top -= arg_count;
                    return true;
                } else {
                    _runtime_error(AS_STRING(vm.stack_top[-arg_count - 1])->chars);
                    return false;
                }
            }
            default: break; // Non-callable object type.
        }
    }
    _runtime_error("Can only call functions and classes!");
    return false;
}

ObjUpvalue* capture_upvalue(Value* local) {
    ObjUpvalue* prev_upvalue = nullptr;
    ObjUpvalue* upvalue = vm.open_upvalues;
    while(upvalue != nullptr && upvalue->location > local) {
        prev_upvalue = upvalue;
        upvalue = upvalue->next;
    }

    if(upvalue != nullptr && upvalue->location == local) return upvalue;

    ObjUpvalue* created_upvalue = new_upvalue(local);

    if(prev_upvalue == nullptr) vm.open_upvalues = created_upvalue;
    else prev_upvalue->next = created_upvalue;

    return created_upvalue;
}

void close_upvalues(Value* last) {
    while(vm.open_upvalues != nullptr && vm.open_upvalues->location >= last) {
        ObjUpvalue* upvalue = vm.open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm.open_upvalues = upvalue->next;
    }
}

bool is_falsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concatenate() {
    ObjString* b = AS_STRING(peek(0));
    ObjString* a = AS_STRING(peek(1));

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';
    
    ObjString* result = take_string(chars, length);
    pop();
    pop();
    push(OBJ_VAL(result));
}

ObjModule* load_module(ObjString* name) {
    const char* moduleFileName = name->chars;

    ifstream file(moduleFileName);
    if (!file.is_open()) {
        string message = "Could not open file -> ";
        message += set_color(RED);
        message += moduleFileName;
        message += set_color(RESET);
        _runtime_error(message.c_str());
        return nullptr;
    }

    string source((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    ObjFunction* function = compile(source.c_str());
    file.close();

    // Create an ObjModule and assign the compiled function to it
    ObjModule* module = new ObjModule();
    module->function = function;
    return module;
}

ObjModule* import_module(ObjString* name) {
    ObjModule* module = load_module(name);
    push(OBJ_VAL(module));
    return module;
}

static InterpretResult run() {
    #ifndef DEBUG_TRACE_EXECUTION
        auto print_stack = []() {
            cout << "          ";
            for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
                printf("[ ");
                print_value(*slot);
                printf(" ]");
            }
            cout << "\n";
        };
    #endif

    CallFrame* frame = &vm.frames[vm.frame_count - 1];

    #define read_byte() (*frame->ip++)
    #define read_short()                                        \
        (frame->ip += 2,                                        \
        (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
    #define read_constant()                                     \
        (frame->closure->function->chunk.constants.values[read_byte()])
    
    #define BINARY_OP(value_type, op)                           \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                _runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(a op b));                           \
        } while (false)

        #define MODULO_OP(value_type, op)                       \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                _runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(fmod(a, b)));                       \
        } while (false)

        #define POW_OP(value_type, op)                          \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                _runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(pow(a, b)));                        \
        } while (false)

    for (;;) {
        #ifndef DEBUG_TRACE_EXECUTION
            print_stack();
            disassemble_instruction(&frame->closure->function->chunk, 
            (int)(frame->ip - frame->closure->function->chunk.code));
        #endif

        uint8_t instruction;
        switch (instruction = read_byte()) {
            case OP_CONSTANT: push(read_constant()); break;
            // Global variable operation codes
            case OP_SET_GLOBAL: {
                ObjString* name = AS_STRING(read_constant());
                if (table_set(&vm.globals, name, peek(0))) {
                    table_delete(&vm.globals, name);
                    string message = "Undefined variable -> ";
                    message += set_color(RED);
                    message += string(name->chars, name->length);
                    message += set_color(RESET);
                    _runtime_error(message.c_str());
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_GET_GLOBAL: {
                ObjString* name = AS_STRING(read_constant());
                Value value;
                if (!table_get(&vm.globals, name, &value)) {
                    string message = "Undefined variable -> ";
                    message += set_color(RED);
                    message += string(name->chars, name->length);
                    message += set_color(RESET);
                    _runtime_error(message.c_str());
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                ObjString* name = AS_STRING(read_constant());
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
            // Bool operation codes
            case OP_TRUE:       push(BOOL_VAL(true)); break;
            case OP_FALSE:      push(BOOL_VAL(false)); break;
            case OP_NIL:        push(NIL_VAL); break;
            case OP_POP:        pop(); break;

            // Comparison operation codes
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(values_equal(a, b)));
                break;
            }
            case OP_GREATER:    BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:       BINARY_OP(BOOL_VAL, <); break;

            // Math operation codes
            case OP_ADD: {
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                } else {
                    _runtime_error("Operands must be two numbers or two strings\n");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT:   BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY:   BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:     BINARY_OP(NUMBER_VAL, /); break;
            case OP_MODULO:     MODULO_OP(NUMBER_VAL, %); break;
            case OP_POWER:         POW_OP(NUMBER_VAL, **); break;

            case OP_NOT: push(BOOL_VAL(is_falsey(pop()))); break;
            case OP_NEGATE: {
                if(!IS_NUMBER(peek(0))) {
                    _runtime_error("Operands must be numbers\n");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            // Closure operation codes
            case OP_CLOSURE: {
                ObjFunction* function = AS_FUNCTION(read_constant());
                ObjClosure* closure = new_closure(function);
                push(OBJ_VAL(closure));
                for (int i = 0; i < closure->upvalue_count; i++) {
                    uint8_t is_local = read_byte();
                    uint8_t index = read_byte();
                    if (is_local) closure->upvalues[i] = capture_upvalue(frame->slots + index);
                    else closure->upvalues[i] = frame->closure->upvalues[index];
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
                if (is_falsey(peek(0))) frame->ip += offset;
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
                if(!call_value(peek(arg_count), arg_count)) return INTERPRET_RUNTIME_ERROR;
                frame = &vm.frames[vm.frame_count - 1];
                break;
            }
            // Statement operation codes
            case OP_IMPORT: {
                ObjString* module_name = AS_STRING(pop());
                ObjModule* module = import_module(module_name);
                push(OBJ_VAL(module));
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
                    return INTERPRET_OK;
                }
                vm.stack_top = frame->slots;
                push(result);
                
                frame = &vm.frames[vm.frame_count - 1];
                break;
            }
            default: {
                cout << "Unknown opcode " << instruction;
                return INTERPRET_RUNTIME_ERROR;
            }
        }
    }
    #undef BINARY_OP
    #undef MODULO_OP
}

InterpretResult interpret(const char* source) {
    ObjFunction* function = compile(source);
    if (function == nullptr) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    ObjClosure* closure = new_closure(function);
    pop();
    push(OBJ_VAL(closure));
    call_value(OBJ_VAL(closure), 0);

    return run();
}

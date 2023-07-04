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
#include "common.h"
#include "object.h"
#include "vm.h"

VM vm;

void reset_stack() {
    vm.stack_top = vm.stack;
    vm.frame_count = 0;
}

void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    std::vprintf(format, args);
    va_end(args);
    std::cout << std::endl;

    for (int i = vm.frame_count - 1; i >= 0; i--) {
        CallFrame* frame = &vm.frames[i];
        ObjFunction* function = frame->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        std::cout << set_color(RESET) << "["<< set_color(YELLOW) <<"line " << set_color(RESET) << "-> " 
                  << set_color(RED) << function->chunk.lines[instruction] << set_color(RESET) << "] in ";
        if(function->name == nullptr) {
            std::cout << set_color(RED) << "script" << set_color(RESET) << std::endl;
        } else {
            std::cout << set_color(RED) << function->name->chars << set_color(RESET) << std::endl;
        }
    }
    reset_stack();
}

void init_vm() { 
    reset_stack();
    vm.objects = nullptr;

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

bool call(ObjFunction* function, int arg_count) {
    if(arg_count != function->arity) {
        std::string message = "Expected -> ";
        message += set_color(RED);
        message += std::to_string(function->arity);
        message += set_color(RESET);
        message += " arguments but got -> ";
        message += set_color(RED);
        message += std::to_string(arg_count);
        message += set_color(RESET);
        runtime_error(message.c_str());
        return false;
    }

    if(vm.frame_count == FRAMES_MAX) {
        runtime_error("Stack overflow!");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frame_count++];
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->slots = vm.stack_top - arg_count - 1;
    return true;
}

bool call_value(Value callee, int arg_count) {
    if(IS_OBJECT(callee)) {
        switch (OBJ_TYPE(callee)) {
            case OBJ_FUNCTION: return call(AS_FUNCTION(callee), arg_count);
            case OBJ_NATIVE: {
                NativeFn native = AS_NATIVE(callee);
                Value result = native(arg_count, vm.stack_top - arg_count);
                vm.stack_top -= arg_count + 1;
                push(result);
                return true;
            }
            default: break; // Non-callable object type.
        }
    }
    runtime_error("Can only call functions and classes!");
    return false;
}

bool is_falsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void concatenate() {
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';
    
    ObjString* result = take_string(chars, length);
    push(OBJ_VAL(result));
}

static InterpretResult run();

ObjModule* load_module(ObjString* name) {
    const char* moduleFileName = name->chars;
    moduleFileName = strcat((char*)moduleFileName, ".az");

    std::ifstream file(moduleFileName);
    if (!file.is_open()) {
        std::string message = "Could not open file -> ";
        message += set_color(RED);
        message += moduleFileName;
        message += set_color(RESET);
        runtime_error(message.c_str());
        return nullptr;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    InterpretResult result = interpret(source.c_str());
    if (result != INTERPRET_OK) return nullptr;
    file.close();
    return AS_MODULE(pop());
}

ObjModule* import_module(ObjString* name) {
    ObjModule* module = load_module(name);
    table_set(&vm.modules, name, OBJ_VAL(module));
    return module;
}

static InterpretResult run() {
    #ifndef DEBUG_TRACE_EXECUTION
        auto print_stack = []() {
            std::cout << "          ";
            for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
                std::printf("[ ");
                print_value(*slot);
                std::printf(" ]");
            }
            std::cout << "\n";
        };
    #endif

    CallFrame* frame = &vm.frames[vm.frame_count - 1];

    #define read_byte() (*frame->ip++)
    #define read_short()                                        \
        (frame->ip += 2,                                        \
        (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
    #define read_constant()                                     \
        (frame->function->chunk.constants.values[read_byte()])
    
    #define BINARY_OP(value_type, op)                           \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(a op b));                           \
        } while (false)

        #define MODULO_OP(value_type, op)                       \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(fmod(a, b)));                       \
        } while (false)

        #define POW_OP(value_type, op)                          \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                runtime_error("Operands must be numbers\n");    \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(pow(a, b)));                        \
        } while (false)

    for (;;) {
        #ifndef DEBUG_TRACE_EXECUTION
            print_stack();
            disassemble_instruction(&frame->function->chunk, (int)(frame->ip - frame->function->chunk.code));
        #endif

        uint8_t instruction;
        switch (instruction = read_byte()) {
            case OP_CONSTANT: push(read_constant()); break;
            // Global variable operation codes
            case OP_SET_GLOBAL: {
                ObjString* name = AS_STRING(read_constant());
                if (table_set(&vm.globals, name, peek(0))) {
                    table_delete(&vm.globals, name);
                    std::string message = "Undefined variable -> ";
                    message += set_color(RED);
                    message += std::string(name->chars, name->length);
                    message += set_color(RESET);
                    runtime_error(message.c_str());
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_GET_GLOBAL: {
                ObjString* name = AS_STRING(read_constant());
                Value value;
                if (!table_get(&vm.globals, name, &value)) {
                    std::string message = "Undefined variable -> ";
                    message += set_color(RED);
                    message += std::string(name->chars, name->length);
                    message += set_color(RESET);
                    runtime_error(message.c_str());
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
                    runtime_error("Operands must be two numbers or two strings\n");
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
                    runtime_error("Operands must be numbers\n");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
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
                table_set(&vm.modules, module_name, OBJ_VAL(module));
                break;
            }
            case OP_INFO: {
                print_value(pop());
                std::cout << "\n";
                break;
            }
            case OP_RETURN: {
                Value result = pop();
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
                std::cout << "Unknown opcode " << instruction;
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
    call(function, 0);

    return run();
}

#include <unordered_map>
#include <stdarg.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>

#include "helper/terminal_color.h"
#include "../memory/memory.h"
#include "../debug/debug.h"
#include "parser.h"
#include "common.h"
#include "object.h"
#include "vm.h"

VM vm;

void reset_stack() {
    vm.stack_top = vm.stack;
}

void runtime_error(const char* format, ...) {
    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    std::cout << set_color(RESET) << "["<< set_color(YELLOW) <<"line " << set_color(RESET) << "-> " 
              << set_color(RED) << line << set_color(RESET) << "] in script" << std::endl;
    std::cout << format << "\n" << std::endl;
    reset_stack();
}

void init_vm() { 
    reset_stack();
    vm.objects = nullptr;

    init_table(&vm.globals);
    init_table(&vm.strings);
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
    std::string moduleFileName = std::string(name->chars, name->length) + ".az";

    std::ifstream file(moduleFileName);
    if (!file.is_open()) {
        return nullptr;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(source.c_str(), &chunk)) {
        free_chunk(&chunk);
        file.close();
        return nullptr;
    }

    ObjModule* module = new ObjModule();
    module->name = name;
    init_table(&module->variables);

    push(OBJ_VAL(module));

    Chunk* previousChunk = vm.chunk;
    uint8_t* previousIp = vm.ip;

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    vm.chunk = previousChunk;
    vm.ip = previousIp;


    free_chunk(&chunk);
    file.close();
    return result == INTERPRET_OK ? module : nullptr;
}

ObjModule* import_module(ObjString* name) {
    ObjModule* module = load_module(name);
    if (!module) {
        std::string message = "Could not load module -> ";
        message += set_color(RED);
        message += std::string(name->chars, name->length);
        runtime_error(message.c_str());
        message = set_color(RESET);
        return nullptr;
    }
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

    #define BINARY_OP(value_type, op)                           \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                std::cout << "Operands must be numbers\n";      \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(a op b));                           \
        } while (false)

        #define MODULO_OP(value_type, op)                       \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                std::cout << "Operands must be numbers\n";      \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(fmod(a, b)));                       \
        } while (false)

        #define POW_OP(value_type, op)                          \
        do {                                                    \
            if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {   \
                std::cout << "Operands must be numbers\n";      \
                return INTERPRET_RUNTIME_ERROR;                 \
            }                                                   \
            double b = AS_NUMBER(pop());                        \
            double a = AS_NUMBER(pop());                        \
            push(value_type(pow(a, b)));                        \
        } while (false)

    for (;;) {
        #ifndef DEBUG_TRACE_EXECUTION
            print_stack();
            disassemble_instruction(vm.chunk, static_cast<int>(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction = *vm.ip++;
        switch (instruction) {
            case OP_CONSTANT: push(vm.chunk->constants.values[*vm.ip++]); break;
            // Global variable operation codes
            case OP_SET_GLOBAL: {
                ObjString* name = AS_STRING(vm.chunk->constants.values[*vm.ip++]);
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
                ObjString* name = AS_STRING(vm.chunk->constants.values[*vm.ip++]);
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
                ObjString* name = AS_STRING(vm.chunk->constants.values[*vm.ip++]);
                table_set(&vm.globals, name, peek(0));
                pop();
                break;
            }
            // Local variable operation codes
            case OP_GET_LOCAL: {
                uint8_t slot = *vm.ip++;
                push(vm.stack[slot]);
                break;
            }
            case OP_SET_LOCAL: {
                uint8_t slot = *vm.ip++;
                vm.stack[slot] = peek(0);
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
                    std::cout << "Operand must be a number\n";
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            // Jump operation codes for loops and if statements
            case OP_JUMP: {
                uint16_t offset = (vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]));
                vm.ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = (vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]));
                if (is_falsey(peek(0))) vm.ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = (vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]));
                vm.ip -= offset;
                break;
            }
            case OP_BREAK: {
                uint16_t offset = (vm.ip += 2, (uint16_t)((vm.ip[-2] << 8) | vm.ip[-1]));
                vm.ip += offset;
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
                print_value(pop());
                std::cout << "\n";
                return INTERPRET_OK;
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
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
}

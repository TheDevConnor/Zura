#include <unordered_map>
#include <stdarg.h>
#include <iostream>
#include <cmath>

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
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    reset_stack();
}

void init_vm() { reset_stack(); }

void free_vm() {}

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

    ObjString* result = copy_string(chars, length);
    push(OBJ_VAL(result));
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
            // Bool operation codes
            case OP_TRUE:       push(BOOL_VAL(true)); break;
            case OP_FALSE:      push(BOOL_VAL(false)); break;
            case OP_NIL:        push(NIL_VAL); break;

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
                    std::cout << "Operands must be two numbers or two strings\n";
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

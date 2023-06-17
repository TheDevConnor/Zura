#include <unordered_map>
#include <iostream>
#include <cmath>

#include "../debug/debug.h"
#include "parser.h"
#include "common.h"
#include "vm.h"

VM vm;

void init_vm() {
    vm.stack_top = vm.stack;
}

void free_vm() {}

void push(Value value) {
    *vm.stack_top++ = value;
}

Value pop() {
    return *--vm.stack_top;
}

static InterpretResult run() {
    #ifndef DEBUG_TRACE_EXECUTION
        auto print_stack = []() {
            std::cout << "          ";
            for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
                std::cout << "[ " << *slot << " ]";
            }
            std::cout << "\n";
        };
    #endif

    #define BINARY_OP(op) \
        do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
        } while (false)

    for (;;) {
        #ifndef DEBUG_TRACE_EXECUTION
            print_stack();
            disassemble_instruction(vm.chunk, static_cast<int>(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction = *vm.ip++;
        switch (instruction) {
            case OP_CONSTANT: push(vm.chunk->constants.values[*vm.ip++]); break;
            case OP_ADD:        BINARY_OP(+); break;
            case OP_SUBTRACT:   BINARY_OP(-); break;
            case OP_MULTIPLY:   BINARY_OP(*); break;
            case OP_DIVIDE:     BINARY_OP(/); break;
            case OP_MODULO:     push(fmod(pop(), pop())); break;
            case OP_NEGATE:     push(-pop()); break;
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
}

InterpretResult interpret(const char* source) {
    compile(source);
    return INTERPRET_OK;
}

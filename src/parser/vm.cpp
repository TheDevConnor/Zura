#include <unordered_map>
#include <iostream>
#include <cmath>

#include "../debug/debug.h"
#include "parser.h"
#include "common.h"
#include "vm.h"

VM vm;

void init_vm() {
    vm.stack = nullptr;
    vm.stack_capacity = 0;
    vm.stack_count = 0;
}

void free_vm() {}

void push(Value value) {
    if (vm.stack_capacity < vm.stack_count + 1) {
        int old_capacity = vm.stack_capacity;
        vm.stack_capacity = GROW_CAPACITY(old_capacity);
        vm.stack = GROW_ARRAY(Value, vm.stack, old_capacity, vm.stack_capacity);
    }

    vm.stack[vm.stack_count] = value;
    vm.stack_count++;
}

Value pop() {
    vm.stack_count--;
    return vm.stack[vm.stack_count];
}

static InterpretResult run() {
    #ifndef DEBUG_TRACE_EXECUTION
        auto print_stack = []() {
            std::cout << "          ";
            for (Value* slot = vm.stack; slot < vm.stack_count + vm.stack; slot++) {
                std::cout << "[ " << *slot << " ]";
            }
            std::cout << "\n";
        };
    #endif

    #define BINARY_OP(op)       \
        do {                    \
            Value b = pop();    \
            Value a = pop();    \
            push(a op b);       \
        } while (false)

    #define MODULO_OP(op)       \
        do {                    \
            Value b = pop();    \
            Value a = pop();    \
            push(fmod(a, b));   \
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
            case OP_MODULO:     MODULO_OP(%); break;
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
    #undef MODULO_OP
}

InterpretResult interpret(const char* source) {
    compile(source);
    return INTERPRET_OK;
}

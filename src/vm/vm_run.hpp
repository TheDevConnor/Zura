#pragma once

#include <iostream>
#include <cmath>

#include "../helper/errors/vm_error.hpp"
#include "../debug/debug.hpp"
#include "../types/type.hpp"
#include "../common.hpp"
#include "vm.hpp"

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define LINE_READ (vm.chunk->lines[vm.ip - vm.chunk->code - 1])

void performBinaryOp(uint8_t op) {
    if(!isOpNumber()) {
        VMError::vm_error("[line: " + std::to_string(LINE_READ) + "] Operands must be numbers.");
        return;
    }

    double b = AS_NUMBER(pop());
    double a = AS_NUMBER(pop());

    switch (op) {
        case OP_ADD:      push(NUMBER_VAL(a + b)); break;
        case OP_SUBTRACT: push(NUMBER_VAL(a - b)); break;
        case OP_MULTIPLY: push(NUMBER_VAL(a * b)); break;
        case OP_DIVIDE:   push(NUMBER_VAL(a / b)); break;
        case OP_POW:      push(NUMBER_VAL(pow(a, b))); break;
        case OP_MOD:      push(NUMBER_VAL(fmod(a, b))); break;

        // Comparison operations
        case OP_GREATER: push(BOOL_VAL(a > b)); break;
        case OP_LESS: push(BOOL_VAL(a < b)); break;
    }
}

static Zura_Exit_Value run() {
    while (true) {
    #ifdef DEBUG_TRACE_EXECUTION
        std::cout << "          ";
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            std::cout << "[ ";
            printValue(*slot);
            std::cout << " ]";
        }
        std::cout << std::endl;
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
    #endif

        for (;;){
            uint8_t instruction;
            switch (instruction = READ_BYTE()) {
                // Constantss
                case OP_CONSTANT: {
                    Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }

                // Numaric operations
                case OP_ADD:
                case OP_SUBTRACT:
                case OP_MULTIPLY:
                case OP_DIVIDE:
                case OP_POW:
                case OP_MOD: performBinaryOp(instruction); break;

                case OP_NEGATE: {
                    if (!IS_NUMBER(peek(0))) {
                        VMError::vm_error("[line: " + std::to_string(LINE_READ) + "] Operand must be numbers.");
                        return RUNTIME_ERROR;
                    }
                    push(NUMBER_VAL(-AS_NUMBER(pop())));
                    break;
                }

                // Boolean operations
                case OP_NIL:   push(NIL_VAL()); break;
                case OP_TRUE:  push(BOOL_VAL(true)); break;
                case OP_FALSE: push(BOOL_VAL(false)); break;

                // Comparison operations
                case OP_NOT: push(BOOL_VAL(isFalsey(pop()))); break;
                case OP_EQUAL: {
                    Value b = pop();
                    Value a = pop();
                    push(BOOL_VAL(valuesEqual(a, b)));
                    break;
                }
                case OP_GREATER: performBinaryOp(instruction); break;
                case OP_LESS: performBinaryOp(instruction); break;
                
                // Logical operations
                case OP_RETURN: {
                    printValue(pop());
                    std::cout << std::endl;
                    return OK;
                }

            default: {
                VMError::vm_error("[line: " + std::to_string(LINE_READ) + "] Unknown OpCode.");
                return RUNTIME_ERROR;
            }
            }
        }

    }
}

#undef READ_BYTE
#undef READ_CONSTANT
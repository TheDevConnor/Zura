#include <iostream>

#include "../debug/debug.hpp"
#include "../types/type.hpp"
#include "vm_opcode_fn.hpp"
#include "../common.hpp"
#include "vm.hpp"

using namespace Zura;

static Zura_Exit_Value run() {
    while (true) {
    #ifdef DEBUG_TRACE_EXECUTION
        std::cout << "          ";
        for (Types::Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            std::cout << "[ ";
            printValue(*slot);
            std::cout << " ]";
        }
        std::cout << std::endl;
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
    #endif
        uint8_t instruction = *vm.ip++;
        if (instruction < sizeof(opCodeHandlers) / sizeof(opCodeHandlers[0])) {
            // execute the instruction
            std::cout << "Executing " << instruction << std::endl;
            opCodeHandlers[instruction]();
        } else {
            std::cout << "Unknown opcode " << instruction << std::endl;
            return Zura_Exit_Value::RUNTIME_ERROR;
        }
    }
}
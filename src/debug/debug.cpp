#include <iostream>
#include <iomanip>

#include "../compiler/value.hpp"
#include "debug.hpp"

void disassembleChunk(Chunk* chunk, const char* name) {
    std::cout << "== " << name << " ==" << std::endl;

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char* name, int offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    std::cout << std::left << std::setw(16) << name << " " << std::setw(4) << constant << " '";
    printValue(chunk->constants.values[constant]);
    std::cout << "'" << std::endl;
    return offset + 2;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    // std::cout << std::setfill('0') << std::setw(4) << offset << " ";
    std::cout << offset << "  ";
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
        std::cout << "   | ";
    else
        std::cout << chunk->lines[offset] << " ";

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);

        case OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        case OP_POW:
            return simpleInstruction("OP_POW", offset);
        case OP_MOD:
            return simpleInstruction("OP_MOD", offset);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);

        case OP_NIL:
            return simpleInstruction("OP_NIL", offset);
        case OP_TRUE:
            return simpleInstruction("OP_TRUE", offset);
        case OP_FALSE:
            return simpleInstruction("OP_FALSE", offset);

        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instruction << std::endl;
            return offset + 1;
    }
}
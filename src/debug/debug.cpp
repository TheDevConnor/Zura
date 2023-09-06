#include <iostream>

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

int disassembleInstruction(Chunk* chunk, int offset) {
    std::cout << offset << " ";

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instruction << std::endl;
            return offset + 1;
    }
}
#include <iostream>
#include <iomanip>

#include "../parser/chunk.h"
#include "../parser/value.h"
#include "debug.h"

void disassemble_chunk(Chunk* chunk, const char* name) {
    std::cout << " == " << name << " ==" << std::endl;

    for (int offset = 0; offset < chunk->count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int simple_instruction(const char* name, int offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

static int constant_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d - ", name, constant);
    print_value(chunk->constants.values[constant]);
    return offset + 2;
}

int disassemble_instruction(Chunk* chunk, int offset) {
    std::cout << std::setw(4) << std::setfill('0') << offset << ' ';

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        std::cout << "    | ";
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << static_cast<int>(instruction) << std::endl;
            return offset + 1;
    }
}

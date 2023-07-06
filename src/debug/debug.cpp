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

static int byte_instruction(const char* name, Chunk* chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    std::printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

static int jump_instruction(const char* name, int sign, Chunk* chunk, int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    std::printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
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
        case OP_CONSTANT: return constant_instruction("OP_CONSTANT", chunk, offset);

        case OP_GET_GLOBAL:    return constant_instruction("OP_GET_GLOBAL", chunk, offset);
        case OP_SET_GLOBAL:    return constant_instruction("OP_SET_GLOBAL", chunk, offset);
        case OP_DEFINE_GLOBAL: return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);

        case OP_GET_LOCAL:  return byte_instruction("OP_GET_LOCAL", chunk, offset);
        case OP_SET_LOCAL:  return byte_instruction("OP_SET_LOCAL", chunk, offset);

        case OP_JUMP_IF_FALSE: return jump_instruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
        case OP_LOOP:          return jump_instruction("OP_LOOP", -1, chunk, offset);
        case OP_JUMP:          return jump_instruction("OP_JUMP", 1, chunk, offset);
        case OP_BREAK:         return jump_instruction("OP_BREAK", 1, chunk, offset);

        case OP_NIL:      return simple_instruction("OP_NIL", offset);
        case OP_TRUE:     return simple_instruction("OP_TRUE", offset);
        case OP_FALSE:    return simple_instruction("OP_FALSE", offset);
        case OP_POP:      return simple_instruction("OP_POP", offset);

        case OP_EQUAL:    return simple_instruction("OP_EQUAL", offset);
        case OP_GREATER:  return simple_instruction("OP_GREATER", offset);
        case OP_LESS:     return simple_instruction("OP_LESS", offset);

        case OP_ADD:      return simple_instruction("OP_ADD", offset);
        case OP_SUBTRACT: return simple_instruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY: return simple_instruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:   return simple_instruction("OP_DIVIDE", offset);
        case OP_MODULO:   return simple_instruction("OP_MODULO", offset);
        case OP_POWER:    return simple_instruction("OP_POWER", offset);
        
        case OP_NOT:      return simple_instruction("OP_NOT", offset);
        case OP_NEGATE:   return simple_instruction("OP_NEGATE", offset);

        case OP_CLOSURE: {
            offset++;
            uint8_t constant = chunk->code[offset++];
            std::cout << constant << " OP_CLOSURE";
            print_value(chunk->constants.values[constant]);
            std::cout << "\n";
            return offset;
        }

        case OP_CALL:     return byte_instruction("OP_CALL", chunk, offset);

        case OP_IMPORT:   return simple_instruction("OP_IMPORT", offset);

        case OP_INFO:     return simple_instruction("OP_INFO", offset);

        case OP_RETURN:   return simple_instruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << static_cast<int>(instruction) << std::endl;
            return offset + 1;
    }
}

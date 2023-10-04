#pragma once

#include <cstdint>

#include "../compiler/value.hpp"
#include "../types/type.hpp"

using namespace Zura;

enum OPCode {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POW,
    OP_MOD,
    OP_NEGATE,
    OP_RETURN,
};

struct Chunk {
    uint8_t* code;
    size_t capacity;
    size_t count;
    int* lines;

    ValueArray constants;
};

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);

int addConstants(Chunk* chunk, Types::Value value);

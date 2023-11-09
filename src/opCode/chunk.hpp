#pragma once

#include <cstdint>

#include "../compiler/value.hpp"
#include "../types/type.hpp"

enum OPCode {
    // Constants
    OP_CONSTANT,

    // Type casting
    OP_CAST_INT,

    // Variables
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,

    // Numeric operations
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POW,
    OP_MOD,
    OP_NEGATE,

    // Boolean operations
    OP_NIL,
    OP_TRUE,
    OP_FALSE,

    // Comparison operations
    OP_NOT,
    OP_GREATER,
    OP_LESS,
    OP_EQUAL,

    // Logical operations
    OP_POP,
    OP_PRINT,
    OP_RETURN,
};

struct Chunk {
    uint8_t* code;
    size_t   capacity;
    size_t   count;
    int*     lines;

    ValueArray constants;
};

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);

size_t addConstants(Chunk* chunk, Value value);

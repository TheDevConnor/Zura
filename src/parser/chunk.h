#ifndef AZUA_CHUNK_H
#define AZUA_CHUNK_H

#include "common.h"
#include "value.h"

enum OpCode {
    OP_CONSTANT,
    // Binary operations
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    // Unary operations
    OP_NEGATE,
    OP_RETURN,
};

struct Chunk {
    uint8_t* code;
    int capacity;
    int* lines;
    int count;

    ValueArray constants;
};

void init_chunk(Chunk* chunk);
void free_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t byte, int line);
int add_constant(Chunk* chunk, Value value);

#endif // AZUA_CHUNK_H

#ifndef AZUA_CHUNK_H
#define AZUA_CHUNK_H

#include "../common.h"
#include "value.h"

enum OpCode {
    OP_CONSTANT,
    // Global operations
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_DEFINE_GLOBAL,
    // Local operations
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    // Upvalue operations
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    // Property operations
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    // Binary operations
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_POWER,
    // Bool operations
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    // Comparison operations
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    // Unary operations
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
    // Closures
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    // Jump operations (for if statements, loops, etc.)
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_LOOP,
    OP_BREAK,
    // Call operation
    OP_CALL,
    // Statements
    OP_METHOD,
    OP_IMPORT,
    OP_CLASS,
    OP_INFO,
    OP_POP,
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

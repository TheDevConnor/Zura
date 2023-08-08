#pragma once

#include "../common.h"
#include "../compiler/value.h"

enum OpCode {
  OP_CONSTANT,
  // Global operations
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_DEFINE_GLOBAL,
  // static operations
  OP_GET_STATIC,
  OP_DEFINE_STATIC,
  // Local operations
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  // Upvalue operations
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  // Property operations
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  // Super operations
  OP_GET_SUPER,
  OP_SUPER_INVOKE,
  // Array operations
  OP_ARRAY,
  // Binary operations
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_MODULO,
  OP_POWER,
  OP_INCREMENT,
  OP_DECREMENT,
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
  OP_INVOKE,
  // Class operations
  OP_INHERIT,
  // Statements
  OP_METHOD,
  OP_IMPORT,
  OP_SLEEP,
  OP_STD,
  OP_CLASS,
  OP_INPUT,
  OP_INFO,
  OP_POP,
};

struct Chunk {
  uint8_t *code;
  int capacity;
  int *lines;
  int count;

  ValueArray constants;
};

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, uint8_t byte, int line);
int add_constant(Chunk *chunk, Value value);

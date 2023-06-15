#include <iostream>

#include "chunk.h"
#include "../memory/memory.h"

void init_chunk(Chunk* chunk) {
    chunk->lines = nullptr;
    chunk->code = nullptr;
    chunk->capacity = 0;
    chunk->count = 0;

    init_value_array(&chunk->constants);
}

void free_chunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

void write_chunk(Chunk* chunk, uint8_t byte, int line)  {
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, old_capacity, chunk->capacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_capacity, chunk->capacity);
    }
    chunk->lines[chunk->count] = line;
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

int add_constant(Chunk* chunk, Value value) {
    write_value_array(&chunk->constants, value);
    return chunk->constants.count - 1;
}

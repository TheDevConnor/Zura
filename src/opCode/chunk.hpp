#pragma once

#include <cstdint>

enum OPCode {
    OP_RETURN,
};

struct Chunk {
    uint8_t* code;
    int capacity;
    int count;
};

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);
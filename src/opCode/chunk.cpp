#include <cstdint>

#include "../memory/memory.hpp"
#include "chunk.hpp"

void initChunk(Chunk* chunk) {
    chunk->capacity = 0;
    chunk->count = 0;
    chunk->code = nullptr;
}

void freeChunk(Chunk* chunk) {
    Memory::FREE_ARRAY(chunk->code, chunk->capacity);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = Memory::GROW_CAPACITY(oldCapacity);
        chunk->code = Memory::GROW_ARRAY(chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;
}
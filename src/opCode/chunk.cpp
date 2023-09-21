#include <cstdint>

#include "../compiler/value.hpp"
#include "../memory/memory.hpp"
#include "../types/type.hpp"
#include "chunk.hpp"

using namespace Zura;

void initChunk(Chunk* chunk) {
    chunk->capacity = 0;
    chunk->count = 0;
    chunk->lines = nullptr;
    chunk->code = nullptr;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    Memory::FREE_ARRAY(chunk->code, chunk->capacity);
    Memory::FREE_ARRAY(chunk->lines, chunk->capacity);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = Memory::GROW_CAPACITY(oldCapacity);
        chunk->code = Memory::GROW_ARRAY(chunk->code, oldCapacity, chunk->capacity);
        chunk->lines = Memory::GROW_ARRAY(chunk->lines, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int addConstants(Chunk* chunk, Types::Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}
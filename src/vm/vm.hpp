#pragma once

#include "../compiler/value.hpp"
#include "../opCode/chunk.hpp"
#include "../common.hpp"

struct VM {
    Chunk* chunk;
    uint8_t* ip;

    Value stack[256]; // 256 is the max stack size
    Value* stackTop;
};

inline VM vm;

void initVM();
void freeVM();

Zura_Exit_Value interpret(const char* source);

void push(Value value);
Value pop();
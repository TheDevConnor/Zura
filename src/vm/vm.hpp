#pragma once

#include "../compiler/value.hpp"
#include "../opCode/chunk.hpp"
#include "../types/type.hpp"
#include "../common.hpp"

using namespace Zura;

struct VM {
    Chunk* chunk;
    uint8_t* ip;

    Types::Value stack[256]; // 256 is the max stack size
    Types::Value* stackTop;
};

inline VM vm;

void initVM();
void freeVM();

Zura_Exit_Value interpret(const char* source);

Types::Value peek(int distance);
void push(Types::Value value);
Types::Value pop();
#pragma once

#include "../compiler/value.hpp"
#include "../opCode/chunk.hpp"
#include "../types/type.hpp"
#include "../common.hpp"

struct VM {
    Chunk* chunk;
    uint8_t* ip;

    Value stack[256]; // 256 is the max stack size
    Value* stackTop;
};

inline VM vm;

void push(Value value);
void resetStack();
void freeVM();
void initVM();

Zura_Exit_Value interpret(const char* source);

Value peek(int distance);
Value pop();


bool valuesEqual(Value a, Value b);
bool isFalsey(Value value);
bool isOpNumber();
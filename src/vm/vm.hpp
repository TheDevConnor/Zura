#pragma once

#include "../common.hpp"
#include "../compiler/value.hpp"
#include "../hash/table.hpp"
#include "../opCode/chunk.hpp"
#include "../types/type.hpp"

struct VM {
    Chunk*   chunk;
    uint8_t* ip;

    HashTable::Table strings;

    HashTable::Table globalNames;
    ValueArray       globalValues;

    Value  stack[256]; // 256 is the max stack size
    Value* stackTop;
    Obj*   objects;
};

inline VM vm;

void push(Value value);
void concatenate();
void resetStack();
void freeVM();
void initVM();

Zura::Exit_Value interpret(const char* source);

Value peek(int distance);
Value pop();

bool valuesEqual(Value a, Value b);
bool isFalsey(Value value);
bool isOpNumber();

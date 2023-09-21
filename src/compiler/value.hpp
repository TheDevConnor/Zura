#pragma once

#include "../types/type.hpp"
#include "../common.hpp"

using namespace Zura;

struct ValueArray {
    int capacity;
    int count;
    Types::Value* values;
};

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Types::Value value);
void freeValueArray(ValueArray* array);

void printValue(Types::Value value);
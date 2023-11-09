#pragma once

#include <cstddef>

#include "../common.hpp"
#include "../types/type.hpp"

struct ValueArray {
    size_t capacity;
    size_t count;
    Value* values;
};

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

void printValue(Value value);
void printType(Value value);

#include <iostream>
#include <cstdio>

#include "../memory/memory.hpp"
#include "../types/type.hpp"
#include "value.hpp"

using namespace Zura;

void initValueArray(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = nullptr;
}

void writeValueArray(ValueArray* array, Types::Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = Memory::GROW_CAPACITY(oldCapacity);
        array->values = Memory::GROW_ARRAY(array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    Memory::FREE_ARRAY(array->values, array->capacity);
    initValueArray(array);
}

void printValue(Types::Value value) {
    printf("%g", Types::AS_NUMBER(value));
}
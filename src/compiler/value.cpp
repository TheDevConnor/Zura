#include <iostream>
#include <cstdio>

#include "../memory/memory.hpp"
#include "value.hpp"

void initValueArray(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = nullptr;
}

void writeValueArray(ValueArray* array, Value value) {
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

void printValue(Value value) {
    std::cout << value;
}
#pragma once

#include <cstddef>

#include "../common.hpp"

constexpr int GROW_CAPACITY(int capacity) {
    return (capacity < 8) ? 8 : (capacity * 2);
}

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))
// template <typename T>
// T* GROW_ARRAY(T* pointer, size_t oldCount, size_t newCount) {
//     return (T*)reallocate(pointer, sizeof(T) * (oldCount), sizeof(T) * (newCount));
// }

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)
// !NOTE: I think this is the correct conversion not to sure though
// template <typename T>
// T* FREE_ARRAY(T* pointer, size_t oldCount) {
//     reallocate(pointer, sizeof(T) * (oldCount), 0);
// }

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

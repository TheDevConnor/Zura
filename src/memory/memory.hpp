#pragma once

#include <cstddef>

#include "../common.hpp"

#define FREE(type, pointer) Memory::reallocate(pointer, sizeof(type), 0)

class Memory {
public:
    template <typename T>
    static T* ALLOCATE(size_t count) {
        return static_cast<T*>(reallocate(nullptr, 0, sizeof(T) * (count)));
    }

    static constexpr size_t GROW_CAPACITY(size_t capacity) {
        return capacity < 8 ? 8 : capacity * 2;
    }

    template <typename T>
    static T* GROW_ARRAY(T* pointer, size_t oldCount, size_t newCount){
        return static_cast<T*>(reallocate(pointer, sizeof(T) * (oldCount), 
                            sizeof(T) * (newCount)));
    }

    template <typename T>
    static T* FREE_ARRAY(T* pointer, size_t oldCount){
        return static_cast<T*>(reallocate(pointer, sizeof(T) * (oldCount), 0));
    }

    static void freeObjects();
    static void *reallocate(void *pointer, size_t oldSize, size_t newSize);
};

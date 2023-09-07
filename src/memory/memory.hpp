#pragma once

#include <cstddef>

#include "../common.hpp"

class Memory {
public:
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

    static void *reallocate(void *pointer, size_t oldSize, size_t newSize);
};

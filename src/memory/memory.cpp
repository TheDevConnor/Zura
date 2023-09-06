#include <iostream>
#include <cstdlib>

#include "memory.hpp"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        delete[] static_cast<char*>(pointer);
        return nullptr;
    }

    void *result = realloc(pointer, newSize);
    if (result == nullptr) {
        ZuraExit(MEMORY_FAILURE);
        std::cerr << "Failed to reallocate memory!" << std::endl;
        std::exit(1);
    }
    return result;
}

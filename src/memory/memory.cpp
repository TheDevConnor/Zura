#include "memory.h"
#include <stdlib.h>

void* reallocate(void* pointer, size_t old_size, size_t new_size) {
    if (new_size > old_size) {}
    if (new_size == 0) {
        free(pointer);
        return nullptr;
    }

    void* new_pointer = realloc(pointer, new_size);
    if (new_pointer == nullptr) exit(1);
    return new_pointer;
}

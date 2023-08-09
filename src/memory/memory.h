#pragma once

#include <stdlib.h>

#include "../common.h"

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, old_count, new_count) \
    (type*)reallocate(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count))

#define FREE_ARRAY(type, pointer, old_count) \
    delete[] pointer;

void* reallocate(void* pointer, size_t old_size, size_t new_size);
void sweep();
void free_objects();

//    reallocate(pointer, sizeof(type) * (old_count), 0)

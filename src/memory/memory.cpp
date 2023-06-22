#include <stdlib.h>

#include "../parser/object.h"
#include "../parser/vm.h"
#include "memory.h"

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

static void free_obj(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            reallocate(object, sizeof(ObjString) + string->length + 1, 0);
            break;
        }
    }
}

void free_objects() {
    Obj* object = vm.objects;
    while (object != nullptr) {
        Obj* next = object->next;
        free_obj(object);
        object = next;
    }
}

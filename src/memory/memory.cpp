#include <stdlib.h>
#include <iostream>

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
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*) object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalue_count);
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*) object;
            free_chunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
            break;
        }
        case OBJ_STRING: {
            ObjString* string = reinterpret_cast<ObjString*>(object);
            reallocate(string, sizeof(ObjString) + string->length + 1, 0);
            break;
        }
        case OBJ_UPVALUE: {
            ObjClosure* closure = (ObjClosure*) object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalue_count);
            FREE(ObjUpvalue, object);
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

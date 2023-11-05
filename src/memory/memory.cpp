#include <iostream>
#include <cstdlib>
#include <ostream>

#include "../object/object.hpp"
#include "../vm/vm.hpp"
#include "memory.hpp"

void freeObject(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            Memory::FREE_ARRAY<char>(string->chars, string->length + 1);
            Memory::FREE(object);
            break;
        }
    }
}

void Memory::freeObjects() {
    Obj* object = vm.objects;
    while (object != nullptr) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}

void *Memory::reallocate(void *pointer, size_t oldSize, size_t newSize){
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
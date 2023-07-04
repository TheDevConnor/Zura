#include "helper/import.h"

#include "../memory/memory.h"
#include "object.h"
#include "table.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) \
    (type*)allocate_object(sizeof(type), object_type)

struct Obj* allocate_object(size_t size, ObjType type) {
    struct Obj* object = (struct Obj*)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object;
    return object;
}

ObjFunction* new_function() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = nullptr;
    init_chunk(&function->chunk);
    return function;
}

ObjString* allocate_string(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    table_set(&vm.strings, string, NIL_VAL);
    return string;
}

uint32_t hash_string(const char* key, int length) {
    uint32_t hash = 2166136261u;

    for (int i = 0; i < length; i++) {
        hash ^= key[i];
        hash *= 16777619;
    }

    return hash;
}

ObjString* take_string(char* chars, int length) {
    uint32_t hash = hash_string(chars, length);

    ObjString* existing_string = table_find_string(&vm.strings, chars, length, hash);
    if (existing_string != nullptr) {
        FREE_ARRAY(char, chars, length + 1);
        return existing_string;
    }

    return allocate_string(chars, length, hash);
}

ObjString* copy_string(const char* chars, int length) {
    uint32_t hash = hash_string(chars, length);

    ObjString* existing_string = table_find_string(&vm.strings, chars, length, hash);
    if (existing_string != nullptr) return existing_string;

    char* heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';

    return allocate_string(heap_chars, length, hash);
}

void print_function(ObjFunction* function) {
    if (function->name == nullptr) {
        std::cout << "<script>";
        return;
    }
    std::cout << "<fn " << function->name->chars;
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_FUNCTION:
            print_function(AS_FUNCTION(value));
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
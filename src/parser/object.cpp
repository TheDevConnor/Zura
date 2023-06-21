#include "helper/import.h"

#include "../memory/memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, object_type) \
    (type*)allocate_object(sizeof(type), object_type)

struct Obj* allocate_object(size_t size, ObjType type) {
    struct Obj* object = (struct Obj*)reallocate(NULL, 0, size);
    object->type = type;
    return object;
}

ObjString* allocate_string(char* chars, int length) {
    ObjString* string = ALLOCATE(ObjString, 1);
    string->length = length;
    string->chars = chars;
    return string;
}

ObjString* take_sting(char* chars, int length) {
    return allocate_string(chars, length);
}

ObjString* copy_string(const char* chars, int length) {
    char* heap_chars = ALLOCATE(char, length + 1);
    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';
    return allocate_string(heap_chars, length);
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            std::cout << AS_CSTRING(value);
            break;
    }
}
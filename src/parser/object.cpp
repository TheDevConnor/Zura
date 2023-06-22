#include "helper/import.h"

#include "../memory/memory.h"
#include "object.h"
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

ObjString* allocate_string(char* chars, int length) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    return string;
}

ObjString* take_string(int length) {
    ObjString* string = (ObjString*)allocate_object(
        sizeof(ObjString) + length + 1, OBJ_STRING);
    string->length = length;
    return string;
}

ObjString* copy_string(const char* chars, int length) {
    ObjString* string = take_string(length);

    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';

    return string;
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}
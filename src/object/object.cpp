#include <cstring>
#include <iostream>
#include <stdio.h>

#include "../hash/table.hpp"
#include "../memory/memory.hpp"
#include "../types/type.hpp"
#include "../vm/vm.hpp"
#include "object.hpp"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type)
{
    Obj* object  = (Obj*)Memory::reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    vm.objects   = object;
    return object;
}

static ObjString* allocateString(char* chars, int length, uint32_t hash)
{
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length    = length;
    string->chars     = chars;
    string->hash      = hash;
    HashTable::tableSet(&vm.strings, string, NIL_VAL());
    return string;
}

static uint32_t hashString(const char* key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* copyString(const char* chars, size_t length)
{
    uint32_t hash = hashString(chars, length);

    ObjString* interned = HashTable::findString(&vm.strings, chars, length, hash);
    if (interned != nullptr)
        return interned;

    char* heapChars = Memory::ALLOCATE<char>(length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocateString(heapChars, length, hash);
}

ObjString* takeString(char* chars, size_t length)
{
    uint32_t hash = hashString(chars, length);

    ObjString* interned = HashTable::findString(&vm.strings, chars, length, hash);
    if (interned != nullptr) {
        Memory::FREE_ARRAY<char>(chars, length + 1);
        return interned;
    }

    return allocateString(chars, length, hash);
}

void printObject(Value value)
{
    switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
        std::cout << AS_CSTRING(value);
        break;
    }
}

void concatenate()
{
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());

    size_t length = a->length + b->length;
    char*  chars  = Memory::ALLOCATE<char>(length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    push(OBJECT_VAL(result));
}

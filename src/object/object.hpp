#pragma once

#include "../types/type.hpp"
#include "../common.hpp"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

enum ObjType {
    OBJ_STRING,
};

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct  ObjString {
    Obj obj;
    int length;
    char* chars;
};

ObjString* copyString(const char* chars, int length);
ObjString* takeString(char* chars, int length);

void printObject(Value value);
void concatenate();

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJECT(value) && AS_OBJ(value)->type == type;
}

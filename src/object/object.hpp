#pragma once

#include "../types/type.hpp"
#include "../common.hpp"

enum ObjType {
    OBJ_STRING,
};

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct  ObjString {
    Obj obj;
    size_t length;
    char* chars;
    uint32_t hash;
};

ObjString* copyString(const char* chars, size_t length);
ObjString* takeString(char* chars, size_t length);

void printObject(Value value);
void concatenate();

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJECT(value) && AS_OBJ(value)->type == type;
}

inline ObjType OBJ_TYPE(Value value)     { return AS_OBJ(value)->type; }
inline bool IS_STRING(Value value)       { return isObjType(value, OBJ_STRING); }
inline ObjString* AS_STRING(Value value) { return (ObjString*)AS_OBJ(value); }
inline char* AS_CSTRING(Value value)     { return ((ObjString*)AS_OBJ(value))->chars; }

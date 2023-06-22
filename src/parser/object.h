#ifndef AZURA_OBJECT_H
#define AZURA_OBJECT_H

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value)   (AS_OBJ(value)->type)

#define IS_STRING(value)  is_obj_type(value, OBJ_STRING)

#define AS_STRING(value)  ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

enum ObjType {
    OBJ_STRING,
};

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct ObjString {
    Obj obj;
    int length;
    char* chars;
};

ObjString* take_string(char* chars, int length);
ObjString* copy_string(const char* chars, int length);

void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return IS_OBJECT(value) && AS_OBJ(value)->type == type;
}



#endif //AZURA_OBJECT_H
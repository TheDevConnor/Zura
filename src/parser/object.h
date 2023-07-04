#ifndef AZURA_OBJECT_H
#define AZURA_OBJECT_H

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_FUNCTION(value)  is_obj_type(value, OBJ_FUNCTION)
#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)

#define AS_FUNCTION(value)  ((ObjFunction*)AS_OBJ(value))
#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)
#define AS_MODULE(value)    ((ObjModule*)AS_OBJ(value))

enum ObjType {
    OBJ_FUNCTION,
    OBJ_STRING,
};

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct ObjFunction {
    Obj obj;
    int arity;
    Chunk chunk;
    ObjString* name;
};

struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

struct ObjModule {
    Obj obj;
    ObjString* name;
    Table variables;
};

ObjFunction* new_function();
ObjString* take_string(char* chars, int length);
ObjString* copy_string(const char* chars, int length);

void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return IS_OBJECT(value) && AS_OBJ(value)->type == type;
}



#endif //AZURA_OBJECT_H
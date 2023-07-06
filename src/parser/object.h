#ifndef AZURA_OBJECT_H
#define AZURA_OBJECT_H

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_CLOSURE(value)   is_obj_type(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)  is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    is_obj_type(value, OBJ_NATIVE)
#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)

#define AS_CLOSURE(value)   ((ObjClosure*)AS_OBJ(value))
#define AS_FUNCTION(value)  ((ObjFunction*)AS_OBJ(value))
#define AS_NATIVE(value)    (((ObjNative*)AS_OBJ(value))->function)
#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)
#define AS_MODULE(value)    ((ObjModule*)AS_OBJ(value))

enum ObjType {
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_NATIVE,
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

typedef bool (*NativeFn)(int arg_count, Value* args);

struct ObjNative {
    Obj obj;
    NativeFn function;
};

struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

struct ObjClosure {
    Obj obj;
    ObjFunction* function;
};


struct ObjModule {
    Obj obj;
    ObjString* name;
    Table variables;
    ObjFunction* function;
};

ObjClosure* new_closure(ObjFunction* function);
ObjFunction* new_function();
ObjNative* new_native(NativeFn function);
ObjString* take_string(char* chars, int length);
ObjString* copy_string(const char* chars, int length);

void print_object(Value value);

static inline bool is_obj_type(Value value, ObjType type) {
    return IS_OBJECT(value) && AS_OBJ(value)->type == type;
}

#endif //AZURA_OBJECT_H
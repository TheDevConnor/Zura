#pragma once

#include "../compiler/object.h"
#include "../compiler/value.h"
#include "../compiler/table.h"
#include "../parser/chunk.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

struct CallFrame {
    Obj* function;
    ObjClosure* closure;
    uint8_t* ip;
    Value* slots;
};

struct VM {
    CallFrame frames[FRAMES_MAX];
    int frame_count;

    Value stack[STACK_MAX];
    Value* stack_top;
    Table globals;
    Table strings;
    ObjString* init_string;
    ObjUpvalue* open_upvalues;

    size_t bytes_allocated;
    size_t next_gc;

    Table modules;
    Obj* objects;

    int gray_count;
    int gray_capacity;
    Obj** gray_stack;

    Table arrays;
    ValueArray array_values;
};

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

extern VM vm;

void init_vm();
void free_vm();

InterpretResult interpret(const char* source);
void push(Value value);
Value pop();
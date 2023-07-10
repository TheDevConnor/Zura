#ifndef AZURA_VM_H
#define AZURA_VM_H

#include "chunk.h"
#include "table.h"
#include "object.h"
#include "value.h"

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

#endif // AZURA_VM_H
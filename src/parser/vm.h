#ifndef AZURA_VM_H
#define AZURA_VM_H

#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

struct VM {
    Chunk* chunk;
    uint8_t* ip;

    Value stack[STACK_MAX];
    Value* stack_top;
    Table strings;
    Obj* objects;
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
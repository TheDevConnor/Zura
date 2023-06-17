#ifndef AZURA_VM_H
#define AZURA_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

struct VM {
    Chunk* chunk;
    uint8_t* ip;

    Value* stack;
    int stack_count;
    int stack_capacity;
};

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

void init_vm();
void free_vm();

InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif // AZURA_VM_H
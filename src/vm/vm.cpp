#include <iostream>

#include "../debug/debug.hpp"
#include "../common.hpp"
#include "vm_run.hpp"
#include "vm.hpp"

void initVM() {
    // this will reset the stackTop to the beginning of the stack
    vm.stackTop = vm.stack;
}
void freeVM() {}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

Zura_Exit_Value interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}
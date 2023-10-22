#include <iostream>

#include "../parser/compiler.hpp"
#include "../debug/debug.hpp"
#include "../types/type.hpp"
#include "../common.hpp"
#include "vm_run.hpp"
#include "vm.hpp"

using namespace Zura;

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

Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

Zura_Exit_Value interpret(const char *source) {
   Chunk chunk;
   initChunk(&chunk);

    if (!parserClass.compile(source, &chunk)) {
         freeChunk(&chunk);
         return COMPILATION_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    Zura_Exit_Value result = run();
    
    freeChunk(&chunk);
    return result;
}
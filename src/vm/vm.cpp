#include <iostream>
#include <cstring>

#include "../parser/compiler.hpp"
#include "../memory/memory.hpp"
#include "../object/object.hpp"
#include "../debug/debug.hpp"
#include "../types/type.hpp"
#include "../common.hpp"
#include "vm_run.hpp"
#include "vm.hpp"

using namespace Zura;

void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
    
    vm.objects = nullptr;

    HashTable::initTable(&vm.globals);
    HashTable::initTable(&vm.strings);
}

void freeVM() {
    HashTable::freeTable(&vm.globals);
    HashTable::freeTable(&vm.strings);

    Memory::freeObjects();
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;

    switch (a.type) {
        case Bool:   return AS_BOOL(a) == AS_BOOL(b);
        case Nil:    return true;
        case Number: return AS_NUMBER(a) == AS_NUMBER(b);
        case Object: return AS_OBJ(a) == AS_OBJ(b);
        default: return false; // Unreachable
    }
}

bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

bool isOpNumber() {
    // TODO: Add support for strings
    return IS_NUMBER(peek(0)) && IS_NUMBER(peek(1));
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}


Zura::Exit_Value interpret(const char *source) {
   Chunk chunk;
   initChunk(&chunk);

    if (!parserClass.compile(source, &chunk)) {
         freeChunk(&chunk);
         return Zura::Exit_Value::COMPILATION_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    Zura::Exit_Value result = run();
    
    freeChunk(&chunk);
    return result;
}

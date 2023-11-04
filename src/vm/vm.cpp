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
}

void freeVM() {
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
        case Object: {
            ObjString* aString = AS_STRING(a);
            ObjString* bString = AS_STRING(b);
            return aString->length == bString->length && 
                   memcmp(aString->chars, bString->chars, 
                          aString->length) == 0;
        }
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
#ifndef AZURA_NATIVE_H
#define AZURA_NATIVE_H

#include <iostream>
#include <cstring>
#include <ctime>

#include "../object.h"
#include "../vm.h"

void define_native(const char* name, NativeFn function) {
    push(OBJ_VAL(copy_string(name, (int)strlen(name))));
    push(OBJ_VAL(new_native(function)));
    table_set(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

Value clock_native(int arg_count, Value* args) {
    (void)args;
    (void)arg_count;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

void define_all_natives() {
    define_native("clock", clock_native);
}

#endif //AZURA_NATIVE_H
#pragma once

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#include "../compiler/object.h"
#include "../vm/vm.h"

class Natives {
    public:
        static Value define_native(const char* name, NativeFn function) {
            push(OBJ_VAL(copy_string(name, (int)strlen(name))));
            push(OBJ_VAL(new_native(function)));
            table_set(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
            Value value = vm.stack[1];
            pop();
            pop();
            return value;
        }
};
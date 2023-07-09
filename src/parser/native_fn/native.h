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

Value has_field_native(int arg_count, Value* args) {
    if (arg_count != 2) return BOOL_VAL(false);
    if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
    if (!IS_STRING(args[1])) return BOOL_VAL(false);

    ObjInstance* instance = AS_INSTANCE(args[0]);
    Value value;
    return BOOL_VAL(table_get(&instance->fields, AS_STRING(args[1]), &value));
}

Value get_field_native(int arg_count, Value* args) {
    if (arg_count != 2) return BOOL_VAL(false);
    if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
    if (!IS_STRING(args[1])) return BOOL_VAL(false);

    ObjInstance* instance = AS_INSTANCE(args[0]);
    Value value;
    table_get(&instance->fields, AS_STRING(args[1]), &value);
    return value;
}

Value set_field_native(int arg_count, Value* args) {
    if (arg_count != 3) return BOOL_VAL(false);
    if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
    if (!IS_STRING(args[1])) return BOOL_VAL(false);

    ObjInstance* instance = AS_INSTANCE(args[0]);
    table_set(&instance->fields, AS_STRING(args[1]), args[2]);
    return args[2];
}

Value delete_field_native(int arg_count, Value* args) {
    if (arg_count != 2) return BOOL_VAL(false);
    if (!IS_INSTANCE(args[0])) return BOOL_VAL(false);
    if (!IS_STRING(args[1])) return BOOL_VAL(false);

    ObjInstance* instance = AS_INSTANCE(args[0]);
    table_delete(&instance->fields, AS_STRING(args[1]));
    return NIL_VAL;
}

Value clock_native(int arg_count, Value* args) {
    (void)arg_count;
    (void)args;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

Value sleep_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

    double seconds = AS_NUMBER(args[0]);
    clock_t start = clock();
    while ((clock() - start) / CLOCKS_PER_SEC < seconds);
    return NIL_VAL;
}

Value to_string_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

    char buffer[100];
    sprintf(buffer, "%g", AS_NUMBER(args[0]));
    return OBJ_VAL(copy_string(buffer, (int)strlen(buffer)));
}

Value exit_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

    exit((int)AS_NUMBER(args[0]));
    return NIL_VAL;
}

void define_all_natives() {
    define_native("clock", clock_native);
    define_native("exit", exit_native);
    define_native("sleep", sleep_native);

    define_native("to_string", to_string_native);

    define_native("has_field", has_field_native);
    define_native("get_field", get_field_native);
    define_native("set_field", set_field_native);
    define_native("delete_field", delete_field_native);
}

#endif //AZURA_NATIVE_H
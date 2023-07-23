#ifndef AZURA_NATIVE_H
#define AZURA_NATIVE_H

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#include "../../vm/vm.h"
#include "../object.h"

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
    sprintf(buffer, "%lld", AS_NUMBER(args[0]));
    return OBJ_VAL(copy_string(buffer, (int)strlen(buffer)));
}

Value to_number_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_STRING(args[0])) return BOOL_VAL(false);

    char* string = AS_CSTRING(args[0]);
    char* end;
    errno = 0;
    long long number = strtoll(string, &end, 10);
    if (errno == ERANGE) {
        return BOOL_VAL(false);
    }
    return NUMBER_VAL(number);
}

Value exit_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

    exit((int)AS_NUMBER(args[0]));
    return NIL_VAL;
}

Value read_file_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_STRING(args[0])) return BOOL_VAL(false);

    ObjString* path = AS_STRING(args[0]);
    FILE* file = fopen(path->chars, "rb");
    if (file == NULL) {
        return NIL_VAL;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NIL_VAL;
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        free(buffer);
        fclose(file);
        return NIL_VAL;
    }

    buffer[bytes_read] = '\0';
    fclose(file);
    return OBJ_VAL(copy_string(buffer, (int)bytes_read));
}

Value len_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_STRING(args[0])) return BOOL_VAL(false);

    ObjString* string = AS_STRING(args[0]);
    double number_length = string->length;
    return NUMBER_VAL(number_length);
}

Value squrt_native(int arg_count, Value* args) {
    if (arg_count != 1) return BOOL_VAL(false);
    if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_squrt = sqrt(number);
    return NUMBER_VAL(number_squrt);
}

void define_all_natives() {
    define_native("read_file", read_file_native);

    define_native("len", len_native);
    define_native("clock", clock_native);
    define_native("exit", exit_native);
    define_native("sleep", sleep_native);

    define_native("to_string", to_string_native);
    define_native("to_number", to_number_native);

    define_native("has_field", has_field_native);
    define_native("get_field", get_field_native);
    define_native("set_field", set_field_native);
    define_native("delete_field", delete_field_native);

    define_native("sqrt", squrt_native);
}

#endif //AZURA_NATIVE_H
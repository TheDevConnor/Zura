#pragma once 

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class Fs {
    private:
    static Value read_file_native(int arg_count, Value* args) {
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

    public:
    static void define_filesystem_natives() {
        Natives::define_native("fs_readFile", read_file_native);
    }
};
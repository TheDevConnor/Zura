#pragma once

#include <cmath>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class Fs {
private:
  static Value read_file_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);

    ObjString *path = AS_STRING(args[0]);
    FILE *file = fopen(path->chars, "rb");
    if (file == NULL) {
      return NIL_VAL;
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
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

    delete file;
    delete path;

    Value ret = OBJ_VAL(copy_string(buffer, (int)bytes_read));
    
    free(buffer);

    return ret;
  }
  static Value write_file_native(int arg_count, Value *args) {
    if (arg_count != 2)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);
    if (!IS_STRING(args[1]))
      return BOOL_VAL(false);

    ObjString *path = AS_STRING(args[0]);
    ObjString *content = AS_STRING(args[1]);

    FILE *file = fopen(path->chars, "wb");
    if (file == NULL) {
      return NIL_VAL;
    }

    size_t bytes_written =
        fwrite(content->chars, sizeof(char), content->length, file);
    if (bytes_written < (size_t)content->length) {
      fclose(file);
      return NIL_VAL;
    }

    fclose(file);

    delete file;
    delete path;
    delete content;

    return BOOL_VAL(true);
  }
  static Value generate_file_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);

    ObjString *path = AS_STRING(args[0]);
    FILE *file = fopen(path->chars, "wb");
    if (file == NULL) {
      return NIL_VAL;
    }

    fclose(file);

    delete file;

    return BOOL_VAL(true);
  }
  static Value delete_file_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);

    ObjString *path = AS_STRING(args[0]);
    if (remove(path->chars) != 0) {
      return NIL_VAL;
    }

    delete path;

    return BOOL_VAL(true);
  }

public:
  static void define_filesystem_natives() {
    Natives::define_native("fsReadFile", read_file_native);
    Natives::define_native("fsWriteFile", write_file_native);
    Natives::define_native("fsGenerateFile", generate_file_native);
    Natives::define_native("fsDeleteFile", delete_file_native);
  }
};

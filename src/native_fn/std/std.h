#pragma once

#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class StdFunc {
private:
  static Value clock_native(int arg_count, Value *args) {
    (void)arg_count;
    (void)args;
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
  }

  static Value len_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);

    ObjString *string = AS_STRING(args[0]);
    double number_length = string->length;
    return NUMBER_VAL(number_length);
  }

  static Value to_string_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    char buffer[100];
    sprintf(buffer, "%f", AS_NUMBER(args[0]));
    return OBJ_VAL(copy_string(buffer, (int)strlen(buffer)));
  }

  static Value to_number_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_STRING(args[0]))
      return BOOL_VAL(false);

    char *string = AS_CSTRING(args[0]);
    char *end;
    errno = 0;
    double number = (double)strtoll(string, &end, 10);
    if (errno == ERANGE) {
      return BOOL_VAL(false);
    }
    return NUMBER_VAL(number);
  }

public:
  static void define_std_natives() {
    Natives::define_native("len", len_native);
    Natives::define_native("clock", clock_native);
    Natives::define_native("toString", to_string_native);
    Natives::define_native("toNumber", to_number_native);
  }
};

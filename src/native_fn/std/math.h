#pragma once

#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdint.h>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class Math {
private:
  static Value random_native(int arg_count, Value *args) {
    (void)args;
    if (arg_count != 0)
      return BOOL_VAL(false);

    srand((unsigned int)time(NULL));
    double random = (double)rand() / RAND_MAX;
    return NUMBER_VAL(random);
  }
  static Value random_range_native(int arg_count, Value *args) {
    if (arg_count != 2)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1]))
      return BOOL_VAL(false);

    double min = AS_NUMBER(args[0]);
    double max = AS_NUMBER(args[1]);

    double random = min + (rand() % (int)(max - min + 1));
    return NUMBER_VAL(random);
  }

  static Value round_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_round = round(number);
    return NUMBER_VAL(number_round);
  }
  static Value floor_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_floor = floor(number);
    return NUMBER_VAL(number_floor);
  }
  static Value ceil_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_ceil = ceil(number);
    return NUMBER_VAL(number_ceil);
  }
  static Value abs_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_abs = abs(number);
    return NUMBER_VAL(number_abs);
  }

  static Value sqrt_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_squrt = sqrt(number);
    return NUMBER_VAL(number_squrt);
  }

  static Value log_native(int arg_count, Value *args) {
    if (arg_count != 2)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double base = AS_NUMBER(args[1]);
    double number_log = log(number) / log(base);
    return NUMBER_VAL(number_log);
  }
  static Value log10_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_log10 = log10(number);
    return NUMBER_VAL(number_log10);
  }

  static Value sin_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_sin = sin(number);
    return NUMBER_VAL(number_sin);
  }
  static Value cos_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_cos = cos(number);
    return NUMBER_VAL(number_cos);
  }
  static Value tan_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_tan = tan(number);
    return NUMBER_VAL(number_tan);
  }

  static Value csc_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_csc = 1 / sin(number);
    return NUMBER_VAL(number_csc);
  }
  static Value sec_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_sec = 1 / cos(number);
    return NUMBER_VAL(number_sec);
  }
  static Value cot_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_cot = 1 / tan(number);
    return NUMBER_VAL(number_cot);
  }

  static Value asin_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_asin = asin(number);
    return NUMBER_VAL(number_asin);
  }
  static Value acos_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_acos = acos(number);
    return NUMBER_VAL(number_acos);
  }
  static Value atan_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    double number = AS_NUMBER(args[0]);
    double number_atan = atan(number);
    return NUMBER_VAL(number_atan);
  }
  static double fastPow(int32_t a, int32_t b) {
    union {
      double d;
      int x[2];
    } u = { static_cast<double>(a) };
    u.x[1] = (int32_t)(b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
    return u.d;
  }
  static int32_t fib(int32_t input) {
    int32_t sqrt5 = sqrt(5);
    return (fastPow(1 + sqrt5, input) - fastPow(1 - sqrt5, input)) / fastPow(2, input) / sqrt5;
  }
  static Value fib_native(int arg_count, Value *args) {
    if (arg_count != 1)
      return BOOL_VAL(false);
    if (!IS_NUMBER(args[0]))
      return BOOL_VAL(false);

    int32_t number = AS_NUMBER(args[0]);
    int32_t number_fib = fib(number);
    return NUMBER_VAL(static_cast<double>(number_fib));
  }

public:
  static void define_math_natives() {
    Natives::define_native("mathRandom", random_native);
    Natives::define_native("mathRandomRange", random_range_native);

    Natives::define_native("mathRound", round_native);
    Natives::define_native("mathFloor", floor_native);
    Natives::define_native("mathCeil", ceil_native);
    Natives::define_native("mathAbs", abs_native);

    Natives::define_native("mathLog", log_native);
    Natives::define_native("mathLog10", log10_native);

    Natives::define_native("mathSqrt", sqrt_native);

    Natives::define_native("mathSin", sin_native);
    Natives::define_native("mathCos", cos_native);
    Natives::define_native("mathTan", tan_native);

    Natives::define_native("mathCsc", csc_native); // Cosecant
    Natives::define_native("mathSec", sec_native); // Secant
    Natives::define_native("mathCot", cot_native); // Cotangent

    Natives::define_native("mathAsin", asin_native); // Arch Sine
    Natives::define_native("mathAcos", acos_native); // Arch Cosine
    Natives::define_native("mathAtan", atan_native); // Arch Tangent
    Natives::define_native("mathFastFib", fib_native);
  }
};

#pragma once 

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class Math {
    private:
    static Value random_native(int arg_count, Value* args) {
        (void)args;
        if (arg_count != 0) return BOOL_VAL(false);

        srand(time(NULL));
        double random = (double)rand() / RAND_MAX;
        return NUMBER_VAL(random);
    }
    static Value random_range_native(int arg_count, Value* args) {
        if (arg_count != 2) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) return BOOL_VAL(false);

        double min = AS_NUMBER(args[0]);
        double max = AS_NUMBER(args[1]);

        double random = min + (rand() % (int)(max - min + 1));
        return NUMBER_VAL(random);
    }

    static Value round_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_round = round(number);
        return NUMBER_VAL(number_round);
    }
    static Value floor_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_floor = floor(number);
        return NUMBER_VAL(number_floor);
    }
    static Value ceil_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_ceil = ceil(number);
        return NUMBER_VAL(number_ceil);
    }
    static Value abs_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_abs = abs(number);
        return NUMBER_VAL(number_abs);
    }

    static Value sqrt_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_squrt = sqrt(number);
        return NUMBER_VAL(number_squrt);
    }

    static Value log_native(int arg_count, Value* args) {
        if (arg_count != 2) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0]) || !IS_NUMBER(args[1])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double base = AS_NUMBER(args[1]);
        double number_log = log(number) / log(base);
        return NUMBER_VAL(number_log);
    }
    static Value log10_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);
        
        double number = AS_NUMBER(args[0]);
        double number_log10 = log10(number);
        return NUMBER_VAL(number_log10);
    }

    static Value sin_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_sin = sin(number);
        return NUMBER_VAL(number_sin);
    }
    static Value cos_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_cos = cos(number);
        return NUMBER_VAL(number_cos);
    }
    static Value tan_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_tan = tan(number);
        return NUMBER_VAL(number_tan);
    }

    static Value csc_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_csc = 1 / sin(number);
        return NUMBER_VAL(number_csc);
    }
    static Value sec_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_sec = 1 / cos(number);
        return NUMBER_VAL(number_sec);
    }
    static Value cot_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_cot = 1 / tan(number);
        return NUMBER_VAL(number_cot);
    }

    static Value asin_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_asin = asin(number);
        return NUMBER_VAL(number_asin);
    }
    static Value acos_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_acos = acos(number);
        return NUMBER_VAL(number_acos);
    }
    static Value atan_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_atan = atan(number);
        return NUMBER_VAL(number_atan);
    }

    public:
    static void define_math_natives() {
        Natives::define_native("math_random", random_native);
        Natives::define_native("math_random_range", random_range_native);

        Natives::define_native("math_round", round_native);
        Natives::define_native("math_floor", floor_native);
        Natives::define_native("math_ceil", ceil_native);
        Natives::define_native("math_abs", abs_native);

        Natives::define_native("math_log", log_native);
        Natives::define_native("math_log10", log10_native);

        Natives::define_native("math_sqrt", sqrt_native);

        Natives::define_native("math_sin", sin_native);
        Natives::define_native("math_cos", cos_native);
        Natives::define_native("math_tan", tan_native);

        Natives::define_native("math_csc", csc_native); // Cosecant
        Natives::define_native("math_sec", sec_native); // Secant
        Natives::define_native("math_cot", cot_native); // Cotangent

        Natives::define_native("math_asin", asin_native); // Arch Sine
        Natives::define_native("math_acos", acos_native); // Arch Cosine
        Natives::define_native("math_atan", atan_native); // Arch Tangent
    }
};
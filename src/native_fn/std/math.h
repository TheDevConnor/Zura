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
    static Value sqrt_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_squrt = sqrt(number);
        return NUMBER_VAL(number_squrt);
    }

    static Value sin_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_sin = sin(number);
        std::cout << number_sin << "\n";
        return NUMBER_VAL(number_sin);
    }
    static Value cos_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_cos = cos(number);
        std::cout << number_cos << "\n";
        return NUMBER_VAL(number_cos);
    }
    static Value tan_native(int arg_count, Value* args) {
        if (arg_count != 1) return BOOL_VAL(false);
        if (!IS_NUMBER(args[0])) return BOOL_VAL(false);

        double number = AS_NUMBER(args[0]);
        double number_tan = tan(number);
        std::cout << number_tan << "\n";
        return NUMBER_VAL(number_tan);
    }

    public:
    static void define_math_natives() {
        Natives::define_native("math_sqrt", sqrt_native);
        Natives::define_native("math_sin", sin_native);
        Natives::define_native("math_cos", cos_native);
        Natives::define_native("math_tan", tan_native);
    }
};
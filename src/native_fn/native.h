#pragma once

#include <string>

#include "std/math.h"
#include "std/filesystem.h"
#include "std/std.h"


void define_native(std::string native_name) {
    if (native_name == "fs") {
        Fs::define_filesystem_natives();
    }
    if (native_name == "math") {
        Math::define_math_natives();
    }
    if (native_name == "std") {
        StdFunc::define_std_natives();
    }
}
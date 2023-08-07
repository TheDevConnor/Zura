#pragma once

#include <string>

#include "std/math.h"
#include "std/logger.h"
#include "std/filesystem.h"
#include "std/std.h"
#include "std/os.h"


void define_native(std::string native_name) {
    if (native_name == "os") {
        Os::define_os_natives();
    }
    if (native_name == "fs") {
        Fs::define_filesystem_natives();
    }
    if (native_name == "math") {
        Math::define_math_natives();
    }
    if (native_name == "std") {
        StdFunc::define_std_natives();
    }
    if (native_name == "logger") {
        Logger::define_logger_natives();
    }
}
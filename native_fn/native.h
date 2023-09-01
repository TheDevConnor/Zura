#pragma once

#include <string>

#include "std/filesystem.h"
#include "std/logger.h"
#include "std/math.h"
#include "std/std.h"
#include "std/network.h"

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
  if (native_name == "logger") {
    Logger::define_logger_natives();
  }
  if (native_name == "network") {
    NetworkFunc::define_network_natives();
  }
}

#pragma once

#include "../compiler/object.h"
#include "../vm/vm.h"

ObjFunction *compile(const char *source);
void mark_compiler_roots();

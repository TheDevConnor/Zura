#pragma once

#include "../common.h"
#include "../compiler/object.h"
#include "../compiler/table.h"
#include "../compiler/value.h"
#include "../vm/vm.h"

void mark_object(Obj* object);
void mark_value(Value value);
void mark_roots();
void collect_garbage();

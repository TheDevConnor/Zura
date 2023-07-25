#ifndef AZURA_GC_H
#define AZURA_GC_H

#include "../compiler/object.h"
#include "../compiler/value.h"
#include "../compiler/table.h"
#include "../vm/vm.h"
#include "../common.h"

void mark_object(Obj* object);
void mark_value(Value value);
void mark_roots();
void collect_garbage();

#endif // AZURA_GC_H
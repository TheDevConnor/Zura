#ifndef AZUA_VALUE_H
#define AZUA_VALUE_H

#include "../memory/memory.h"

typedef double Value;

struct ValueArray {
    int capacity;
    int count;
    Value* values;
};

void print_value(Value value);
void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);

#endif // AZUA_VALUE_H

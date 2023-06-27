#include <iostream>
#include <string.h>

#include "object.h"
#include "value.h"

void print_value(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            std::cout << (value.as.boolean ? "true" : "false");
            break;
        case VAL_NIL:    std::cout << "nil";           break;
        case VAL_NUMBER: std::cout << value.as.number; break;
        case VAL_OBJ:    print_object(value);          break;
    }
}

bool values_equal(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:    return AS_OBJ(a) == AS_OBJ(b);
        default:         return false; // Unreachable.
    }
}

void init_value_array(ValueArray* array) {
    array->values = nullptr;
    array->capacity = 0;
    array->count = 0;
}

void write_value_array(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void free_value_array(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

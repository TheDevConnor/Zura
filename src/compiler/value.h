#pragma once

#include <cstring>
#include <iostream>

#include "../memory/memory.h"

typedef struct Obj obj;
typedef struct ObjArray obj_array;
typedef struct ObjString obj_string;

#ifdef NAN_BOXING

#define SIGN_BIT ((uint64_t)0x8000000000000000)
#define QNAN ((uint64_t)0x7ffc000000000000)

#define TAG_NIL 1   // 01
#define TAG_FALSE 2 // 10
#define TAG_TRUE 3  // 11

typedef uint64_t Value;

#define FALSE_VAL ((Value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL ((Value)(uint64_t)(QNAN | TAG_TRUE))

#define IS_BOOL(value) (((value)&FALSE_VAL) == FALSE_VAL)
#define IS_NIL(value) ((value) == NIL_VAL)
#define IS_NUMBER(value) (((value)&QNAN) != QNAN)
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define AS_BOOL(value) ((value) == TRUE_VAL)
#define AS_NUMBER(value) value_to_num(value)
#define AS_OBJ(value) ((Obj *)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

#define BOOL_VAL(b) ((b) ? TRUE_VAL : FALSE_VAL)
#define NIL_VAL ((Value)(uint64_t)(QNAN | TAG_NIL))
#define NUMBER_VAL(num) num_to_value(num)
#define OBJ_VAL(obj) (Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj))

inline Value value_to_num(Value value) {
  double num;
  memcpy(&num, &value, sizeof(Value));
  // std::cout << " " << num << std::endl;
  return num;
}

inline Value num_to_value(double num) {
  Value value;
  memcpy(&value, &num, sizeof(double));
  // std::cout << " "<< num << std::endl;
  return value;
}
#else

enum ValueType { VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ, VAL_ARRAY };

struct Value {
  ValueType type;
  union {
    obj_array *array;
    double number;
    bool boolean;
    Obj *obj;
  } as;
};

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)
#define IS_ARRAY(value)   ((value).type == VAL_ARRAY)

#define AS_BOOL(value)   ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value)    ((value).as.obj)
#define AS_ARRAY(value)  ((value).as.array)

#define BOOL_VAL(value)   Value{VAL_BOOL, {.boolean = value}}
#define NIL_VAL           Value{VAL_NIL, {.number = 0}}
#define NUMBER_VAL(value) Value{VAL_NUMBER, {.number = value}}
#define OBJ_VAL(object)   Value{VAL_OBJ, {.obj = (Obj *)object}}
#define ARRAY_VAL(object) Value{VAL_ARRAY, {.obj = (Obj *)object}}

#endif

struct ValueArray {
  int capacity;
  int count;
  Value *values;
};

bool values_equal(Value a, Value b);
void print_value(Value value);
void init_value_array(ValueArray *array);
void write_value_array(ValueArray *array, Value value);
void free_value_array(ValueArray *array);

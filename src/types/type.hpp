#pragma once

#include <string>

typedef struct Obj Obj;
typedef struct ObjString ObjString;

struct Types {
  int asInt;
  float asFloat;
  std::string asString;
  bool asBool;
  void* asAny;
};

enum ValueType {
    Bool, 
    Number,
    Object,
    Nil,
    NaN
};

struct Value {

    ValueType type;

    union {
        bool boolean;
        double number;
        Obj* obj;
    } as;

};

constexpr Value UNDEFINED_VAL()          { return Value{Nil, {.number = NAN}};      }
constexpr bool IS_UNDEFINED(Value value) { return (value).type == Nil;              }

constexpr Value BOOL_VAL(bool value)     { return Value{Bool, {.boolean = value}};  }
constexpr Value NUMBER_VAL(double value) { return Value{Number, {.number = value}}; }
constexpr Value NIL_VAL()                { return Value{Nil, {.number = 0}};        }
template<typename T>
constexpr Value OBJECT_VAL(T* object)    { return Value{Object, {.obj = (Obj*)object}}; }

constexpr bool AS_BOOL(Value value)      { return (value).as.boolean; }
constexpr double AS_NUMBER(Value value)  { return (value).as.number;  }
constexpr Obj* AS_OBJ(Value value)       { return (value).as.obj;     }

constexpr bool IS_BOOL(Value value)      { return (value).type == Bool;   }
constexpr bool IS_NUMBER(Value value)    { return (value).type == Number; }
constexpr bool IS_NIL(Value value)       { return (value).type == Nil;    }
constexpr bool IS_OBJECT(Value value)    { return (value).type == Object; }

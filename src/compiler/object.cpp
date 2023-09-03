#include "../parser/helper/import.h"

#include "../memory/memory.h"
#include "../vm/vm.h"
#include "object.h"
#include "table.h"

using namespace std;

#define ALLOCATE_OBJ(type, object_type)                                        \
  (type *)allocate_object(sizeof(type), object_type)

struct Obj *allocate_object(size_t size, ObjType type) {
  struct Obj *object = (struct Obj *)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;

#ifndef DEBUG_LOG_GC
  cout << (void *)object << " allocate " << size << " for " << type << " bytes"
       << endl;
#endif

  return object;
}

ObjBoundMethod *new_bound_method(Value receiver, ObjClosure *method) {
  ObjBoundMethod *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
  bound->receiver = receiver;
  bound->method = method;
  return bound;
}

ObjClass *new_class(ObjString *name) {
  ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
  klass->name = name;
  klass->initializer = NIL_VAL;
  init_table(&klass->methods);
  return klass;
}

ObjClosure *new_closure(ObjFunction *function) {
  ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalue_count);
  for (int i = 0; i < function->upvalue_count; i++)
    upvalues[i] = nullptr;

  ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;
  closure->upvalues = upvalues;
  closure->upvalue_count = function->upvalue_count;
  return closure;
}

ObjFunction *new_function() {
  ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->upvalue_count = 0;
  function->arity = 0;
  function->name = nullptr;
  init_chunk(&function->chunk);
  return function;
}

ObjInstance *new_instance(ObjClass *klass) {
  ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
  instance->klass = klass;
  init_table(&instance->fields);
  return instance;
}

ObjNative *new_native(NativeFn function) {
  ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}

ObjString *allocate_string(char *chars, int length, uint32_t hash) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  string->hash = hash;

  push(OBJ_VAL(string));
  table_set(&vm.strings, string, NIL_VAL);
  pop();
  return string;
}

uint32_t hash_string(const char *key, int length) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < length; i++) {
    hash ^= key[i];
    hash *= 16777619;
  }

  return hash;
}

ObjString *take_string(char *chars, int length) {
  uint32_t hash = hash_string(chars, length);

  ObjString *existing_string =
      table_find_string(&vm.strings, chars, length, hash);
  if (existing_string != nullptr) {
    FREE_ARRAY(char, chars, length + 1);
    return existing_string;
  }

  return allocate_string(chars, length, hash);
}

ObjString *copy_string(const char *chars, int length) {
  uint32_t hash = hash_string(chars, length);

  ObjString *existing_string =
      table_find_string(&vm.strings, chars, length, hash);
  if (existing_string != nullptr)
    return existing_string;

  char *heap_chars = ALLOCATE(char, length + 1);
  memcpy(heap_chars, chars, length);
  heap_chars[length] = '\0';

  ObjString *string = allocate_string(heap_chars, length, hash);

  // free(heap_chars);

  return string;
}

ObjArray* new_array() {
  ObjArray* array = ALLOCATE(ObjArray, 1);
  array->values = nullptr;
  array->capacity = 0;
  array->count = 0;
  return array;
}

Value array_read(ObjArray* array, int val) {
  if (val >= array->count) return NIL_VAL;
  return array->values[val];
}

ObjArray* array_write(ObjArray* array, int val, Value value) {
  if (array->capacity < val + 1) {
    int old_capacity = array->capacity;
    array->capacity = GROW_CAPACITY(old_capacity);
    array->values = GROW_ARRAY(Value, array->values, old_capacity, array->capacity);
  }
  array->values[val] = value;
  array->count = val + 1;
  return array;
}

ObjUpvalue *new_upvalue(Value *slot) {
  ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
  upvalue->closed = NIL_VAL;
  upvalue->location = slot;
  upvalue->next = nullptr;
  return upvalue;
}

ObjStruct *new_struct(ObjString *name) {
  ObjStruct *structure = ALLOCATE_OBJ(ObjStruct, OBJ_STRUCT);
  structure->name = name;
  init_table(&structure->fields);
  return structure; 
}

void print_function(ObjFunction *function) {
  if (function->name == nullptr) {
    cout << "<script " << function->name << ">";
    return;
  }
  cout << "<fn " << function->name->chars << ">";
}

void print_object(Value value) {
  switch (OBJ_TYPE(value)) {
  case OBJ_BOUND_METHOD:
    print_function(AS_BOUND_METHOD(value)->method->function);
    break;
  case OBJ_CLASS:
    cout << AS_CLASS(value)->name->chars;
    break;
  case OBJ_CLOSURE:
    print_function(AS_CLOSURE(value)->function);
    break;
  case OBJ_FUNCTION:
    print_function(AS_FUNCTION(value));
    break;
  case OBJ_INSTANCE:
    cout << AS_INSTANCE(value)->klass->name->chars << " instance";
    break;
  case OBJ_NATIVE:
    cout << "<native fn>";
    break;
  case OBJ_STRING:
    for (int i = 0; i < AS_STRING(value)->length; i++) {
      if (AS_STRING(value)->chars[i] == '\\') {
        switch (AS_STRING(value)->chars[i + 1]) {
        case 'n': // newline
          cout << '\n';
          break;
        case 't': // tab
          cout << '\t';
          break;
        case 'r': // carriage return
          cout << '\r';
          break;
        default:
          cout << AS_STRING(value)->chars[i + 1];
          break;
        }
        i++;
      } else {
        cout << AS_STRING(value)->chars[i];
      }
    }
    break;
  case OBJ_UPVALUE:
    cout << "upvalue";
    break;
  case OBJ_STRUCT: {
    ObjStruct* structure = AS_STRUCT(value);
    cout << structure->name->chars << " { ";
    for (int i = 0; i < structure->fields.capacity; i++) {
      Entry* entry = &structure->fields.entries[i];
      if (entry->key != nullptr) {
        cout << entry->key->chars << ": ";
        print_value(entry->value);
        cout << ", ";
      }
    }
    cout << "}";
    break;
  }
  }
}

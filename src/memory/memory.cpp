#include <iostream>
#include <stdlib.h>

#include "../compiler/object.h"
#include "../garbage_collector/gc.h"
#include "../vm/vm.h"
#include "memory.h"

using namespace std;

void *reallocate(void *pointer, size_t old_size, size_t new_size) {
  vm.bytes_allocated += new_size - old_size;
  if (new_size > old_size) {
    mark_roots();
  }

  if (new_size == 0) {
    return nullptr;
  }

  void *new_pointer = realloc(pointer, new_size);
  if (new_pointer == nullptr){
    cerr << "ERROR: Failed to reallocate memory.\n";
    exit(1);
  }
  return new_pointer;
}

static void free_obj(Obj *object) {
#ifndef DEBUG_LOG_GC
  cout << (void *)object << " free type " << object->type << endl;
#endif

  switch (object->type) {
    case OBJ_BOUND_METHOD: {
      FREE(ObjBoundMethod, object);
      break;
    }
    case OBJ_CLASS: {
      ObjClass *klass = (ObjClass *)object;
      free_table(&klass->methods);
      break;
    }
    case OBJ_CLOSURE: {
      ObjClosure *closure = (ObjClosure *)object;
      FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalue_count);
      FREE(ObjClosure, object);
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction *function = (ObjFunction *)object;
      free_chunk(&function->chunk);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_INSTANCE: {
      ObjInstance *instance = (ObjInstance *)object;
      free_table(&instance->fields);
      FREE(ObjInstance, object);
      break;
    }
    case OBJ_NATIVE: {
      FREE(ObjNative, object);
      break;
    }
    case OBJ_STRING: {
      ObjString *string = reinterpret_cast<ObjString *>(object);
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, string);
      break;
    }
    case OBJ_UPVALUE: {
      ObjClosure *closure = (ObjClosure *)object;
      FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalue_count);
      FREE(ObjUpvalue, object);
      break;
    }
  }
}

void free_objects() {
  // call garbage collector
  collect_garbage();
  // free memory
  for(Obj* i = vm.objects; i; i = i->next){
      free_obj(i);
  }
}

void sweep() {
  Obj *prev = nullptr;
  Obj *obj = vm.objects;
  while (obj != nullptr) {
    if (obj->is_marked) {
      obj->is_marked = false;
      prev = obj;
      obj = obj->next;
    } else {
      Obj *unreached = obj;
      obj = obj->next;

      if (prev != nullptr)
        prev->next = obj;
      else
        vm.objects = obj;

      free_obj(unreached);
    }
  }
}

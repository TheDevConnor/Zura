#include "gc.h"
#include "../parser/parser.h"
#include <queue>
#include <unordered_set>

using namespace std;

#ifndef DEBUG_LOG_GC
#include "../debug/debug.h"
#include <iostream>
#include <stdio.h>
#endif

#define GC_HEAP_GROW_FACTOR 2

void mark_object(Obj *object) {
  if (object == nullptr)
    return;
  if (object->is_marked)
    return;

#ifndef DEBUG_LOG_GC
  cout << (void *)object << " mark ";
  print_value(OBJ_VAL(object));
  cout << endl;
#endif

  object->is_marked = true;

  if (vm.gray_capacity < vm.gray_count + 1) {
    vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
    vm.gray_stack =
        (Obj **)realloc(vm.gray_stack, sizeof(Obj *) * vm.gray_capacity);

    if (vm.gray_stack == nullptr)
      exit(1);
  }
  vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value) {
  if (IS_OBJ(value))
    mark_object(AS_OBJ(value));
}

void mark_array(ValueArray *array) {
  for (int i = 0; i < array->count; i++) {
    mark_value(array->values[i]);
  }
}

void mark_roots() {
  for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    mark_value(*slot);
  }

  for (int i = 0; i < vm.frame_count; i++) {
    mark_object((Obj *)vm.frames[i].closure);
  }

  for (ObjUpvalue *upvalue = vm.open_upvalues; upvalue != nullptr;
       upvalue = upvalue->next) {
    mark_object((Obj *)upvalue);
  }

  mark_table(&vm.globals);
  mark_compiler_roots();
  mark_object((Obj *)vm.init_string);
}

void blacken_object(Obj *object) {
#ifndef DEBUG_LOG_GC
  cout << (void *)object << " blacken";
  print_value(OBJ_VAL(object));
  cout << endl;
#endif

  switch (object->type) {
  case OBJ_BOUND_METHOD: {
    ObjBoundMethod *bound = (ObjBoundMethod *)object;
    mark_value(bound->receiver);
    mark_object((Obj *)bound->method);
    break;
  }
  case OBJ_CLASS: {
    ObjClass *klass = (ObjClass *)object;
    mark_object((Obj *)klass->name);
    mark_table(&klass->methods);
    break;
  }
  case OBJ_CLOSURE: {
    ObjClosure *closure = (ObjClosure *)object;
    mark_object((Obj *)closure->function);

    for (int i = 0; i < closure->upvalue_count; i++) {
      mark_object((Obj *)closure->upvalues[i]);
    }
    break;
  }
  case OBJ_FUNCTION: {
    ObjFunction *function = (ObjFunction *)object;
    mark_object((Obj *)function->name);
    mark_array(&function->chunk.constants);
    break;
  }
  case OBJ_INSTANCE: {
    ObjInstance *instance = (ObjInstance *)object;
    mark_object((Obj *)instance->klass);
    mark_table(&instance->fields);
    break;
  }
  case OBJ_UPVALUE:
    mark_value(((ObjUpvalue *)object)->closed);
    break;
  case OBJ_NATIVE:
  case OBJ_STRING:
    break;
  }
}

void trace_reference() {
  while (vm.gray_count > 0) {
    Obj *object = vm.gray_stack[--vm.gray_count];
    blacken_object(object);
  }
}

void collect_garbage() {
#ifndef DEBUG_LOG_GC
  cout << "-- gc begins" << endl;
  size_t before = vm.bytes_allocated;
#endif

  mark_roots();
  trace_reference();
  table_remove_white(&vm.strings);
  sweep();

  vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifndef DEBUG_LOG_GC
  cout << "-- gc end" << endl;
  cout << "collected " << before - vm.bytes_allocated << " bytes (from "
       << before << " to " << vm.bytes_allocated << ") next at " << vm.next_gc;
#endif
}

// Additional data structures and functionalities

// A set to store all objects that have been visited during the mark phase
unordered_set<Obj *> visited_objects;

// Function to recursively mark the value of an upvalue and any other object it
// references
void recursive_mark(Value value) {
  if (IS_OBJ(value)) {
    Obj *object = AS_OBJ(value);
    if (!object->is_marked) {
      object->is_marked = true;
      switch (object->type) {
      case OBJ_BOUND_METHOD: {
        ObjBoundMethod *bound = (ObjBoundMethod *)object;
        recursive_mark(bound->receiver); // Mark the receiver object
        mark_object((Obj *)bound->method);
        break;
      }
      case OBJ_CLASS: {
        ObjClass *klass = (ObjClass *)object;
        recursive_mark(OBJ_VAL(klass->name));
        mark_table(&klass->methods);
        break;
      }
      case OBJ_CLOSURE: {
        ObjClosure *closure = (ObjClosure *)object;
        recursive_mark(OBJ_VAL(closure->function));
        for (int i = 0; i < closure->upvalue_count; i++) {
          recursive_mark(OBJ_VAL(closure->upvalues[i]));
        }
        break;
      }
      case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        recursive_mark(OBJ_VAL(function->name));
        mark_array(&function->chunk.constants);
        break;
      }
      case OBJ_INSTANCE: {
        ObjInstance *instance = (ObjInstance *)object;
        recursive_mark(OBJ_VAL(instance->klass));
        mark_table(&instance->fields);
        break;
      }
      case OBJ_UPVALUE: {
        recursive_mark(
            ((ObjUpvalue *)object)->closed); // Recursive call to mark the value
        break;
      }
      default:
        break;
      }
    }
  }
}

// Function to perform a breadth-first search to mark all objects
void bfs_mark() {
  queue<Obj *> objects_queue;

  // Push roots onto the queue
  for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    if (IS_OBJ(*slot)) {
      objects_queue.push(AS_OBJ(*slot));
    }
  }

  for (int i = 0; i < vm.frame_count; i++) {
    objects_queue.push((Obj *)vm.frames[i].closure);
  }

  for (ObjUpvalue *upvalue = vm.open_upvalues; upvalue != nullptr;
       upvalue = upvalue->next) {
    objects_queue.push((Obj *)upvalue);
  }

  objects_queue.push((Obj *)vm.init_string);

  // Perform BFS
  while (!objects_queue.empty()) {
    Obj *object = objects_queue.front();
    objects_queue.pop();
    if (!object->is_marked) {
      object->is_marked = true;
      blacken_object(object);
      switch (object->type) {
      case OBJ_BOUND_METHOD: {
        ObjBoundMethod *bound = (ObjBoundMethod *)object;
        objects_queue.push((Obj *)bound->method);
        break;
      }
      case OBJ_CLASS: {
        ObjClass *klass = (ObjClass *)object;
        objects_queue.push((Obj *)klass->name);
        break;
      }
      case OBJ_CLOSURE: {
        ObjClosure *closure = (ObjClosure *)object;
        objects_queue.push((Obj *)closure->function);
        for (int i = 0; i < closure->upvalue_count; i++) {
          objects_queue.push((Obj *)closure->upvalues[i]);
        }
        break;
      }
      case OBJ_FUNCTION: {
        ObjFunction *function = (ObjFunction *)object;
        objects_queue.push((Obj *)function->name);
        break;
      }
      case OBJ_INSTANCE: {
        ObjInstance *instance = (ObjInstance *)object;
        objects_queue.push((Obj *)instance->klass);
        break;
      }
      case OBJ_UPVALUE: {
        recursive_mark(
            ((ObjUpvalue *)object)->closed); // Recursive call to mark the value
        break;
      }
      default:
        break;
      }
    }
  }
}

// Modify mark_phase() to use the new bfs_mark() function
void mark_phase() {
  mark_roots();
  visited_objects.clear();
  bfs_mark();
}

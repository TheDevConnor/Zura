#include "../parser/parser.h"
#include "gc.h"

using namespace std;

#ifndef DEBUG_LOG_GC
    #include <stdio.h>
    #include <iostream>
    #include "../debug/debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

void mark_object(Obj* object) {
    if(object == nullptr) return;
    if(object->is_marked) return;

    #ifndef DEBUG_LOG_GC
        cout<< "\n" << (void*)object << " mark ";
        print_value(OBJ_VAL(object));
        cout << endl;
    #endif

    object->is_marked = true;

    if(vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = (Obj**)realloc(vm.gray_stack, sizeof(Obj*) + vm.gray_capacity);

        if(vm.gray_stack == nullptr){
            ZuraExit(BAD_GRAY_STACK);
        }
    }
    vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value) {
    if (IS_OBJ(value)) mark_object(AS_OBJ(value));
}

void mark_array(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        mark_value(array->values[i]);
    }
}

void mark_roots() {
    for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_value(*slot);
    }

    for (int i = 0; i < vm.frame_count; i++) {
        mark_object((Obj*)vm.frames[i].closure);
    }

    for (ObjUpvalue* upvalue = vm.open_upvalues; upvalue != nullptr; upvalue = upvalue->next) {
        mark_object((Obj*)upvalue);
    }

    mark_table(&vm.globals);
    mark_compiler_roots();
}

void blacken_object(Obj* object) {
    #ifndef DEBUG_LOG_GC
        cout << (void*)object << " blacken ";
        print_value(OBJ_VAL(object));
        cout << endl;
    #endif

    switch (object->type) {
      case OBJ_CLASS: {
          ObjClass* klass = (ObjClass*)object;
          mark_object((Obj*)klass->name);
          mark_table(&klass->methods);
          break;
      }
      case OBJ_INSTANCE: {
          ObjInstance* instance = (ObjInstance*)object;
          mark_object((Obj*)instance->klass);
          mark_table(&instance->fields);
          break;
      }
      case OBJ_BOUND_METHOD: {
          ObjBoundMethod* bound = (ObjBoundMethod*)object;
          mark_value(bound->receiver);
          mark_object((Obj*)bound->method);
          break;
      }
      case OBJ_CLOSURE: {
          ObjClosure* closure = (ObjClosure*)object;
          mark_object((Obj*)closure->function);
          
          for(int i = 0; i < closure->upvalue_count; i++) {
              mark_object((Obj*)closure->upvalues[i]);
          }
          break;
      }
      case OBJ_FUNCTION: {
          ObjFunction* function = (ObjFunction*)object;
          mark_object((Obj*)function->name);
          mark_array(&function->chunk.constants);
          break;
      }
      case OBJ_UPVALUE:
          mark_value(((ObjUpvalue*)object)->closed);
          break;
      case OBJ_STRUCT: {
        ObjStruct* _struct = (ObjStruct*)object;
        mark_object((Obj*)_struct->name);
        mark_table(&_struct->fields);
        mark_table(&_struct->field_types);
        break;
      }
      case OBJ_NATIVE:
      case OBJ_STRING:
          ObjString* string = (ObjString*)object;
          mark_object((Obj*)string->chars);
          break;
    }
}

void trace_reference() {
    while(vm.gray_count > 0) {
        Obj* object = vm.gray_stack[--vm.gray_count];
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
        cout << "collected " << before - vm.bytes_allocated << " bytes (from " << before 
             << " to " << vm.bytes_allocated << ") next at " << vm.next_gc << endl;
    #endif
}

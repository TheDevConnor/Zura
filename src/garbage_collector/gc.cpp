#include "../parser/parser.h"
#include "gc.h"

#ifndef DEBUG_LOG_GC
#include <iostream>
#include "../debug/debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

// Forward declarations
void mark_object(Obj* object);
void mark_value(Value value);

void mark_object(Obj* object) {
    if (object == nullptr || object->is_marked) return;

    #ifndef DEBUG_LOG_GC
        std::cout << "\n" << (void*)object << " mark ";
        print_value(OBJ_VAL(object));
        std::cout << std::endl;
    #endif

    object->is_marked = true;

    if (vm.gray_capacity < vm.gray_count + 1) {
        vm.gray_capacity = GROW_CAPACITY(vm.gray_capacity);
        vm.gray_stack = (Obj**)realloc(vm.gray_stack, sizeof(Obj*) + vm.gray_capacity);

        if (vm.gray_stack == nullptr) {
            ZuraExit(BAD_GRAY_STACK);
        }
    }
    vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value) {
    if (IS_OBJ(value)) {
        mark_object(AS_OBJ(value));
    }
}

void mark_array(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        mark_value(array->values[i]);
    }
}

void mark_roots() {
    // Mark objects on the stack
    for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
        mark_value(*slot);
    }

    // Mark closure objects in frames
    for (int i = 0; i < vm.frame_count; i++) {
        mark_object((Obj*)vm.frames[i].closure);
    }

    // Mark open upvalues
    for (ObjUpvalue* upvalue = vm.open_upvalues; upvalue != nullptr; upvalue = upvalue->next) {
        mark_object((Obj*)upvalue);
    }

    mark_table(&vm.globals);
    mark_compiler_roots();
}

void blacken_object(Obj* object) {
#ifndef DEBUG_LOG_GC
    std::cout << (void*)object << " blacken ";
    print_value(OBJ_VAL(object));
    std::cout << std::endl;
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

            for (int i = 0; i < closure->upvalue_count; i++) {
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
        case OBJ_NATIVE:
        case OBJ_STRING:
            ObjString* string = (ObjString*)object;
            mark_object((Obj*)string->chars);
            break;
    }
}

void trace_reference() {
    while (vm.gray_count > 0) {
        Obj* object = vm.gray_stack[--vm.gray_count];
        blacken_object(object);
    }
}

void collect_garbage() {
#ifndef DEBUG_LOG_GC
    std::cout << "-- gc begins" << std::endl;
    size_t before = vm.bytes_allocated;
#endif

    mark_roots();
    trace_reference();
    table_remove_white(&vm.strings);
    sweep();

    vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifndef DEBUG_LOG_GC
    std::cout << "-- gc end" << std::endl;
    std::cout << "collected " << before - vm.bytes_allocated << " bytes (from " << before
              << " to " << vm.bytes_allocated << ") next at " << vm.next_gc << std::endl;
#endif
}

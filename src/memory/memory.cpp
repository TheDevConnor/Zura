#include <stdlib.h>
#include <iostream>

#include "../garbage_collector/gc.h"
#include "../parser/object.h"
#include "../parser/vm.h"
#include "memory.h"

using namespace std;

void* reallocate(void* pointer, size_t old_size, size_t new_size) {
    vm.bytes_allocated += new_size - old_size;
    if (new_size > old_size) {
        #ifndef DEBUG_STRESS_GC
            collect_garbage();
        #endif
    }
    if (new_size == 0) {
        free(pointer);
        return nullptr;
    }

    void* new_pointer = realloc(pointer, new_size);
    if (new_pointer == nullptr) exit(1);
    return new_pointer;
}

static void free_obj(Obj* object) {

    #ifndef DEBUG_LOG_GC
        cout << (void*)object << " free type " << object->type << endl;
    #endif

    switch (object->type) {
        case OBJ_CLASS: {
            FREE(ObjClass, object);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*) object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalue_count);
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*) object;
            free_chunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_INSTANCE: {
            ObjInstance* instance = (ObjInstance*)object;
            free_table(&instance->fields);
            FREE(ObjInstance, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
            break;
        }
        case OBJ_STRING: {
            ObjString* string = reinterpret_cast<ObjString*>(object);
            reallocate(string, sizeof(ObjString) + string->length + 1, 0);
            break;
        }
        case OBJ_UPVALUE: {
            ObjClosure* closure = (ObjClosure*) object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalue_count);
            FREE(ObjUpvalue, object);
            break;
        }
    }
}

void free_objects() {
    Obj* object = vm.objects;
    while (object != nullptr) {
        Obj* next = object->next;
        free_obj(object);
        object = next;
    }

    free(vm.gray_stack);
}

void sweep() {
    Obj* prev = nullptr;
    Obj* obj  = vm.objects;
    while (obj != nullptr) {
        if (obj->is_marked) {
            obj->is_marked = false;
            prev = obj;
            obj = obj->next;
        } else {
            Obj* unreached = obj;
            obj = obj->next;

            if (prev != nullptr) prev->next = obj;
            else vm.objects = obj;

            free_obj(unreached);
        }
    } 
}

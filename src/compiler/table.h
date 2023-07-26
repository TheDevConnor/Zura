#pragma once

#include "../common.h"
#include "value.h"

struct Entry {
    ObjString* key;
    Value value;
};

struct Table {
    int count;
    int capacity;
    Entry* entries;
};

void init_table(Table* table);
void free_table(Table* table);

Table* new_table();

bool table_get(Table* table, ObjString* key, Value* value);
bool table_set(Table* table, ObjString* key, Value value);
bool table_delete(Table* table, ObjString* key);

void table_add_all(Table* from, Table* to);

ObjString* table_find_string(Table* table, const char* chars, int length, uint32_t hash);

void table_remove_white(Table* table);
void mark_table(Table* table);

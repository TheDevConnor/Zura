#ifndef AZURE_TABLE_H
#define AZURE_TABLE_H

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

bool table_get(Table* table, ObjString* key, Value* value);
bool table_set(Table* table, ObjString* key, Value value);
bool table_delete(Table* table, ObjString* key);

void table_add_all(Table* from, Table* to);

ObjString* table_find_string(Table* table, const char* chars, int length, uint32_t hash);

#endif // AZURE_TABLE_H
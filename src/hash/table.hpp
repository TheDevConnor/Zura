#pragma once

#include "../types/type.hpp"
#include "../common.hpp"

class HashTable {
public:
    struct Entry {
        ObjString* key;
        Value value;
    };

    struct Table {
        int count; 
        int capacity;
        Entry* entries;
    };

    static constexpr double TABLE_MAX_LOAD = 0.75;

    static void initTable(Table* table);
    static void freeTable(Table* table);
    static void tableAddAll(Table* from, Table* to);
    static void adjustCapacity(Table* table, int capacity);

    static Entry* findEntry(Entry* entries, int capacity, ObjString* key);

    static ObjString* findString(Table* table, const char* chars, int length, uint32_t hash);

    static bool tableGet(Table* table, ObjString* key, Value* value);
    static bool tableSet(Table* table, ObjString* key, Value value);
    static bool tableDelete(Table* table, ObjString* key);

private:
};
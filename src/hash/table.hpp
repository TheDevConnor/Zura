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
        size_t capacity;
        Entry* entries;
    };

    static constexpr double TABLE_MAX_LOAD = 0.75;

    static void initTable(Table* table);
    static void freeTable(Table* table);
    static void tableAddAll(Table* from, Table* to);
    static void adjustCapacity(Table* table, size_t capacity);

    static Entry* findEntry(Entry* entries, size_t capacity, ObjString* key);

    static ObjString* findString(Table* table, const char* chars, size_t length, size_t hash);

    static bool tableGet(Table* table, ObjString* key, Value* value);
    static bool tableSet(Table* table, ObjString* key, Value value);
    static bool tableDelete(Table* table, ObjString* key);

private:
};

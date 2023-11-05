#include <cstring>

#include "../memory/memory.hpp"
#include "../object/object.hpp"
#include "table.hpp"

void HashTable::initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = nullptr;
}

void HashTable::freeTable(Table* table) {
    Memory::FREE_ARRAY<Entry>(table->entries, table->capacity);
    initTable(table);
}

HashTable::Entry* HashTable::findEntry(Entry* entries, int capacity, ObjString* key) {
    uint32_t index = key->hash % capacity;
    Entry* tombstone = nullptr;

    for (;;) {
        Entry* entry = &entries[index];
        if (entry->key == NULL) {
            if (IS_NIL(entry->value)) return tombstone != NULL ? tombstone : entry;
            if (tombstone == NULL) tombstone = entry;
        } else if (entry->key == key) return entry;
        index = (index + 1) % capacity;
    }
}

void HashTable::tableAddAll(Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if (entry->key != nullptr) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

ObjString* HashTable::findString(Table* table, const char* chars, int length, uint32_t hash) {
    if (table->count == 0) return nullptr;

    uint32_t index = hash % table->capacity;

    for (;;) {
        HashTable::Entry* entry = &table->entries[index];

        if (entry->key == nullptr) {
            // Stop if we need an empty, non-tombstone entry.
            if (IS_NIL(entry->value)) return nullptr;
        } else if (entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0) {
            // We found it.
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }

}

void HashTable::adjustCapacity(Table* table, int capacity) {
    Entry* entries = Memory::ALLOCATE<Entry>(capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = nullptr;
        entries[i].value = NIL_VAL();
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key == nullptr) continue;

        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    Memory::FREE_ARRAY<Entry>(table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

bool HashTable::tableGet(Table* table, ObjString* key, Value* value) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr) return false;

    *value = entry->value;
    return true;
}

bool HashTable::tableSet(Table* table, ObjString* key, Value value) {
    if (table->count + 1 > table->capacity * HashTable::TABLE_MAX_LOAD) {
        int capacity = Memory::GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry* entry = findEntry(table->entries, table->capacity, key);
    bool isNewKey = entry->key == nullptr;
    if (isNewKey && IS_NIL(entry->value)) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool HashTable::tableDelete(Table* table, ObjString* key) {
    if (table->count == 0) return false;

    // Find the entry.
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr) return false;

    // Place a tombstone in the entry.
    entry->key = nullptr;
    entry->value = BOOL_VAL(true);
    return true;
}
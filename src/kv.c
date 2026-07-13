#include <kv.h>
#include <string.h>
#include <stdlib.h>

#define TOMBSTONE 0x1

size_t hash(char *val, int capacity) {
    size_t hash = 0x13371337deadbeef;

    while(*val) {
        hash ^= *val;
        hash = hash << 8;
        hash += *val;

        val++;
    }

    return hash % capacity;
}

// fn kv_put
// params:
//  - db: a points to the db
//  - key: a pointer to the key value
//  - value: a pointer to the value itself
// returns: the index of the key, otherwise on
// error, returns -1, on not found returns -2
int kv_put(kv_t *db, char *key, char *value) {
    if (!db || !key || !value) return -1;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity -1; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        // found the slot, occupied, and the key matches
        if (entry->key
            && entry->key != (void *)TOMBSTONE
            && !strcmp(entry->key, key)
        ) {
            char *newval = strdup(value);
            if (!newval) return -1;
            entry->value = newval;

            return real_idx;
        }

        // found the slot, and its empty or tombstone
        // When probing for an empty slot, you can reuse a tombstone slot just
        // like an empty one. However, you still need to keep probing past it to
        // check whether the key already exists further down the chain. A clean
        // way to handle this is to record the first tombstone you encounter, and
        // then if you finish probing without finding a match, write the new entry
        // into that saved tombstone slot.
        if (!entry->key || entry->key == (void *)TOMBSTONE) {
            char *newval = strdup(value);
            char *newkey = strdup(key);

            if (!newval || !newkey) {
                free(newkey);
                free(newval);

                return -1;
            }

            entry->value = newval;
            entry->key = newkey;

            db->count++;

            return real_idx;
        }
    }

    // the db is occupied
    return -2;
}

kv_t *kv_init(size_t capacity) {
    if (capacity == 0) return NULL;

    kv_t *table = malloc(sizeof(kv_t));
    if (table == NULL) return NULL;

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(sizeof(kv_entry_t), capacity);
    if (table->entries == NULL) {
        free(table);
        table = NULL;

        return NULL;
    }

    return table;
}

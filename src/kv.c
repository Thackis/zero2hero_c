#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kv.h"

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

// fn kv_delete
// params:
//  - db: a points to the db
//  - key: a pointer to the key value
// returns: the index of the deletion
// 			-1 if not found
int kv_delete(kv_t *db, char *key) {
	if (!db || !key) return -1;

	size_t idx = hash(key, db->capacity);

	for (size_t i = 0; i < db->capacity -1; i++) {
		size_t real_idx = (idx + i) % db->capacity;

		kv_entry_t *entry = &db->entries[real_idx];

		if (entry->key == NULL) return -1;

		if (entry->key != (void *)TOMBSTONE
			&& !strcmp(entry->key, key)
		) {
			free(entry->key);
			free(entry->value);
			db->count--;

			entry->key = (void *)TOMBSTONE;
			entry->value = NULL;

			return 0;
			// return real_idx;
		}
	}

	return -1;
}

// fn kv_get
// params:
//  - db: a points to the db
//  - key: a pointer to the key value
// returns: the pointer to the key, NULL if not found
char *kv_get(kv_t *db, char *key) {
	if (!db || !key) return NULL;
	// if (!db || !key) return -1;

	size_t idx = hash(key, db->capacity);

	for (size_t i = 0; i < db->capacity -1; i++) {
		size_t real_idx = (idx + i) % db->capacity;

		kv_entry_t *entry = &db->entries[real_idx];

		// is no key, therefore return nothing
		if (entry->key == NULL) return NULL;

		// check if the keys match (already know it exists)
		if (entry->key != (void *)TOMBSTONE
			&& !strcmp(entry->key, key)
		) {
			return entry->value;
		}
	}

	return NULL;
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
	size_t tombstone_slot = 0;

	for (size_t i = 0; i < db->capacity -1; i++) {
		size_t real_idx = (idx + i) % db->capacity;

		kv_entry_t *entry = &db->entries[real_idx];

		// Found a tombstone!
		// Saving location if first instance, otherwise continue.
		if (entry->key == (void *)TOMBSTONE) {
			if (!tombstone_slot) tombstone_slot = real_idx;

			continue;
		}

		// found the slot, and its empty
		if (!entry->key) {
			char *newkey = strdup(key);
			char *newval = strdup(value);

			if (!newkey || !newval) {
				free(newkey);
				free(newval);

				return -1;
			}

			// Change entry to first tombstone location if tombstone was found.
			if (tombstone_slot) entry = &db->entries[tombstone_slot];

			entry->key = newkey;
			entry->value = newval;

			db->count++;

			return 0;
			// return real_idx;
		}

		// Check if key matches (already know slot is occupied)
		if (!strcmp(entry->key, key)) {
			char *newval = strdup(value);
			if (!newval) return -1;

			free(entry->value);

			entry->value = newval;

			return 0;
			// return real_idx;
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

// fn kv_free
// params:
//  - db: a points to the db
// returns: 0 on success, -1 on failure
// [NOTE]	This really should null out the pointer, because, why not? Just
// 			because free does it that way, doesn't mean we need to make a user's
//			life harder.
void kv_free(kv_t *db) {
// int kv_free(kv_t *db) {
	if (!db) return;
	// if (!db) return -1;

	for (size_t i = 0; i < db->capacity - 1; i++) {
		kv_entry_t *e = &db->entries[i];

		if (e->key && e->key != (void *)TOMBSTONE) {
			free(e->key);
			free(e->value);

			e->key = NULL;
			e->value = NULL;

			db->count--;
		}
	}

	free(db->entries);
	free(db);

	return;
	// return 0;
}

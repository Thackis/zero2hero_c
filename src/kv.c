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

// fn kv_delete
// params:
//  - db: a points to the db
//  - key: a pointer to the key value
// returns: the index of the deletion
// 			-1 if not found
int kv_delete(kv_t *db, char *key) {
	if (!db || !key) return -1;

	size_t idx = hash(key, db->capacity);

	for (int i = 0; i < db->capacity -1; i++) {
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

	for (int i = 0; i < db->capacity -1; i++) {
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
	size_t first_tombstone = 0;

	for (int i = 0; i < db->capacity -1; i++) {
		size_t real_idx = (idx + i) % db->capacity;

		kv_entry_t *entry = &db->entries[real_idx];

		// found the slot, and its empty
		if (!entry->key) {
			char *newval = strdup(value);
			char *newkey = strdup(key);

			if (!newval || !newkey) {
				free(newkey);
				free(newval);

				return -1;
			}

			// Check if found a tomestone, and if so, enter there instead
			if (!first_tombstone) {
				entry->value = newval;
				entry->key = newkey;
			} else {
				kv_entry_t *tomestone_entry = &db->entries[first_tombstone];

				tomestone_entry->value = newval;
				tomestone_entry->key = newkey;
			}

			db->count++;

			return 0;
			// return real_idx;
		}

		// Check if key matches (already know slot is occupied)
		if (!strcmp(entry->key, key)) {
			char *newval = strdup(value);
			if (!newval) return -1;
			entry->value = newval;

			return 0;
			// return real_idx;
		}

		// Found a tombstone!
		// Saving location if first instance, otherwise continue.
		if (entry->key == (void *)TOMBSTONE
			&& !first_tombstone
		) {
			first_tombstone = real_idx;
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

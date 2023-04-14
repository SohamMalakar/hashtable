#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _Entry Entry;
typedef struct _Hashtable Hashtable;

typedef uint64_t hashfunc(const void *, size_t);

Entry *new_entry(const void *key, size_t size, void *value, size_t vsize);
void free_entry(Entry *entry);

Hashtable *init_hashtable(uint64_t capacity, hashfunc *hf);
void free_hashtable(Hashtable *ht);

bool hashtable_insert(Entry *object, Hashtable *ht);
Entry *hashtable_delete(const void *key, size_t size, Hashtable *ht);
Entry *hashtable_lookup(const void *key, size_t size, Hashtable *ht);

#endif

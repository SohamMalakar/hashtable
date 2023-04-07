#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    char *key;
    int value;
} Entry;

struct _Hashtable
{
    uint64_t count;
    uint64_t capacity;
    Entry **entries;
};

typedef struct _Hashtable Hashtable;

Entry *new_entry(const char *key, int value);
void free_entry(Entry *entry);

uint64_t hash(const char *key, uint64_t capacity);

void init_hashtable(Hashtable *ht, uint64_t capacity);
void free_hashtable(Hashtable *ht);

bool hashtable_insert(Entry *object, Hashtable *ht);
Entry *hashtable_delete(const char *key, Hashtable *ht);
Entry *hashtable_lookup(const char *key, Hashtable *ht);

#endif

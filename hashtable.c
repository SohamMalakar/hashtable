#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define GROWTH_RATE 2
#define TABLE_MAX_LOAD 0.75

#define DELETED ((void *)-1)

Entry *new_entry(const char *key, int value)
{
    Entry *_new = malloc(sizeof(Entry));

    if (_new == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    _new->key = strdup(key);
    _new->value = value;
    return _new;
}

void free_entry(Entry *entry)
{
    if (entry == NULL)
        return;
    free(entry->key);
    free(entry);
}

// TODO: optimise the hash function
uint64_t hash(const char *key, uint64_t capacity)
{
    uint64_t length = strlen(key);
    uint64_t hash_value = 0;

    for (uint64_t i = 0; i < length; i++)
    {
        hash_value += key[i];
        hash_value = (hash_value * key[i]) % capacity;
    }

    return hash_value;
}

void init_hashtable(Hashtable *ht, uint64_t capacity)
{
    ht->count = 0;
    ht->capacity = capacity;
    ht->entries = calloc(sizeof(void *), capacity);

    if (ht->entries == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }
}

void free_hashtable(Hashtable *ht)
{
    Entry *tmp;

    for (uint64_t i = 0; i < ht->capacity; i++)
    {
        tmp = ht->entries[i];

        if (tmp != NULL && tmp != DELETED)
            free_entry(tmp);
    }

    free(ht->entries);
    ht->capacity = 0;
    ht->count = 0;
}

bool hashtable_insert(Entry *object, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(object->key, capacity);
    uint64_t tmp;

    // grow the hashtable
    if (ht->count > capacity * TABLE_MAX_LOAD)
    {
        Hashtable new_ht;
        uint64_t new_capacity = capacity * GROWTH_RATE;
        init_hashtable(&new_ht, new_capacity);

        for (uint64_t i = 0; i < capacity; i++)
            if (ht->entries[i] != NULL && ht->entries[i] != DELETED)
                hashtable_insert(ht->entries[i], &new_ht);

        free(ht->entries);
        ht->capacity = new_capacity;
        ht->count = new_ht.count;
        ht->entries = new_ht.entries;
    }

    // linear probing
    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (i + index) % capacity;

        if (ht->entries[tmp] == NULL || ht->entries[tmp] == DELETED)
        {
            if (ht->entries[tmp] == NULL)
                ht->count++;

            ht->entries[tmp] = object;
            return true;
        }
        else if (strcmp(ht->entries[tmp]->key, object->key) == 0)
        {
            // already same key exist
            // break;
            free_entry(ht->entries[tmp]);
            ht->entries[tmp] = object;
            return true;
        }
    }

    free_entry(object);
    return false;
}

Entry *hashtable_delete(const char *key, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(key, capacity);
    uint64_t tmp;

    // TODO: shrink the hashtable

    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (i + index) % capacity;

        if (ht->entries[tmp] == NULL)
        {
            return NULL;
        }
        else if (ht->entries[tmp] != DELETED && strcmp(ht->entries[tmp]->key, key) == 0)
        {
            void *deleted = ht->entries[tmp];
            ht->entries[tmp] = DELETED;
            return deleted;
        }
    }

    return NULL;
}

Entry *hashtable_lookup(const char *key, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(key, capacity);
    uint64_t tmp;

    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (i + index) % capacity;

        if (ht->entries[tmp] == NULL)
            return NULL;
        else if (ht->entries[tmp] != DELETED && strcmp(ht->entries[tmp]->key, key) == 0)
            return ht->entries[tmp];
    }

    return NULL;
}

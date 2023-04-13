#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

#define TABLE_MAX_LOAD 0.75

#define GROWTH_FACTOR 2
#define SHRINK_FACTOR 0.5

#define DELETED ((void *)-1)

struct _Entry
{
    void *key;
    size_t size;
    void *value;
    size_t vsize;
};

struct _Hashtable
{
    uint64_t count;
    uint64_t capacity;
    uint64_t deleted;
    Entry **entries;
};

Entry *new_entry(const void *key, size_t size, void *value, size_t vsize)
{
    Entry *_new = malloc(sizeof(Entry));

    if (_new == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    _new->key = malloc(size);

    if (_new->key == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    memcpy(_new->key, key, size);
    _new->size = size;

    _new->value = malloc(vsize);

    if (_new->value == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    memcpy(_new->value, value, vsize);
    return _new;
}

void free_entry(Entry *entry)
{
    if (entry == NULL)
        return;
    free(entry->key);
    free(entry->value);
    free(entry);
}

// FNV-1a hash function
// Reference: http://www.isthe.com/chongo/tech/comp/fnv/
#define FNV_PRIME 1099511628211llu
#define FNV_OFFSET 14695981039346656037llu

static uint64_t hash(const void *key, size_t size)
{
    uint64_t hash_value = FNV_OFFSET;

    for (int i = 0; i < size; i++)
    {
        hash_value ^= *((uint8_t *)key + i);
        hash_value *= FNV_PRIME;
    }

    return hash_value;
}

Hashtable *init_hashtable(uint64_t capacity)
{
    Hashtable *ht = malloc(sizeof(Hashtable));

    if (ht == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    ht->count = 0;
    ht->deleted = 0;
    ht->capacity = capacity;

    ht->entries = calloc(sizeof(void *), capacity);

    if (ht->entries == NULL)
    {
        fprintf(stderr, "out of memory!\n");
        exit(EXIT_FAILURE);
    }

    return ht;
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
    ht->deleted = 0;

    free(ht);
}

static void hashtable_resize(Hashtable *ht, uint64_t *capacity, float multiplier)
{
    uint64_t new_capacity = *capacity * multiplier;
    Hashtable *new_ht = init_hashtable(new_capacity);

    for (uint64_t i = 0; i < *capacity; i++)
        if (ht->entries[i] != NULL && ht->entries[i] != DELETED)
            hashtable_insert(ht->entries[i], new_ht);

    free(ht->entries);

    *capacity = ht->capacity = new_capacity;

    ht->count = new_ht->count;
    ht->entries = new_ht->entries;
    ht->deleted = 0;

    free(new_ht);
}

uint64_t q_probing(uint64_t x)
{
    // quadratic probing function
    // P(x) = (x^2 + x) / 2
    return (x * x + x) >> 1;
}

bool hashtable_insert(Entry *object, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(object->key, object->size);
    uint64_t tmp;

    // grow the hashtable
    if (ht->count > capacity * TABLE_MAX_LOAD)
        hashtable_resize(ht, &capacity, GROWTH_FACTOR);

    // quadratic probing
    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (index + q_probing(i)) % capacity;

        if (ht->entries[tmp] == NULL || ht->entries[tmp] == DELETED)
        {
            if (ht->entries[tmp] == NULL)
                ht->count++;
            else
                ht->deleted--;

            ht->entries[tmp] = object;
            return true;
        }
        else if (ht->entries[tmp]->size == object->size &&
                 memcmp(ht->entries[tmp]->key, object->key, object->size) == 0)
        {
            free_entry(ht->entries[tmp]);
            ht->entries[tmp] = object;
            return true;
        }
    }

    free_entry(object);
    return false;
}

Entry *hashtable_delete(const void *key, size_t size, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(key, size);
    uint64_t tmp;

    // shrink the hashtable
    if (ht->deleted * 2 > ht->capacity)
        hashtable_resize(ht, &capacity, SHRINK_FACTOR);

    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (index + q_probing(i)) % capacity;

        if (ht->entries[tmp] == NULL)
        {
            return NULL;
        }
        else if (ht->entries[tmp] != DELETED && ht->entries[tmp]->size == size &&
                 memcmp(ht->entries[tmp]->key, key, size) == 0)
        {
            void *deleted = ht->entries[tmp];
            ht->entries[tmp] = DELETED;
            ht->deleted++;
            return deleted;
        }
    }

    return NULL;
}

Entry *hashtable_lookup(const void *key, size_t size, Hashtable *ht)
{
    uint64_t capacity = ht->capacity;
    uint64_t index = hash(key, size);
    uint64_t tmp;

    for (uint64_t i = 0; i < capacity; i++)
    {
        tmp = (index + q_probing(i)) % capacity;

        if (ht->entries[tmp] == NULL)
            return NULL;
        else if (ht->entries[tmp] != DELETED && ht->entries[tmp]->size == size &&
                 memcmp(ht->entries[tmp]->key, key, size) == 0)
            return ht->entries[tmp];
    }

    return NULL;
}

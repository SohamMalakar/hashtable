#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hashtable.h"

#define INIT_HASHTABLE_SIZE 8

int main(void)
{
    char str[256];
    uint64_t count = 0;

    Hashtable *ht = init_hashtable(INIT_HASHTABLE_SIZE);

    FILE *fp = fopen("words.txt", "r");

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);

    srand(ts.tv_nsec);

    int _rand;

    while (fscanf(fp, "%s", str) == 1)
    {
        _rand = rand();
        count += hashtable_insert(new_entry(str, strlen(str), &_rand, sizeof(_rand)), ht);
    }

    printf("Number of successful insertion: %lu\n", count);

    rewind(fp);

    while (fscanf(fp, "%s", str) == 1)
        assert(hashtable_lookup(str, strlen(str), ht) != NULL);

    rewind(fp);

    while (fscanf(fp, "%s", str) == 1)
        free_entry(hashtable_delete(str, strlen(str), ht));

    fclose(fp);
    free_hashtable(ht);

    return 0;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hashtable.h"

#define INIT_HASHTABLE_SIZE (1 << 20)

int main(void)
{
    char str[256];
    uint64_t count = 0;

    Hashtable ht;
    init_hashtable(&ht, INIT_HASHTABLE_SIZE);

    FILE *fp = fopen("words.txt", "r");

    srand(time(NULL));

    while (fscanf(fp, "%s", str) == 1)
        count += hashtable_insert(new_entry(str, rand()), &ht);

    printf("Number of successful insertion: %lu\n", count);

    rewind(fp);

    while (fscanf(fp, "%s", str) == 1)
        assert(hashtable_lookup(str, &ht) != NULL);

    rewind(fp);

    while (fscanf(fp, "%s", str) == 1)
        free_entry(hashtable_delete(str, &ht));

    fclose(fp);
    free_hashtable(&ht);

    return 0;
}

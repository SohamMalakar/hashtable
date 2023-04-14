#include "murmur3_64.h"

// MurmurHash3 constants
const uint64_t c1 = 0x87c37b91114253d5;
const uint64_t c2 = 0x4cf5ad432745937f;
const uint32_t r1 = 31;
const uint32_t r2 = 27;
const uint32_t r3 = 33;
const uint32_t m = 0x5bd1e995;
const uint32_t n = 0x52dce729;

static uint64_t murmur3_64(const char *key, size_t len, uint64_t seed)
{
    // Initialize hash to seed value
    uint64_t h = seed ^ (len * c1);

    // If the key is not aligned on an 8-byte boundary, process the remaining bytes first
    if (len > 0)
    {
        const uint8_t *data = (const uint8_t *)key;
        while (len >= 8)
        {
            uint64_t k = *(uint64_t *)data;
            k *= c1;
            k = (k << r1) | (k >> (64 - r1));
            k *= c2;
            h ^= k;
            h = (h << r2) | (h >> (64 - r2));
            h = h * m + n;
            data += 8;
            len -= 8;
        }

        // Process the remaining bytes (if any)
        uint64_t k = 0;
        switch (len)
        {
        case 7:
            k ^= (uint64_t)data[6] << 48;
        case 6:
            k ^= (uint64_t)data[5] << 40;
        case 5:
            k ^= (uint64_t)data[4] << 32;
        case 4:
            k ^= (uint64_t)data[3] << 24;
        case 3:
            k ^= (uint64_t)data[2] << 16;
        case 2:
            k ^= (uint64_t)data[1] << 8;
        case 1:
            k ^= (uint64_t)data[0];
            k *= c1;
            k = (k << r1) | (k >> (64 - r1));
            k *= c2;
            h ^= k;
        }
    }

    // Finalize hash
    h ^= (h >> r3);
    h *= 0x9e3779b97f4a7c13;
    h ^= (h >> r2);
    h *= 0x9e3779b97f4a7c13;
    h ^= (h >> r3);

    return h;
}

uint64_t hash(const void *key, size_t size)
{
    return murmur3_64(key, size, 42);
}

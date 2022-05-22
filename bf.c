#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "salts.h"
#include "bv.h"
#include "speck.h"

typedef struct BloomFilter BloomFilter;

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    struct BloomFilter *bf = (struct BloomFilter *) malloc(sizeof(struct BloomFilter));
    if (bf == NULL) {
        return NULL;
    }
    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;
    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;
    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;
    bf->filter = bv_create(size);
    if (!bf->filter) {
        free(bf);
        bf = NULL;
    }
    return bf;
}

void bf_delete(BloomFilter **bf) {
    if (*bf && (*bf)->filter) {
        bv_delete(&((*bf)->filter));
        free(*bf);
        *bf = NULL;
    }
    return;
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *oldspeak) {
    uint32_t primary_hashKey = hash(bf->primary, oldspeak);
    uint32_t secondary_hashKey = hash(bf->secondary, oldspeak);
    uint32_t tertiary_hashKey = hash(bf->tertiary, oldspeak);

    if (bv_set_bit(bf->filter, primary_hashKey % bf_size(bf)) == false) {
        fprintf(stderr, "Error setting primary bit");
        return;
    }
    if (bv_set_bit(bf->filter, secondary_hashKey % bf_size(bf)) == false) {
        fprintf(stderr, "Error setting secondary bit");
        return;
    }
    if (bv_set_bit(bf->filter, tertiary_hashKey % bf_size(bf)) == false) {
        fprintf(stderr, "Error setting tertiary bit");
        return;
    }
}

bool bf_probe(BloomFilter *bf, char *oldspeak) {
    uint32_t primary_hashKey = hash(bf->primary, oldspeak);
    uint32_t secondary_hashKey = hash(bf->secondary, oldspeak);
    uint32_t tertiary_hashKey = hash(bf->tertiary, oldspeak);

    if ((bv_get_bit(bf->filter, primary_hashKey % bf_size(bf)) == true)
        && (bv_get_bit(bf->filter, secondary_hashKey % bf_size(bf)) == true)
        && (bv_get_bit(bf->filter, tertiary_hashKey % bf_size(bf)) == true)) {
        return true;
    }
    return false;
}

uint32_t bf_count(BloomFilter *bf) {
    uint32_t set_bit_count = 0;
    uint32_t sz = bf_size(bf);
    for (uint32_t i = 0; i < sz; i++) {
        if (bv_get_bit(bf->filter, i)) {
            set_bit_count++;
        }
    }
    return set_bit_count;
}

void bf_print(BloomFilter *bf) {
    printf("Length: %u\n", bf_size(bf));
    bv_print(bf->filter);
}

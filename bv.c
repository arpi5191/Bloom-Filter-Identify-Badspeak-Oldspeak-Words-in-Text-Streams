#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "salts.h"
#include "bv.h"

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

struct BitVector *bv_create(uint32_t length) {
    uint32_t n = (length) / 8;
    if ((length % 8) > 0) {
        n = n + 1;
    }
    struct BitVector *bv = (struct BitVector *) malloc(sizeof(struct BitVector));
    if (bv == NULL) {
        return NULL;
    }
    bv->vector = (uint8_t *) calloc(n, sizeof(uint8_t));
    if (!bv->vector) {
        free(bv);
        bv = NULL;
    }
    bv->length = length;
    return bv;
}

void bv_print(BitVector *bv) {
    printf("Length of Bit Vector: %u\n", bv->length);
    uint32_t bv_length = (bv->length) / 8;
    if ((bv->length % 8) > 0) {
        bv_length = bv_length + 1;
    }
    for (uint32_t i = 0; i < bv_length; i++) {
        printf("Byte[%u]: 0x%x ", i, bv->vector[i]);
    }
    printf("\n");
}

void bv_delete(BitVector **bv) {
    if (*bv && (*bv)->vector) {
        free((*bv)->vector);
        free(*bv);
        *bv = NULL;
    }
    return;
}

uint32_t bv_length(BitVector *bv) {
    return bv->length;
}

bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (i > bv->length) {
        return false;
    }
    uint32_t index = i / 8;
    uint8_t sub_index = (uint8_t) i % 8;
    uint8_t shifted_bit = 1 << sub_index;
    bv->vector[index] = bv->vector[index] | shifted_bit;
    return true;
}

bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i > bv->length) {
        return false;
    }
    uint32_t index = i / 8;
    uint8_t sub_index = (uint8_t) i % 8;
    uint8_t shifted_bit = ~(1 << sub_index);
    bv->vector[index] = bv->vector[index] & shifted_bit;
    return true;
}

bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (i > bv->length) {
        return false;
    }
    uint32_t index = i / 8;
    uint8_t sub_index = (uint8_t) i % 8;
    uint8_t shifted_bit = 1 << sub_index;
    uint8_t bit = bv->vector[index] & shifted_bit;
    if (bit > 0) {
        return true;
    } else {
        return false;
    }
}

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "salts.h"
#include "bv.h"
#include "bf.h"
#include "node.h"
#include "bst.h"
#include "ht.h"
#include "speck.h"

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **trees;
};

uint64_t lookups = 0;

HashTable *ht_create(uint32_t size) {
    struct HashTable *ht = (struct HashTable *) malloc(sizeof(struct HashTable));
    if (ht == NULL) {
        return NULL;
    }
    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;
    ht->size = size;
    ht->trees = (Node **) calloc(size, sizeof(Node *));

    if (!(ht->trees)) {
        free(ht);
        ht = NULL;
    }
    return ht;
}

void ht_print(HashTable *ht) {
    uint32_t i = 0;
    if (ht != NULL) {
        uint32_t sz = ht_size(ht);
        printf("Hash Table Size: %u\n", sz);
        if (ht->trees != NULL) {
            while (i < sz) {
                printf("Tree %u:\n", i);
                bst_print(ht->trees[i]);
                i++;
            }
        }
    }
}

void ht_delete(HashTable **ht) {
    if (*ht && (*ht)->trees) {
        uint32_t sz = ht_size(*ht);
        for (uint32_t i = 0; i < sz; i++) {
            Node *p = (*ht)->trees[i];
            if (p != NULL) {
                bst_delete(&p);
            }
        }

        free((*ht)->trees);
        free(*ht);
        *ht = NULL;
    }
    return;
}

uint32_t ht_size(HashTable *ht) {
    return (ht->size);
}

Node *ht_lookup(HashTable *ht, char *oldspeak) {
    lookups++;
    uint32_t hashKey = hash(ht->salt, oldspeak);
    hashKey = hashKey % ht_size(ht);
    Node *p = bst_find(ht->trees[hashKey], oldspeak);
    return p;
}

void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
    lookups++;
    uint32_t hashKey = hash(ht->salt, oldspeak);
    hashKey = hashKey % ht_size(ht);
    Node *p = ht->trees[hashKey];
    Node *node = bst_insert(p, oldspeak, newspeak);
    if (p == NULL) {
        ht->trees[hashKey] = node;
        lookups++;
    }
}

uint32_t ht_count(HashTable *ht) {
    uint32_t binary_tree_count = 0;
    uint32_t sz = ht_size(ht);

    for (uint32_t i = 0; i < sz; i++) {
        if (ht->trees[i] != NULL) {
            binary_tree_count++;
        }
    }
    return binary_tree_count;
}

double ht_avg_bst_size(HashTable *ht) {
    uint32_t binary_tree_size = 0;
    uint32_t sz = ht_size(ht);
    uint32_t binary_tree_count = 0;
    double avg_sz = 0;
    for (uint32_t i = 0; i < sz; i++) {
        if (ht->trees[i] != NULL) {
            binary_tree_size += bst_size(ht->trees[i]);
            binary_tree_count++;
        }
    }
    avg_sz = (double) binary_tree_size / (double) binary_tree_count;
    return avg_sz;
}

double ht_avg_bst_height(HashTable *ht) {
    uint32_t binary_tree_height = 0;
    uint32_t sz = ht_size(ht);
    uint32_t binary_tree_count = 0;
    double avg_ht = 0;
    for (uint32_t i = 0; i < sz; i++) {
        if (ht->trees[i] != NULL) {
            binary_tree_height += bst_height(ht->trees[i]);
            binary_tree_count++;
        }
    }
    avg_ht = (double) binary_tree_height / (double) binary_tree_count;
    return avg_ht;
}

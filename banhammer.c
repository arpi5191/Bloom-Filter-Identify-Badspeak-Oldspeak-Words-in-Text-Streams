#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bf.h"
#include "bst.h"
#include "bv.h"
#include "ht.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
#include "salts.h"
#include "speck.h"
//#include <time.h>
#include <sys/stat.h>
#include "parser.h"
#include <regex.h>

#define WORD    "[a-zA-Z0-9’_-]+"
#define OPTIONS "hst:f:"

void usage(void);

// global variables
extern uint64_t branches;
extern uint64_t lookups;

bool stat_print = false;

int main(int argc, char **argv) {
    int opt = 0;
    char choices[2] = { '0', '0' };
    int num_choices = 0;
    bool help = false;
    uint32_t t_size = 65536;
    uint32_t f_size = 1048576;
    BloomFilter *bf_ret = NULL;
    HashTable *ht_ret = NULL;
    FILE *fp_1 = NULL;
    FILE *fp_2 = NULL;
    char str_1[200];
    char str_2[200];

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        if (opt == 's') {
            stat_print = true;
        } else if (opt == 'h') {
            help = true;
        } else {
            choices[num_choices] = opt;
            num_choices++;

            if (opt == 't') {
                t_size = atoi(optarg);
            } else if (opt == 'f') {
                f_size = atoi(optarg);
            }
        }
    }
    if (help) {
        usage();
        return 0;
    }

    bf_ret = bf_create(f_size);
    ht_ret = ht_create(t_size);

    if (bf_ret != NULL && ht_ret != NULL) {
        fp_1 = fopen("badspeak.txt", "r");

        while (fscanf(fp_1, "%s", str_1) != EOF) {
            bf_insert(bf_ret, str_1);
            ht_insert(ht_ret, str_1, NULL);
        }

        fclose(fp_1);

        fp_2 = fopen("newspeak.txt", "r");

        uint32_t toggle = 1;
        char oldspeak[200];
        char newspeak[200];
        while (fscanf(fp_2, "%s", str_2) != EOF) {
            if ((toggle % 2) > 0) {
                // oldspeak
                strcpy(oldspeak, str_2);
                bf_insert(bf_ret, oldspeak);
            } else {
                strcpy(newspeak, str_2);
                ht_insert(ht_ret, oldspeak, newspeak);
            }

            toggle++;
        }
    }

    fclose(fp_2);

    struct word_list {
        struct word_list *next;
        Node *n;
    };

    struct word_list *rs_root = NULL;
    struct word_list *tc_root = NULL;

    // modified Dr. Long's code in the assignment instructions  from here to line 150
    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) {
        fprintf(stderr, " Failed to compile regex .\n");
        return 1;
    }

    char *word = NULL;
    while ((word = next_word(stdin, &re)) != NULL) {
        if (bf_probe(bf_ret, word) == true) {
            Node *hash_ret = ht_lookup(ht_ret, word);
            if (hash_ret != NULL) {
                if (hash_ret->newspeak != NULL) {
                    //rightspeak case
                    if (rs_root == NULL) {
                        rs_root = malloc(sizeof(struct word_list));
                        rs_root->next = NULL;
                        rs_root->n = hash_ret;
                    } else {
                        struct word_list *temp = malloc(sizeof(struct word_list));
                        temp->next = rs_root;
                        temp->n = hash_ret;
                        rs_root = temp;
                    }
                } else {
                    //thoughtcrime case
                    if (tc_root == NULL) {
                        tc_root = malloc(sizeof(struct word_list));
                        tc_root->next = NULL;
                        tc_root->n = hash_ret;
                    } else {
                        struct word_list *temp = malloc(sizeof(struct word_list));
                        temp->next = tc_root;
                        temp->n = hash_ret;
                        tc_root = temp;
                    }
                }
            }
        }
    }

    clear_words();
    regfree(&re);

    struct word_list *temp = NULL;

    if ((rs_root != NULL) && (tc_root != NULL)) {
        //mixspeak
        if (stat_print == false) {
            fprintf(stdout, "%s", mixspeak_message);
        }
        temp = tc_root;
        while (temp != NULL) {
            if (stat_print == false) {
                fprintf(stdout, "%s\n", (temp->n)->oldspeak);
            }
            temp = temp->next;
        }

        temp = rs_root;
        while (temp != NULL) {
            if (stat_print == false) {
                fprintf(stdout, "%s -> %s\n", (temp->n)->oldspeak, (temp->n)->newspeak);
            }
            temp = temp->next;
        }
    } else if ((rs_root != NULL) && (tc_root == NULL)) {
        //rightspeak
        if (stat_print == false) {
            fprintf(stdout, "%s", goodspeak_message);
        }
        temp = rs_root;
        while (temp != NULL) {
            if (stat_print == false) {
                fprintf(stdout, "%s -> %s\n", (temp->n)->oldspeak, (temp->n)->newspeak);
            }
            temp = temp->next;
        }
    } else if ((rs_root == NULL) && (tc_root != NULL)) {
        //thoughtcrime
        if (stat_print == false) {
            fprintf(stdout, "%s", badspeak_message);
        }
        temp = tc_root;
        while (temp != NULL) {
            if (stat_print == false) {
                fprintf(stdout, "%s\n", (temp->n)->oldspeak);
            }
            temp = temp->next;
        }
    }

    // freeing up allocated memory
    while (rs_root != NULL) {
        struct word_list *temp = rs_root->next;
        free(rs_root);
        rs_root = temp;
    }

    while (tc_root != NULL) {
        struct word_list *temp = tc_root->next;
        free(tc_root);
        tc_root = temp;
    }

    double average_sz = 0;
    double average_ht = 0;
    double average_branches = 0;
    double hash_table_load = 0;
    double bloom_filter_load = 0;

    if (stat_print == true) {
        average_sz = ht_avg_bst_size(ht_ret);
        average_ht = ht_avg_bst_height(ht_ret);
        average_branches = (double) branches / (double) lookups;
        hash_table_load = ((double) (ht_count(ht_ret)) / (double) (ht_size(ht_ret))) * 100;
        bloom_filter_load = ((double) (bf_count(bf_ret)) / (double) (bf_size(bf_ret))) * 100;

        fprintf(stdout, "Average BST size: %.6f\n", average_sz);
        fprintf(stdout, "Average BST height: %.6f\n", average_ht);
        fprintf(stdout, "Average branches traversed: %.6f\n", average_branches);
        fprintf(stdout, "Hash table load: %.6f%%\n", hash_table_load);
        fprintf(stdout, "Bloom filter load: %.6f%%\n", bloom_filter_load);
    }

    ht_delete(&ht_ret);
    bf_delete(&bf_ret);
}

void usage(void) {
    printf("SYNOPSIS\n");
    printf("\tA word filtering program for the GPRSC.\n");
    printf("\tFilters out and reports bad words parsed from stdin.\n\n");
    printf("USAGE\n");
    printf("\t./banhammer [-hs] [-t size] [-f size]\n\n");
    printf("OPTIONS\n");
    printf("\t-h\t\tProgram usage and help.\n");
    printf("\t-s\t\tPrint program statistics.\n");
    printf("\t-t size\t\tSpecify hash table size (default: 10000).\n");
    printf("\t-f size\t\tSpecify Bloom filter size (default: 2^20).\n");
}

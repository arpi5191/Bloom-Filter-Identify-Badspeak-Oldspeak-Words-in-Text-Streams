#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "salts.h"
#include "bv.h"
#include "bf.h"
#include "node.h"

Node *node_create(char *oldspeak, char *newspeak) {
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        return NULL;
    }

    if (oldspeak != NULL) {
        n->oldspeak = strdup(oldspeak);
    } else
        n->oldspeak = NULL;

    if (newspeak != NULL) {
        n->newspeak = strdup(newspeak);
    } else
        n->newspeak = NULL;

    n->left = NULL;
    n->right = NULL;
    return n;
}

void node_delete(Node **n) {
    if (*n == NULL) {
        fprintf(stderr, "Error: node pointer is NULL\n");
        return;
    }

    if (((*n)->left == NULL) && ((*n)->right == NULL)) {
        free((*n)->oldspeak);
        free((*n)->newspeak);
        free(*n);
        *n = NULL;
    } else if (((*n)->left != NULL) && ((*n)->right == NULL)) {
        strcpy((*n)->oldspeak, ((*n)->left)->oldspeak);
        strcpy((*n)->newspeak, ((*n)->left)->newspeak);
        free(((*n)->left)->oldspeak);
        free(((*n)->left)->newspeak);
        free((*n)->left);
        (*n)->left = NULL;
    } else if (((*n)->left == NULL) && ((*n)->right != NULL)) {
        strcpy((*n)->oldspeak, ((*n)->right)->oldspeak);
        strcpy((*n)->newspeak, ((*n)->right)->newspeak);
        free(((*n)->right)->oldspeak);
        free(((*n)->right)->newspeak);
        free((*n)->right);
        (*n)->right = NULL;
    } else {
        Node *right_subtree = (*n)->right;
        Node *left_p = right_subtree->left;
        if (left_p == NULL) {
            // right_subtree has no child on left, so promote right_subtree to n
            strcpy((*n)->oldspeak, right_subtree->oldspeak);
            strcpy((*n)->newspeak, right_subtree->newspeak);
            (*n)->right = right_subtree->right;
            free(right_subtree->oldspeak);
            free(right_subtree->newspeak);
            free(right_subtree);
        } else {
            Node *parent = right_subtree;
            while (left_p->left != NULL) {
                parent = left_p;
                left_p = left_p->left;
            }

            strcpy((*n)->oldspeak, left_p->oldspeak);
            strcpy((*n)->newspeak, left_p->newspeak);
            parent->left = left_p->right;
            free(left_p->oldspeak);
            free(left_p->newspeak);
            free(left_p);
        }
    }
}

void node_print(Node *n) {
    if ((n->oldspeak != NULL) && (n->newspeak != NULL)) {
        printf("%s -> %s\n", n->oldspeak, n->newspeak);
    } else if ((n->oldspeak != NULL) && (n->newspeak == NULL)) {
        printf("%s\n", n->oldspeak);
    }
}

#include "bf.h"
#include "bst.h"
#include "bv.h"
#include "node.h"
#include "salts.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t branches = 0;

Node *bst_create(void) {
    return NULL;
}

void bst_delete(Node **root) {
    if (*root == NULL) {
        return;
    }
    while (*root) {
        bst_delete(&((*root)->left));
        bst_delete(&((*root)->right));
        node_delete(root);
    }
}

uint32_t bst_height(Node *root) {
    uint32_t height = 0;
    uint32_t left_height = 0;
    uint32_t right_height = 0;

    if (root == NULL) {
        return 0;
    }

    height++;

    left_height = left_height + bst_height(root->left);
    right_height = right_height + bst_height(root->right);

    if (left_height >= right_height) {
        height += left_height;
    } else {
        height += right_height;
    }

    return height;
}

uint32_t bst_size(Node *root) {
    uint32_t sz = 0;

    if (root == NULL) {
        return 0;
    }

    sz++;

    sz = sz + bst_size(root->left);
    sz = sz + bst_size(root->right);

    return sz;
}

Node *bst_find(Node *root, char *oldspeak) {
    Node *find_node = root;

    while (find_node != NULL) {
        if (strcmp(oldspeak, find_node->oldspeak) < 0) {
            // go left
            find_node = find_node->left;
            branches++;
        } else if (strcmp(oldspeak, find_node->oldspeak) > 0) {
            // go right
            find_node = find_node->right;
            branches++;
        } else if (strcmp(oldspeak, find_node->oldspeak) == 0) {
            // found it
            return find_node;
        } else {
            break;
        }
    }

    return NULL;
}

Node *bst_insert(Node *root, char *oldspeak, char *newspeak) {
    Node *ret_node = NULL;

    // if NULL insert node
    if (root == NULL) {
        // this case handles the very first insertion i.e., when tree is empty
        ret_node = node_create(oldspeak, newspeak);
        return ret_node;
    } else {
        if (strcmp(oldspeak, root->oldspeak) < 0) {
            // left
            branches++;
            if (root->left != NULL) {
                ret_node = bst_insert(root->left, oldspeak, newspeak);
            } else {
                ret_node = node_create(oldspeak, newspeak);
                root->left = ret_node;
            }
        } else if (strcmp(oldspeak, root->oldspeak) > 0) {
            // right
            branches++;
            if (root->right != NULL) {
                ret_node = bst_insert(root->right, oldspeak, newspeak);
            } else {
                ret_node = node_create(oldspeak, newspeak);
                root->right = ret_node;
            }

        } else {
            // oldspeak already exists, so do not insert
            return NULL;
        }

        return ret_node;
    }
}

void bst_print(Node *root) {
    if (root == NULL) {
        return;
    }

    bst_print(root->left);
    node_print(root);
    bst_print(root->right);
}

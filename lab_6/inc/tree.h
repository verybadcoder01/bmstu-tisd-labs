#ifndef TREE_H__
#define TREE_H__

#include "node.h"
#include "errors.h"
#include "stdio.h"

typedef struct {
    node_t *root;
} bin_tree;

void tree_delete(node_t *root);

error tree_insert(node_t **root, char *surname, double gpa);

error tree_remove(node_t **root, const char *surname);

error tree_find(node_t *root, const char *surname, node_t **result);

error tree_remove_below(node_t **root, double gpa_bar);

typedef enum {
    PREFIX = 1,
    INFIX,
    POSTFIX
} walk_order;

void tree_walk(node_t *root, walk_order order);

void serialize_to_graphviz(node_t *root, FILE *fout);

#endif
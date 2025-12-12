#ifndef TREE_H__
#define TREE_H__

#include "errors.h"
#include "node.h"
#include "stdio.h"

typedef int (*comp_fn)(const node_t *, const void *);

int comp_node_data(const node_t *n, const void *data);

void tree_delete(node_t *root);

error tree_insert(node_t **root, comp_fn comp, int data);

error tree_remove(node_t **root, comp_fn comp, const void *data);

error tree_find(node_t *root, comp_fn comp, const void *data, int *cmp_count);

typedef enum
{
    PREFIX = 1,
    INFIX,
    POSTFIX
} walk_order;

void tree_walk(node_t *root, walk_order order);

void serialize_to_graphviz(node_t *root, FILE *fout);

#endif
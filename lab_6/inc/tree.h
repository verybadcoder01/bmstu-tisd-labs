#ifndef TREE_H__
#define TREE_H__

#include "errors.h"
#include "node.h"
#include "stdio.h"

typedef int (*comp_fn)(const node_t *, const void *);

int comp_node_surname(const node_t *n, const void *str);

int comp_node_gpa(const node_t *n, const void *gpa);

void tree_delete(node_t *root);

error tree_insert(node_t **root, comp_fn comp, char *surname, double gpa);

error tree_remove(node_t **root, comp_fn comp, const void *surname);

error tree_find(node_t *root, comp_fn comp, const void *surname, node_t **result);

error tree_remove_below(node_t **root, comp_fn comp, double gpa_bar);

typedef enum
{
    PREFIX = 1,
    INFIX,
    POSTFIX
} walk_order;

void tree_walk(node_t *root, walk_order order);

void serialize_to_graphviz(node_t *root, FILE *fout);

size_t tree_to_sorted_array(node_t *root, char **surnames, double *gpas, size_t index);

int tree_depth(node_t *root);

#endif
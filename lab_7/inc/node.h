#ifndef NODE_H__
#define NODE_H__

#include "stdio.h"

typedef struct node node_t;

struct node {
    int data;
    int height;
    node_t *left, *right;
};

node_t *node_create(int data);

void node_delete(node_t *node);

void node_print(node_t *node);

void print_node_for_graphviz(node_t *node, FILE *fout);

#endif
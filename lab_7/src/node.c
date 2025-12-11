#define _GNU_SOURCE
#include "node.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

node_t *node_create(int data)
{
    node_t *node = malloc(sizeof(node_t));
    if (!node)
    {
        return NULL;
    }
    node->data = data;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void node_delete(node_t *node)
{
    free(node);
}

void node_print(node_t *node)
{
    printf("%d\n", node->data);
}

void print_node_for_graphviz(node_t *node, FILE *fout)
{
    if (!node->left && !node->right)
    {
        fprintf(fout, "\"%d\";\n", node->data);
    }
    if (node->left)
    {
        fprintf(fout, "\"%d\"", node->data);
        fprintf(fout, "->\"%d\";\n", node->left->data);
    }
    if (node->right)
    {
        fprintf(fout, "\"%d\"->", node->data);
        fprintf(fout, "\"%d\";\n", node->right->data);
    }
}

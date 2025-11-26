#define _GNU_SOURCE
#include "node.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

node_t *node_create(char *surname, double gpa)
{
    node_t *node = malloc(sizeof(node_t));
    if (!node)
    {
        return NULL;
    }
    node->surname = strdup(surname);
    if (!node->surname)
    {
        return NULL;
    }
    node->gpa = gpa;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void node_delete(node_t *node)
{
    if (!node)
    {
        return;
    }
    free(node->surname);
    free(node);
}

void node_print(node_t *node)
{
    printf("%s\n", node->surname);
    printf("%lf\n", node->gpa);
}

void print_node_for_graphviz(node_t *node, FILE *fout)
{
    if (!node->left && !node->right)
    {
        fprintf(fout, "\"%s %lf\";\n", node->surname, node->gpa);
    }
    if (node->left)
    {
        fprintf(fout, "\"%s %lf\"", node->surname, node->gpa);
        fprintf(fout, "->\"%s %lf\";\n", node->left->surname, node->left->gpa);
    }
    if (node->right)
    {
        fprintf(fout, "\"%s %lf\"->", node->surname, node->gpa);
        fprintf(fout, "\"%s %lf\";\n", node->right->surname, node->right->gpa);
    }
}

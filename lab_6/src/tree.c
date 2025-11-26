#include "tree.h"
#include "errors.h"
#include "node.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

void tree_delete(node_t *root)
{
    if (!root)
    {
        return;
    }
    node_t *left_nxt = root->left, *right_nxt = root->right;
    node_delete(root);
    tree_delete(left_nxt);
    tree_delete(right_nxt);
}

void tree_walk(node_t *root, walk_order order)
{
    if (!root)
    {
        return;
    }
    if (order == PREFIX)
    {
        node_print(root);
    }
    tree_walk(root->left, order);
    if (order == INFIX)
    {
        node_print(root);
    }
    tree_walk(root->right, order);
    if (order == POSTFIX)
    {
        node_print(root);
    }
}

error tree_insert(node_t **root, char *surname, double gpa)
{
    if (!*root)
    {
        *root = node_create(surname, gpa);
        if (!*root)
        {
            return ALLOC_ERROR;
        }
        return 0;
    }
    int cmp_val = strcmp((*root)->surname, surname);
    if (!cmp_val)
    {
        return DUPLICATE_KEY;
    }
    if (cmp_val < 0)
    {
        return tree_insert(&(*root)->left, surname, gpa);
    }
    else
    {
        return tree_insert(&(*root)->right, surname, gpa);
    }
}

static node_t *find_min(node_t *root)
{
    while (root && root->left)
    {
        root = find_min(root->left);
    }
    return root;
}

error tree_remove(node_t **root, const char *surname)
{
    if (!root || !*root)
    {
        return KEY_NOT_FOUND;
    }
    int cmp_val = strcmp((*root)->surname, surname);
    if (cmp_val < 0)
    {
        return tree_remove(&(*root)->left, surname);
    }
    else if (cmp_val > 0)
    {
        return tree_remove(&(*root)->right, surname);
    }
    else
    {
        if (!(*root)->left)
        {
            node_t *tmp = (*root)->right;
            node_delete(*root);
            *root = tmp;
            return 0;
        }
        else if (!(*root)->right)
        {
            node_t *tmp = (*root)->left;
            node_delete(*root);
            *root = tmp;
            return 0;
        }
        node_t *tmp = find_min((*root)->right);
        char *surname_copy = strdup(tmp->surname);
        free((*root)->surname);
        (*root)->surname = surname_copy;
        (*root)->gpa = tmp->gpa;
        return tree_remove(&(*root)->right, tmp->surname);
    }
}

error tree_remove_below(node_t **root, double gpa_bar)
{
    if (!root || !*root)
    {
        return 0;
    }
    error rc = 0;
    if ((*root)->gpa < gpa_bar)
    {
        rc = tree_remove(root, (*root)->surname);
    }
    if (rc)
    {
        return rc;
    }
    if (*root)
    {
        rc = tree_remove_below(&(*root)->left, gpa_bar);
        if (rc)
        {
            return rc;
        }
        rc = tree_remove_below(&(*root)->right, gpa_bar);
        return rc;
    }
    return 0;
}

error tree_find(node_t *root, const char *surname, node_t **result)
{
    if (!root)
    {
        return KEY_NOT_FOUND;
    }
    int cmp_val = strcmp(root->surname, surname);
    if (!cmp_val)
    {
        *result = root;
        return 0;
    }
    if (cmp_val < 0)
    {
        return tree_find(root->left, surname, result);
    }
    else
    {
        return tree_find(root->right, surname, result);
    }
}

static void tree_to_graphviz(node_t *root, FILE *fout)
{
    if (!root)
    {
        return;
    }
    print_node_for_graphviz(root, fout);
    tree_to_graphviz(root->left, fout);
    tree_to_graphviz(root->right, fout);
}

void serialize_to_graphviz(node_t *root, FILE *fout)
{
    fprintf(fout, "digraph BasicTree {\n");
    tree_to_graphviz(root, fout);
    fprintf(fout, "}\n");
}
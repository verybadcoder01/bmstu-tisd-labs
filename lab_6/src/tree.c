#include "tree.h"
#include "errors.h"
#include "node.h"
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

int comp_node_surname(const node_t *n, const void *str)
{
    return strcmp(n->surname, (const char *)str);
}

int comp_node_gpa(const node_t *n, const void *gpa)
{
    double right = *((const double *)gpa);
    if (n->gpa > right)
    {
        return 1;
    }
    else if (right > n->gpa)
    {
        return -1;
    }
    return 0;
}

error tree_insert(node_t **root, comp_fn comp, char *surname, double gpa)
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
    int cmp_val = comp(*root, surname);
    if (!cmp_val)
    {
        return DUPLICATE_KEY;
    }
    if (cmp_val < 0)
    {
        return tree_insert(&(*root)->left, comp, surname, gpa);
    }
    else
    {
        return tree_insert(&(*root)->right, comp, surname, gpa);
    }
}

error tree_remove(node_t **root, comp_fn comp, const void *surname)
{
    if (!root || !*root)
    {
        return KEY_NOT_FOUND;
    }
    int cmp_val = comp(*root, surname);
    if (cmp_val < 0)
    {
        return tree_remove(&(*root)->left, comp, surname);
    }
    else if (cmp_val > 0)
    {
        return tree_remove(&(*root)->right, comp, surname);
    }
    else
    {
        node_t *node_to_delete = *root;
        if (!node_to_delete->left)
        {
            *root = node_to_delete->right;
            node_delete(node_to_delete);
            return 0;
        }
        else if (!node_to_delete->right)
        {
            *root = node_to_delete->left;
            node_delete(node_to_delete);
            return 0;
        }
        else
        {
            node_t **successor_ptr = &node_to_delete->right;
            while ((*successor_ptr)->left != NULL)
            {
                successor_ptr = &(*successor_ptr)->left;
            }
            node_t *successor = *successor_ptr;
            *successor_ptr = successor->right;
            successor->left = node_to_delete->left;
            successor->right = node_to_delete->right;
            *root = successor;
            node_delete(node_to_delete);
            return 0;
        }
    }
}

error tree_remove_below(node_t **root, comp_fn comp, double gpa_bar)
{
    if (!root || !*root)
    {
        return 0;
    }
    error rc = 0;
    if ((*root)->gpa <= gpa_bar)
    {
        rc = tree_remove(root, comp, (*root)->surname);
    }
    if (rc)
    {
        return rc;
    }
    if (*root)
    {
        rc = tree_remove_below(&(*root)->left, comp, gpa_bar);
        if (rc)
        {
            return rc;
        }
        rc = tree_remove_below(&(*root)->right, comp, gpa_bar);
        return rc;
    }
    return 0;
}

error tree_find(node_t *root, comp_fn comp, const void *surname, node_t **result)
{
    if (!root)
    {
        return KEY_NOT_FOUND;
    }
    int cmp_val = comp(root, surname);
    if (!cmp_val)
    {
        *result = root;
        return 0;
    }
    if (cmp_val < 0)
    {
        return tree_find(root->left, comp, surname, result);
    }
    else
    {
        return tree_find(root->right, comp, surname, result);
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

size_t tree_to_sorted_array(node_t *root, char **surnames, double *gpas, size_t index)
{
    if (!root)
    {
        return index;
    }

    index = tree_to_sorted_array(root->left, surnames, gpas, index);

    surnames[index] = root->surname;
    gpas[index] = root->gpa;
    index++;

    index = tree_to_sorted_array(root->right, surnames, gpas, index);
    return index;
}

int tree_depth(node_t *root) {
    if (!root) {
        return -1;
    }
    int dleft = tree_depth(root->left);
    int dright = tree_depth(root->right);
    return 1 + (dleft > dright ? dleft : dright);
}

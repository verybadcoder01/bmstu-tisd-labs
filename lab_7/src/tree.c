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

static int height(node_t *root)
{
    if (!root)
    {
        return 0;
    }
    return root->height;
}

static int bfactor(node_t *root)
{
    return height(root->right) - height(root->left);
}

static void set_height(node_t *root)
{
    int hl = height(root->left);
    int hr = height(root->right);
    root->height = (hl > hr ? hl : hr) + 1;
}

static node_t *rotate_right(node_t *root)
{
    node_t *q = root->left;
    root->left = q->right;
    q->right = root;
    set_height(root);
    set_height(q);
    return q;
}

static node_t *rotate_left(node_t *root)
{
    node_t *p = root->right;
    root->right = p->left;
    p->left = root;
    set_height(root);
    set_height(p);
    return p;
}

static node_t *balance(node_t *root)
{
    set_height(root);
    if (bfactor(root) > 1)
    {
        if (bfactor(root->right) < 0)
        {
            root->right = rotate_right(root->right);
            set_height(root);
        }
        return rotate_left(root);
    }
    if (bfactor(root) < -1)
    {
        if (bfactor(root->left) > 0)
        {
            root->left = rotate_left(root->left);
            set_height(root);
        }
        return rotate_right(root);
    }
    return root;
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

int comp_node_data(const node_t *n, const void *data)
{
    return n->data - *((const int *)data);
}

error tree_insert(node_t **root, comp_fn comp, int data)
{
    if (!*root)
    {
        *root = node_create(data);
        if (!*root)
        {
            return ALLOC_ERROR;
        }
        return 0;
    }
    int cmp_val = comp(*root, &data);
    if (!cmp_val)
    {
        return DUPLICATE_KEY;
    }
    error rc = 0;
    if (cmp_val < 0)
    {
        rc = tree_insert(&(*root)->left, comp, data);
    }
    else
    {
        rc = tree_insert(&(*root)->right, comp, data);
    }
    if (rc)
    {
        return rc;
    }
    *root = balance(*root);
    return 0;
}

error tree_remove(node_t **root, comp_fn comp, const void *data)
{
    if (!root || !*root)
    {
        return KEY_NOT_FOUND;
    }
    
    int cmp_val = comp(*root, data);
    error rc = 0;
    
    if (cmp_val < 0)
    {
        rc = tree_remove(&(*root)->left, comp, data);
    }
    else if (cmp_val > 0)
    {
        rc = tree_remove(&(*root)->right, comp, data);
    }
    else
    {
        node_t *node_to_delete = *root;
        if (!node_to_delete->left || !node_to_delete->right)
        {
            *root = (node_to_delete->left) ? node_to_delete->left : node_to_delete->right;
            node_delete(node_to_delete);
        }
        else
        {
            node_t *successor_parent = node_to_delete;
            node_t *successor = node_to_delete->right;
            while (successor->left != NULL)
            {
                successor_parent = successor;
                successor = successor->left;
            }
            if (successor_parent != node_to_delete)
            {
                successor_parent->left = successor->right;
            }
            else
            {
                node_to_delete->right = successor->right;
            }
            node_to_delete->data = successor->data;
            node_delete(successor);
            set_height(*root);
        }
    }
    if (rc){
        return rc;
    }
    if (*root) {
        *root = balance(*root);
    }
    return 0;
}

error tree_find(node_t *root, comp_fn comp, const void *data, int *cmp_count)
{
    if (!root)
    {
        return KEY_NOT_FOUND;
    }
    (*cmp_count)++;
    int cmp_val = comp(root, data);
    if (!cmp_val)
    {
        return 0;
    }
    if (cmp_val < 0)
    {
        return tree_find(root->left, comp, data, cmp_count);
    }
    else
    {
        return tree_find(root->right, comp, data, cmp_count);
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

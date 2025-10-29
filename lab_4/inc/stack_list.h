#ifndef STACK_LIST_H__
#define STACK_LIST_H__

#include "stdlib.h"
#include "errors.h"

#define MAX_STACK_LIST_SIZE 10000

typedef struct node node;

struct node
{
    char *data;
    node *next;
};

typedef struct
{
    node *head;
    size_t size;
    const node **freed_ptrs;
    size_t freed_count, freed_cap;
} stack_list;

error create_node(node **res, char *data);

void delete_node(node **data);

error stack_list_push(stack_list *st, char *n_elem);

error add_freed_ptr(stack_list *st, const node *ptr);

error stack_list_pop(stack_list *st);

void stack_list_print(stack_list *st);

void print_node_rev_recursive(node *n);

void stack_list_print_rev(stack_list *st);

void delete_stack_list(stack_list *st);

#endif
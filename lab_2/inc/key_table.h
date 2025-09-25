#ifndef KEY_TABLE_H__
#define KEY_TABLE_H__

#include "stddef.h"

typedef struct
{
    size_t source_ind;
    char *key_value; // title
} key_table_elem;

typedef struct {
    key_table_elem* data;
    size_t size;
} key_table;

int key_elem_comp(const void *left, const void *right);

void delete_key_table_elem(key_table_elem *elem);

void delete_key_table(key_table *tab);

void print_key_table_elem(key_table_elem *elem);

#endif
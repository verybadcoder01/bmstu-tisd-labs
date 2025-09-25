#ifndef TABLE_H__
#define TABLE_H__

#include "book_types.h"
#include "stddef.h"
#include "key_table.h"
#include "sort.h"

typedef struct
{
    book *data;
    size_t size, capacity;
    key_table *keys;
    bool was_change;
} table;

void init_default_table(table *tab);

int comp_book_t(const void *l, const void *r);

error push_back(table *tab, book *n_book);

error remove_elem(table *tab, char *target_key);

error build_key_table(table *tab);

error sort_key_table(table *tab, sort_type method);

error print_sorted_via_key_table(table *tab, sort_type method);

void print_by_variant(table *tab, char *target);

void delete_table(table *tab);

void print_key_table(table *tab);

error init_table_from_file(FILE *f, table *tab);

void print_table(table *tab);

error self_perf_test(FILE *f, unsigned long *res_bubble_orig, unsigned long *res_merge_orig, unsigned long *res_bubble_keys, unsigned long *res_merge_keys);

#endif
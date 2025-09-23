#ifndef TABLE_H__
#define TABLE_H__

#include "book_types.h"
#include "stddef.h"
#include "key_table.h"

typedef struct {
    book* data;
    size_t size, capacity;
    key_table* keys;
    bool was_change;
} table;

int comp_book_t(const book* l, const book *r);

error push_back(table *tab, book *n_book);

error remove_elem(table *tab, char* target_key);

static error build_key_table(table *tab);

error sort_key_table(table *tab, sort_type method);

error print_sorted_via_key_table(table *tab, sort_type method);

void print_by_variant(table *tab, char *target);

#endif
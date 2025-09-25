#include "key_table.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

int key_elem_comp(const void *left, const void *right) {
    return strcmp(((key_table_elem*)left)->key_value, ((key_table_elem*)right)->key_value);
}

void delete_key_table(key_table *tab){
    if (!tab){
        return;
    }
    for (size_t i = 0; i < tab->size; ++i){
        free(tab->data[i].key_value);
        tab->data[i].key_value = NULL;
    }
    free(tab->data);
    tab->data = NULL;
    free(tab);
    tab = NULL;
}

void print_key_table_elem(key_table_elem *elem) {
    printf("%zu %s", elem->source_ind, elem->key_value);
}

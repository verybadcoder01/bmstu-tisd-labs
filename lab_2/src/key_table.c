#include "key_table.h"
#include "string.h"
#include "stdlib.h"

int key_elem_comp(const key_table_elem *left, const key_table_elem *right) {
    return strcmp(left->key_value, right->key_value);
}

void delete_key_table_elem(key_table_elem *elem) {
    free(elem->key_value);
    elem->key_value = NULL;
    free(elem);
    elem = NULL;
}

void delete_key_table(key_table *tab){
    if (!tab){
        return;
    }
    for (size_t i = 0; i < tab->size; ++i){
        delete_key_table_elem(&tab->data[i]);
    }
    free(tab);
    tab = NULL;
}
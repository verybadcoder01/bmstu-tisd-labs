#include "table.h"
#include "malloc.h"
#include "errors.h"
#include "memory.h"
#include "sort.h"

error push_back(table *tab, book *n_book)
{
    if (tab->size < tab->capacity)
    {
        error rc = deep_copy(&tab->data[tab->size], n_book);
        if (rc)
        {
            delete_inner_book_info(&tab->data[tab->size]);
            return rc;
        }
        tab->size++;
        tab->was_change = true;
        return 0;
    }
    tab->data = realloc(tab->data, (tab->size + 1) * sizeof(book));
    tab->capacity++;
    error rc = deep_copy(&tab->data[tab->size], n_book);
    if (rc)
    {
        delete_inner_book_info(&tab->data[tab->size]);
        return rc;
    }
    tab->size++;
    tab->was_change = true;
    return 0;
}

error remove_elem(table *tab, char *target_key)
{
    size_t target_ind = tab->size;
    for (size_t i = 0; i < tab->size; ++i)
    {
        if (!strcmp(tab->data[i].title, target_key))
        {
            target_ind = i;
        }
    }
    if (target_ind == tab->size)
    {
        return NO_ELEM_TO_REMOVE;
    }
    for (size_t i = target_ind; i < tab->size - 1; ++i)
    {
        delete_inner_book_info(&tab->data[i]);
        error rc = deep_copy(&tab->data[i], &tab->data[i + 1]);
        if (rc)
        {
            delete_inner_book_info(&tab->data[i]);
            return rc;
        }
    }
    delete_inner_book_info(&tab->data[tab->size - 1]);
    tab->size--;
    tab->was_change = true;
}

static error build_key_table(table *tab)
{
    tab->keys->data = malloc(tab->size * sizeof(key_table_elem));
    if (!tab->keys->data)
    {
        return ALLOC_ERROR;
    }
    tab->keys->size = tab->size;
    for (size_t i = 0; i < tab->size; ++i)
    {
        tab->keys->data[i].key_value = tab->data[i].title;
        tab->keys->data[i].source_ind = i;
    }
}

error sort_key_table(table *tab, sort_type method)
{
    if (!tab->keys)
    {
        error rc = build_key_table(tab);
        if (rc)
        {
            return rc;
        }
    }
    if (tab->was_change)
    {
        delete_key_table(tab->keys);
        tab->keys = malloc(sizeof(key_table));
        if (!tab->keys)
        {
            return ALLOC_ERROR;
        }
        error rc = build_key_table(tab);
        if (rc)
        {
            return rc;
        }
        tab->was_change = false;
    }
    if (method == BUBBLE)
    {
        bubble_sort(tab->keys, tab->keys->size, sizeof(key_table_elem), key_elem_comp);
    }
    else
    {
        merge_sort(tab->keys, tab->keys->size, sizeof(key_elem_comp), key_elem_comp);
    }
}

error print_sorted_via_key_table(table *tab, sort_type method)
{
    error rc = sort_key_table(tab, method);
    if (rc)
    {
        return rc;
    }
    for (size_t i = 0; i < tab->keys->size; ++i)
    {
        printf("--------------------------------------------------\n");
        print_book(&tab->data[tab->keys->data[i].source_ind]);
        printf("--------------------------------------------------\n");
    }
}

int comp_book_t(const book *l, const book *r)
{
    return strcmp(l->title, r->title);
}

error print_sorted(table *tab, sort_type method)
{
    if (method == BUBBLE)
    {
        bubble_sort(tab->data, tab->size, sizeof(book), &comp_book_t);
    }
    else
    {
        merge_sort(tab->data, tab->size, sizeof(book), &comp_book_t);
    }
    for (size_t i = 0; i < tab->size; ++i)
    {
        printf("--------------------------------------------------\n");
        print_book(&tab->data[i]);
        printf("--------------------------------------------------\n");
    }
}

void print_by_variant(table *tab, char *target)
{
    for (size_t i = 0; i < tab->size; ++i)
    {
        if (!strcmp(tab->data[i].author, target) && tab->data[i].type == FICTIONAL && tab->data[i].lit_info.fic.type == NOVEL)
        {
            printf("--------------------------------------------------\n");
            print_book(&tab->data[i]);
            printf("--------------------------------------------------\n");
        }
    }
}
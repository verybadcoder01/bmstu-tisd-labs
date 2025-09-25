#include "table.h"
#include "malloc.h"
#include "errors.h"
#include "memory.h"
#include "time.h"

error push_back(table *tab, book *n_book)
{
    if (tab->size < tab->capacity)
    {
        init_default_book(&tab->data[tab->size]);
        error rc = deep_copy(&tab->data[tab->size], n_book);
        if (rc)
        {
            return rc;
        }
        tab->size++;
        tab->was_change = true;
        return 0;
    }
    size_t n_cap = 0;
    if (tab->size == 0)
    {
        n_cap = 1;
    }
    else
    {
        n_cap = 2 * tab->size;
    }
    tab->data = realloc(tab->data, n_cap * sizeof(book));
    tab->capacity = n_cap;
    init_default_book(&tab->data[tab->size]);
    error rc = deep_copy(&tab->data[tab->size], n_book);
    if (rc)
    {
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
            return rc;
        }
    }
    delete_inner_book_info(&tab->data[tab->size - 1]);
    tab->size--;
    tab->was_change = true;
    return 0;
}

error build_key_table(table *tab)
{
    tab->keys = malloc(sizeof(key_table));
    if (!tab->keys){
        return ALLOC_ERROR;
    }
    tab->keys->data = malloc(tab->size * sizeof(key_table_elem));
    if (!tab->keys->data)
    {
        return ALLOC_ERROR;
    }
    tab->keys->size = tab->size;
    for (size_t i = 0; i < tab->size; ++i)
    {
        tab->keys->data[i].key_value = NULL;
        error rc = copy_str(&tab->keys->data[i].key_value, tab->data[i].title);
        if (rc){
            return rc;
        }
        tab->keys->data[i].source_ind = i;
    }
    return 0;
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
        error rc = build_key_table(tab);
        if (rc)
        {
            return rc;
        }
        tab->was_change = false;
    }
    if (method == BUBBLE)
    {
        bubble_sort(tab->keys->data, tab->keys->size, sizeof(key_table_elem), &key_elem_comp);
    }
    else
    {
        merge_sort(tab->keys->data, tab->keys->size, sizeof(key_table_elem), &key_elem_comp);
    }
    return 0;
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
    return 0;
}

int comp_book_t(const void *l, const void *r)
{
    return strcmp(((book*)l)->title, ((book*)r)->title);
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
    return 0;
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

void delete_table(table *tab)
{
    if (!tab)
    {
        return;
    }
    if (tab->keys)
    {
        delete_key_table(tab->keys);
        tab->keys = NULL;
    }
    for (size_t i = 0; i < tab->size; ++i)
    {
        delete_inner_book_info(&tab->data[i]);
    }
    free(tab->data);
    tab->data = NULL;
    free(tab);
    tab = NULL;
}

void init_default_table(table *tab) {
    tab->capacity = 0;
    tab->size = 0;
    tab->was_change = false;
    tab->keys = NULL;
    tab->data = NULL;
}

error init_table_from_file(FILE *f, table *tab)
{
    init_default_table(tab);
    while (1)
    {
        book *n_book = malloc(sizeof(book));
        if (!n_book)
        {
            return ALLOC_ERROR;
        }
        error rc = scan_book(f, n_book);
        if (rc && rc != EOF)
        {
            delete_book(n_book);
            return rc;
        }
        if (rc == EOF)
        {
            break;
        }
        rc = push_back(tab, n_book);
        if (rc)
        {
            delete_book(n_book);
            return rc;
        }
    }
    fseek(f, 0, SEEK_SET);
    error rc = build_key_table(tab);
    if (rc)
    {
        return rc;
    }
    return 0;
}

void print_key_table(table *tab)
{
    size_t max_print_sz = tab->keys->size;
    if (tab->keys->size > 40){
        max_print_sz = 40;
    }
    for (size_t i = 0; i < max_print_sz; ++i)
    {
        printf("--------------------------------------------------\n");
        print_key_table_elem(&tab->keys->data[i]);
        printf("\n");
        printf("--------------------------------------------------\n");
    }
}

void print_table(table *tab)
{
    size_t max_print_sz = tab->size;
    if (tab->size > 40){
        max_print_sz = 40;
    }
    for (size_t i = 0; i < max_print_sz; ++i)
    {
        printf("--------------------------------------------------\n");
        print_book(&tab->data[i]);
        printf("--------------------------------------------------\n");
    }
}

error self_perf_test(FILE *f, unsigned long *res_bubble_orig, unsigned long *res_merge_orig, unsigned long *res_bubble_keys, unsigned long *res_merge_keys)
{
    table *tab;
    tab = malloc(sizeof(table));
    if (!tab){
        return ALLOC_ERROR;
    }
    // original table, bubble sort
    error rc = init_table_from_file(f, tab);
    if (rc){
        delete_table(tab);
        return rc;
    }
    rc = measure_sort_time(res_bubble_orig, &bubble_sort, tab->data, tab->size, sizeof(book), &comp_book_t);
    delete_table(tab);
    if (rc){
        return rc;
    }
    // original table, merge sort
    tab = malloc(sizeof(table));
    if (!tab){
        return ALLOC_ERROR;
    }
    rc = init_table_from_file(f, tab);
    if (rc){
        delete_table(tab);
        return rc;
    }
    rc = measure_sort_time(res_merge_orig, &merge_sort, tab->data, tab->size, sizeof(book), &comp_book_t);
    delete_table(tab);
    if (rc){
        return rc;
    }
    // keys table, bubble sort
    tab = malloc(sizeof(table));
    if (!tab){
        return ALLOC_ERROR;
    }
    rc = init_table_from_file(f, tab);
    if (rc){
        delete_table(tab);
        return rc;
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    rc = sort_key_table(tab, BUBBLE);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    delete_table(tab);
    if (rc){
        return rc;
    }
    *res_bubble_keys = elapsed_time(&start, &end);
    // keys table, merge sort
    tab = malloc(sizeof(table));
    if (!tab){
        return ALLOC_ERROR;
    }
    rc = init_table_from_file(f, tab);
    if (rc){
        delete_table(tab);
        return rc;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    rc = sort_key_table(tab, MERGE);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    delete_table(tab);
    if (rc){
        return rc;
    }
    *res_merge_keys = elapsed_time(&start, &end);
    return 0;
}

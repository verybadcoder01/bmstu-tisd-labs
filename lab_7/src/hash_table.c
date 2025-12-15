#include "hash_table.h"
#include "errors.h"
#include "stdio.h"
#include "time.h"
#include <stdbool.h>
#include <stdlib.h>
#include "math.h"

// основная хеш-функция
int hash_knuth_xor(const int key, const int m)
{
    unsigned int k = (unsigned int)key;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = (k >> 16) ^ k;
    return k % m;
}

int hash_mult(const int key, const int m) {
    const unsigned int A = 2654435761U;
    unsigned int k = (unsigned int)key;
    return (int)((A * k) >> (32 - (int)log2(m))) % m;
}

// вторая хеш-функция, [1; m-1]
int second_hash_func(const int key, const int mod)
{
    unsigned int x = (unsigned int)key;
    return 1 + ((x + mod) % (mod - 1));
}

int second_hash_probe(const int key, const int i, const int mod, hash_fn hash)
{
    return i * hash(key, mod);
}

static bool is_prime(const int num)
{
    if (num == 0 || num == 1)
    {
        return 0;
    }
    for (int i = 2; i * i <= num; ++i)
    {
        if (num % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

static int next_prime(const int num)
{
    int n = num + 1;
    while (!is_prime(n))
    {
        n++;
    }
    return n;
}

htable_node_t *ht_node_create(const int key)
{
    htable_node_t *nnode = malloc(sizeof(htable_node_t));
    if (!nnode)
    {
        return NULL;
    }
    nnode->key = key;
    nnode->next = NULL;
    return nnode;
}

void node_list_delete(htable_node_t *head)
{
    if (!head)
    {
        return;
    }
    htable_node_t *nxt = head->next;
    for (; head; head = nxt)
    {
        nxt = head->next;
        free(head);
    }
}

error ht_chain_init(hash_table_chain *ht_chain, int size, double max_load_factor, hash_fn hash_func)
{
    int table_size = next_prime(size);
    srand(time(NULL));
    ht_chain->elems = malloc(sizeof(htable_node_t *) * table_size);
    if (!ht_chain->elems)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < table_size; ++i)
    {
        ht_chain->elems[i] = NULL;
    }
    ht_chain->hash_func = hash_func;
    ht_chain->max_load_factor = max_load_factor;
    ht_chain->count = 0;
    ht_chain->size = table_size;
    return 0;
}

error ht_chain_insert(hash_table_chain *ht_chain, int key)
{
    int hash_ind = ht_chain->hash_func(key, ht_chain->size);
    htable_node_t *nnode = ht_node_create(key);
    if (!nnode)
    {
        return ALLOC_ERROR;
    }
    htable_node_t *head = ht_chain->elems[hash_ind];
    for (; head; head = head->next)
    {
        if (head->key == key)
        {
            return DUPLICATE_KEY;
        }
    }
    nnode->next = ht_chain->elems[hash_ind];
    ht_chain->elems[hash_ind] = nnode;
    ht_chain->count++;
    error rc = 0;
    if ((double)ht_chain->count / (double)ht_chain->size > ht_chain->max_load_factor)
    {
        rc = ht_chain_rehash(ht_chain);
    }
    return rc;
}

error ht_chain_rehash(hash_table_chain *ht_chain)
{
    int nsize = next_prime(ht_chain->size);
    htable_node_t **buf = calloc(nsize, sizeof(htable_node_t *));
    if (!buf)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < ht_chain->size; ++i)
    {
        if (!ht_chain->elems[i])
        {
            continue;
        }
        htable_node_t *head = ht_chain->elems[i];
        htable_node_t *next = head->next;
        for (; head; head = next)
        {
            next = head->next;
            int n_hash_ind = ht_chain->hash_func(head->key, nsize);
            head->next = buf[n_hash_ind];
            buf[n_hash_ind] = head;
        }
    }
    free(ht_chain->elems);
    ht_chain->elems = buf;
    ht_chain->size = nsize;
    return 0;
}

error ht_chain_remove(hash_table_chain *ht_chain, int key)
{
    int hash_ind = ht_chain->hash_func(key, ht_chain->size);
    htable_node_t *head = ht_chain->elems[hash_ind];
    htable_node_t *prev = NULL;
    error rc = KEY_NOT_FOUND;
    for (; head; head = head->next)
    {
        if (head->key == key)
        {
            rc = 0;
            break;
        }
        prev = head;
    }
    if (rc == KEY_NOT_FOUND)
    {
        return rc;
    }
    if (!prev)
    {
        ht_chain->elems[hash_ind] = head->next;
    }
    else
    {
        prev->next = head->next;
    }
    free(head);
    ht_chain->count--;
    return rc;
}

error ht_chain_find(hash_table_chain *ht_chain, int key, int *cmp_count)
{
    int hash_ind = ht_chain->hash_func(key, ht_chain->size);
    htable_node_t *head = ht_chain->elems[hash_ind];
    for (; head; head = head->next)
    {
        (*cmp_count)++;
        if (head->key == key)
        {
            return 0;
        }
    }
    return KEY_NOT_FOUND;
}

void ht_chain_print(hash_table_chain *ht_chain)
{
    printf("Хэш-таблица:\n");
    printf("Выделенный размер: %d\n", ht_chain->size);
    for (int i = 0; i < ht_chain->size; ++i)
    {
        if (!ht_chain->elems[i])
        {
            continue;
        }
        htable_node_t *head = ht_chain->elems[i];
        printf("h = %d: ", ht_chain->hash_func(ht_chain->elems[i]->key, ht_chain->size));
        for (; head; head = head->next)
        {
            if (head->next)
            {
                printf("%d->", head->key);
            }
            else
            {
                printf("%d\n", head->key);
            }
        }
    }
}

void ht_chain_delete(hash_table_chain *ht_chain)
{
    for (int i = 0; i < ht_chain->size; ++i)
    {
        node_list_delete(ht_chain->elems[i]);
    }
    free(ht_chain->elems);
    ht_chain->size = 0;
    ht_chain->count = 0;
}

error ht_open_init(hash_table_open *ht_open, int size, double max_load_factor,
                   hash_fn main_func, hash_fn hash_func2, probe_fn probe_func)
{
    int table_size = next_prime(size);
    srand(time(NULL));
    htable_elem_t *elems = malloc(sizeof(htable_elem_t) * table_size);
    if (!elems)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < table_size; ++i)
    {
        elems[i].is_occupied = 0;
        elems[i].key = 0;
    }
    ht_open->elems = elems;
    ht_open->count = 0;
    ht_open->size = table_size;
    ht_open->main_func = main_func;
    ht_open->hash_func2 = hash_func2;
    ht_open->probe_func = probe_func;
    ht_open->max_load_factor = max_load_factor;
    ht_open->max_probe_length = (int)log2(table_size);
    return 0;
}

static int find_insert_place_ht_open(hash_table_open *ht_open, int key)
{
    int hash_ind = ht_open->main_func(key, ht_open->size);
    int try = 1;
    int current_ind = hash_ind;
    while (ht_open->elems[current_ind].is_occupied && try < ht_open->max_probe_length)
    {
        if (ht_open->elems[current_ind].key == key)
        {
            return current_ind;
        }
        int probe_offset = ht_open->probe_func(key, try, ht_open->size, ht_open->hash_func2);
        current_ind = (hash_ind + probe_offset) % ht_open->size;
        if (current_ind < 0)
        {
            current_ind += ht_open->size;
        }
        try++;
    }
    if (ht_open->elems[current_ind].is_occupied)
    {
        return -1;
    }
    return current_ind;
}

error ht_open_insert(hash_table_open *ht_open, int key)
{
    int current_ind = find_insert_place_ht_open(ht_open, key);
    error rc = 0;
    if (current_ind == -1)
    {
        rc = ht_open_rehash(ht_open);
        if (rc)
        {
            return rc;
        }
        current_ind = find_insert_place_ht_open(ht_open, key);
    }
    else if (ht_open->elems[current_ind].is_occupied && ht_open->elems[current_ind].key == key)
    {
        return DUPLICATE_KEY;
    }
    ht_open->elems[current_ind].key = key;
    ht_open->elems[current_ind].is_occupied = true;
    ht_open->count++;
    if ((double)ht_open->count / (double)ht_open->size > ht_open->max_load_factor){
        rc = ht_open_rehash(ht_open);
    }
    return rc;
}

error ht_open_remove(hash_table_open *ht_open, int key)
{
    int hash_ind = ht_open->main_func(key, ht_open->size);
    int try = 1;
    int current_ind = hash_ind;
    while (try < ht_open->size)
    {
        if (ht_open->elems[current_ind].key == key && ht_open->elems[current_ind].is_occupied)
        {
            ht_open->elems[current_ind].is_occupied = false;
            ht_open->count--;
            return 0;
        }
        int probe_offset = ht_open->probe_func(key, try, ht_open->size, ht_open->hash_func2);
        current_ind = (hash_ind + probe_offset) % ht_open->size;
        if (current_ind < 0)
        {
            current_ind += ht_open->size;
        }
        try++;
    }
    return KEY_NOT_FOUND;
}

error ht_open_find(hash_table_open *ht_open, int key, int *cmp_count)
{
    int hash_ind = ht_open->main_func(key, ht_open->size);
    int try = 1;
    int current_ind = hash_ind;
    while (try < ht_open->size)
    {
        if (ht_open->elems[current_ind].key == key && ht_open->elems[current_ind].is_occupied)
        {
            *cmp_count = try;
            return 0;
        }
        int probe_offset = ht_open->probe_func(key, try, ht_open->size, ht_open->hash_func2);
        current_ind = (hash_ind + probe_offset) % ht_open->size;
        if (current_ind < 0)
        {
            current_ind += ht_open->size;
        }
        try++;
    }
    *cmp_count = try;
    return KEY_NOT_FOUND;
}

error ht_open_rehash(hash_table_open *ht_open)
{
    int nsize = next_prime(ht_open->size);
    htable_elem_t *buf = calloc(nsize, sizeof(htable_elem_t));
    if (!buf)
    {
        return ALLOC_ERROR;
    }
    htable_elem_t *old_elems = ht_open->elems;
    int old_size = ht_open->size;

    ht_open->elems = buf;
    ht_open->size = nsize;
    ht_open->max_probe_length++;
    ht_open->count = 0;
    for (int i = 0; i < old_size; ++i)
    {
        if (old_elems[i].is_occupied)
        {
            error rc = ht_open_insert(ht_open, old_elems[i].key);
            if (rc)
            {
                free(buf);
                ht_open->elems = old_elems;
                ht_open->size = old_size;
                ht_open->count = 0;
                for (int j = 0; j < old_size; ++j)
                {
                    if (old_elems[j].is_occupied)
                    {
                        ht_open->count++;
                    }
                }
                return rc;
            }
        }
    }
    free(old_elems);
    return 0;
}

void ht_open_print(hash_table_open *ht_open)
{
    printf("Хэш-таблица в формате [ключ; значение хэш-функции]\n");
    printf("Выделенный размер: %d\n", ht_open->size);
    for (int i = 0; i < ht_open->size; ++i)
    {
        if (ht_open->elems[i].is_occupied)
        {
            printf("[%d; %d] ", ht_open->elems[i].key,
                   ht_open->main_func(ht_open->elems[i].key, ht_open->size));
        }
    }
    printf("\n");
}

void ht_open_delete(hash_table_open *ht_open)
{
    free(ht_open->elems);
    ht_open->count = 0;
    ht_open->size = 0;
}

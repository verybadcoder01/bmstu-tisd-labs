#ifndef HASH_TABLE_H__
#define HASH_TABLE_H__

#include "errors.h"
#include "stdbool.h"
#include "stddef.h"

#define MAX_PROBE_LENGTH 3
#define MAX_OPEN_LOAD_FACTOR 0.5
#define MAX_CHAIN_LOAD_FACTOR 1.2

typedef struct htable_node htable_node_t;

typedef int (*hash_fn)(const int, const int);

typedef int (*probe_fn)(const int, const int, const int, hash_fn);

struct htable_node
{
    int key;
    htable_node_t *next;
};

htable_node_t *ht_node_create(const int key);

void node_list_delete(htable_node_t *head);

typedef struct
{
    int key;
    bool is_occupied;
} htable_elem_t;

// внутреннее (закрытое) хеширование (открытая адресация)
typedef struct
{
    htable_elem_t *elems;
    int size, count;
    double max_load_factor;
    int max_probe_length;
    hash_fn main_func, hash_func2;
    probe_fn probe_func;
} hash_table_open;

// внешнее (открытое) хеширование (метод цепочек)
typedef struct
{
    htable_node_t **elems;
    int size, count;
    double max_load_factor;
    hash_fn hash_func;
} hash_table_chain;

// вспомогательные функции
int hash_knuth_xor(const int key, const int mod);

int hash_mult(const int key, const int m);

int second_hash_func(const int key, const int mod);

int second_hash_probe(const int key, const int i, const int mod, hash_fn hash);

// внешнее (открытое) хеширование (метод цепочек)
error ht_chain_init(hash_table_chain *ht_chain, int size, double max_load_factor, hash_fn hash_func);

error ht_chain_insert(hash_table_chain *ht_chain, int key);

error ht_chain_remove(hash_table_chain *ht_chain, int key);

error ht_chain_find(hash_table_chain *ht_chain, int key, int *cmp_count);

error ht_chain_rehash(hash_table_chain *ht_chain);

void ht_chain_print(hash_table_chain *ht_chain);

void ht_chain_delete(hash_table_chain *ht_chain);

// внутреннее (закрытое) хеширование (открытая адресация)
error ht_open_init(hash_table_open *ht_open, int size, double max_load_factor, hash_fn hash_func1, hash_fn hash_func2, probe_fn probe_func);

error ht_open_insert(hash_table_open *ht_open, int key);

error ht_open_remove(hash_table_open *ht_open, int key);

error ht_open_find(hash_table_open *ht_open, int key, int *cmp_count);

error ht_open_rehash(hash_table_open *ht_open);

void ht_open_print(hash_table_open *ht_open);

void ht_open_delete(hash_table_open *ht_open);

#endif
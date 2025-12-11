#ifndef UTILS_H__
#define UTILS_H__

#include "node.h"
#include "stdio.h"
#include "errors.h"
#include "hash_table.h"
#include "node.h"

typedef enum {
    AVL_TREE = 1,
    HASH_TABLE_OPEN,
    HASH_TABLE_CHAIN
} struct_type;

void print_menu_struct();

typedef enum {
    T_QUIT = 0,
    T_BUILD,
    T_ADD,
    T_FIND,
    T_REMOVE,
    T_WALK,
    T_GRAPHVIZ,
} tree_action;

void print_menu_tree();

typedef enum {
    H_QUIT = 0,
    H_BUILD,
    H_ADD,
    H_FIND,
    H_REMOVE,
    H_PRINT,
} table_action;

void print_menu_table();

void scan_newlines(FILE *f);

int count_nums_in_file(FILE *f);

error add_nums_from_file_to_tree(node_t **root, FILE *f);

error add_nums_from_file_to_ht_open(hash_table_open *ht, FILE *f);

error add_nums_from_file_to_ht_chain(hash_table_chain *ht, FILE *f);

error read_and_open_file(FILE **f, const char *mode);

#endif
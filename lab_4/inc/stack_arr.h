#ifndef STACK_ARR_H__
#define STACK_ARR_H__

#ifdef PERF_TEST
#define MAX_ARR_STACK_SIZE 10000
#else
#define MAX_ARR_STACK_SIZE 100
#endif

#include "stddef.h"
#include "errors.h"

typedef struct
{
    char* data[MAX_ARR_STACK_SIZE];
    size_t size;
} stack_arr;

error push_stack_arr(stack_arr *st, char* n_elem);

error pop_stack_arr(stack_arr *st, char **data);

void print_st_arr(stack_arr *st);

void print_rev_st_arr(stack_arr *st);

void delete_stack_arr(stack_arr *st);

#endif
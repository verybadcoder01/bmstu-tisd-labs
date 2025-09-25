#ifndef SORT_H__
#define SORT_H__

#include "stddef.h"
#include "errors.h"
#include "time.h"

typedef int (*compare_func_ptr_t)(const void *, const void *);
typedef error (*sort_func_ptr_t)(void *, size_t, size_t, compare_func_ptr_t);

typedef enum
{
    BUBBLE = 1,
    MERGE = 2
} sort_type;
#define valid_sort_type(x) (x == BUBBLE || x == MERGE)

void swap(void *elem_l, void *elem_r, size_t size);

error bubble_sort(void *ptr, size_t count, size_t size, compare_func_ptr_t comp);

void merge_sorted_arrays(void *ptr_first, void *ptr_second, void *result, size_t count_first, size_t count_second, size_t elem_size, compare_func_ptr_t comp);

void merge_sort_impl(void *source, void *dest, size_t start, size_t end, size_t elem_size, compare_func_ptr_t comp);

error merge_sort(void *ptr, size_t count, size_t size, compare_func_ptr_t comp);

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end);

error measure_sort_time(unsigned long *res, sort_func_ptr_t sort, void *ptr, size_t count, size_t size, compare_func_ptr_t comp);

#endif
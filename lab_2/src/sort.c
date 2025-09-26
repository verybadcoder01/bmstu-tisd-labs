#include "sort.h"
#include "stdbool.h"
#include "memory.h"
#include "malloc.h"

void swap(void *elem_l, void *elem_r, size_t size)
{
    char buf[size];
    memcpy(buf, elem_l, size);
    memcpy(elem_l, elem_r, size);
    memcpy(elem_r, buf, size);
}

error bubble_sort(void *ptr, size_t count, size_t size, compare_func_ptr_t comp)
{
    if (count == 0){
        return 0;
    }
    char *start = (char *)ptr;
    for (size_t i = 0; i < count - 1; ++i)
    {
        bool was_swap = false;
        for (size_t j = 0; j < count - i - 1; ++j)
        {
            char *jth = (start + j * size), *next = (start + (j + 1) * size);
            if (comp(next, jth) < 0)
            {
                swap(jth, next, size);
                was_swap = true;
            }
        }
        if (!was_swap)
        {
            break;
        }
    }
    return 0;
}

void merge_sorted_arrays(void *ptr_first, void *ptr_second, void *result, size_t count_first, size_t count_second, size_t elem_size, compare_func_ptr_t comp)
{
    size_t ind_first = 0, ind_second = 0, ind_res = 0;
    char *st_first = (char *)ptr_first;
    char *st_second = (char *)ptr_second;
    char *res_start = (char *)result;
    while (ind_first < count_first && ind_second < count_second)
    {
        char *first_el = st_first + ind_first * elem_size;
        char *second_el = st_second + ind_second * elem_size;
        char *res_elem = res_start + ind_res * elem_size;
        if (comp(first_el, second_el) < 0)
        {
            memcpy(res_elem, first_el, elem_size);
            ind_first++;
        }
        else
        {
            memcpy(res_elem, second_el, elem_size);
            ind_second++;
        }
        ind_res++;
    }
    while (ind_first < count_first)
    {
        char *first_el = st_first + ind_first * elem_size;
        char *res_elem = res_start + ind_res * elem_size;
        memcpy(res_elem, first_el, elem_size);
        ind_first++;
        ind_res++;
    }
    while (ind_second < count_second)
    {
        char *second_el = st_second + ind_second * elem_size;
        char *res_elem = res_start + ind_res * elem_size;
        memcpy(res_elem, second_el, elem_size);
        ind_second++;
        ind_res++;
    }
}

void merge_sort_impl(void *source, void *dest, size_t start, size_t end, size_t elem_size, compare_func_ptr_t comp)
{
    if (end - start <= 1)
    {
        return;
    }
    size_t mid = start + (end - start) / 2;
    merge_sort_impl(dest, source, start, mid, elem_size, comp);
    merge_sort_impl(dest, source, mid, end, elem_size, comp);
    char *src = (char *)source;
    char *dst = (char *)dest;
    char *left = src + start * elem_size, *right = src + mid * elem_size;
    char *result = dst + start * elem_size;
    size_t left_count = mid - start, right_count = end - mid;
    merge_sorted_arrays(left, right, result, left_count, right_count, elem_size, comp);
}

error merge_sort(void *ptr, size_t count, size_t size, compare_func_ptr_t comp)
{
    if (count <= 1)
    {
        return 0;
    }
    void *buf = malloc(count * size);
    if (!buf)
    {
        return ALLOC_ERROR;
    }
    memcpy(buf, ptr, count * size);
    merge_sort_impl(buf, ptr, 0, count, size, comp);
    free(buf);
    return 0;
}

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec * 1000000 + end->tv_nsec / 1000) -
           (start->tv_sec * 1000000 + start->tv_nsec / 1000);
}

error measure_sort_time(unsigned long *res, sort_func_ptr_t sort, void *ptr, size_t count, size_t size, compare_func_ptr_t comp)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    error rc = sort(ptr, count, size, comp);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    if (rc){
        return rc;
    }
    *res = elapsed_time(&start, &end);
    return 0;
}

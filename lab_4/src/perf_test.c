#include "perf_test.h"
#include "stack_arr.h"
#include "stack_list.h"
#include "time.h"
#include "stdio.h"

#define PERF_TEST_RUNS_PUSH_POP 10000
#define PERF_TEST_RUNS_PROCESS 1000

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec * 1000000 + end->tv_nsec / 1000) -
           (start->tv_sec * 1000000 + start->tv_nsec / 1000);
}

void walk_stack_arr(stack_arr *st)
{
    for (size_t i = 0; i < st->size; ++i)
    {
        char *a = st->data[i];
        (void)a;
    }
}

void walk_stack_list(stack_list *st)
{
    for (node *cur = st->head; cur; cur = cur->next)
    {
        char *a = cur->data;
        (void)a;
    }
}

error measure_walk_stack_arr(stack_arr *st, int elems, unsigned long *res)
{
    for (int j = 0; j < elems; ++j)
    {
        error rc = push_stack_arr(st, "abcd");
        if (rc)
        {
            return rc;
        }
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int j = 0; j < PERF_TEST_RUNS_PROCESS; ++j)
    {
        walk_stack_arr(st);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    unsigned long time_walk_stack_arr = elapsed_time(&start, &end);
    *res = time_walk_stack_arr;
    for (int j = 0; j < elems; ++j)
    {
        error rc = pop_stack_arr(st);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
}

error measure_walk_stack_list(stack_list *st, int elems, unsigned long *res)
{
    for (int j = 0; j < elems; ++j)
    {
        error rc = stack_list_push(st, "abcd");
        if (rc)
        {
            return rc;
        }
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int j = 0; j < PERF_TEST_RUNS_PROCESS; ++j)
    {
        walk_stack_list(st);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    unsigned long time_walk_stack_list = elapsed_time(&start, &end);
    *res = time_walk_stack_list;
    for (int j = 0; j < elems; ++j)
    {
        error rc = stack_list_pop(st);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
}

error run_perf_test()
{
#ifdef DEBUG_PRINT
    return PERF_TEST_ERROR;
#endif
    stack_arr st_arr;
    st_arr.size = 0;
    stack_list st_list;
    st_list.head = NULL;
    st_list.size = 0;
    st_list.freed_cap = 0;
    st_list.freed_count = 0;
    st_list.freed_ptrs = NULL;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < PERF_TEST_RUNS_PUSH_POP; ++i)
    {
        error rc = push_stack_arr(&st_arr, "abcd");
        if (rc)
        {
            return rc;
        }
        rc = pop_stack_arr(&st_arr);
        if (rc)
        {
            return rc;
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    unsigned long time_push_pop_arr = elapsed_time(&start, &end);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (int i = 0; i < PERF_TEST_RUNS_PUSH_POP; ++i)
    {
        error rc = stack_list_push(&st_list, "abcd");
        if (rc)
        {
            return rc;
        }
        rc = stack_list_pop(&st_list);
        if (rc)
        {
            return rc;
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    unsigned long time_push_pop_list = elapsed_time(&start, &end);
    printf("Среднее время работы пары операций push-pop на массиве: %.6lf\n", (double)time_push_pop_arr / (double)PERF_TEST_RUNS_PUSH_POP);
    printf("Среднее время работы пары операций push-pop на списке: %.6lf\n", (double)time_push_pop_list / (double)PERF_TEST_RUNS_PUSH_POP);
    int min_size = MAX_ARR_STACK_SIZE;
    if (min_size > MAX_STACK_LIST_SIZE)
    {
        min_size = MAX_STACK_LIST_SIZE;
    }
    unsigned long time_walk_stack_arr = 0;
    error rc = measure_walk_stack_arr(&st_arr, min_size, &time_walk_stack_arr);
    if (rc){
        return rc;
    }
    printf("Среднее время обхода стека на массиве: %.6lf\n", (double)time_walk_stack_arr / (double)PERF_TEST_RUNS_PROCESS);
    unsigned long time_walk_stack_list = 0;
    rc = measure_walk_stack_list(&st_list, min_size, &time_walk_stack_list);
    printf("Среднее время обхода стека на списке: %.6lf\n", (double)time_walk_stack_list / (double)PERF_TEST_RUNS_PROCESS);
    return 0;
}
#ifndef PERF_TEST_H__
#define PERF_TEST_H__

#include "errors.h"
#include "time.h"
#include "stack_arr.h"
#include "stack_list.h"

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end);

void walk_stack_arr(stack_arr *st);

void walk_stack_list(stack_list *st);

error run_perf_test();

#endif
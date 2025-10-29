#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    INPUT_ERROR,
    STACK_OVERFLOW,
    EMPTY_STACK,
    WRONG_STACK_TYPE,
    WRONG_MENU_ITEM,
    PERF_TEST_ERROR
} error;

#endif
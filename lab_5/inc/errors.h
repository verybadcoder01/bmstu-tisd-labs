#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    INPUT_ERROR,
    WRONG_MODE,
    WRONG_QUEUE_TYPE,
    WRONG_TIME,
    QUEUE_LIST_OVERFLOW,
    QUEUE_ARR_OVERFLOW,
    EMPTY_QUEUE_DEL
} error;

typedef enum {
    USER = 1,
    PERF
} mode;

typedef enum {
    LIST = 1,
    ARRAY = 2
} queue_type;

#endif
#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    EMPTY_VEC = 2,
    INPUT_ERROR = 3,
    WRONG_VEC_COORD = 4,
    EMPTY_MAT = 5,
    WRONG_MAT_ROW = 6,
    WRONG_MAT_COL = 7,
    INCOMPATIBLE_SIZES = 8,
    WRONG_MAT_ELEM_CNT = 9,
    WRONG_VEC_ELEM_CNT = 10,
    PERF_TEST_ERROR = 11,
    DUPLICATE_COORD = 12
} error;

#endif
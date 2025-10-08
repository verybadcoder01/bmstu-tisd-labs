#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    EMPTY_VEC = 2,
    INPUT_ERROR = 3,
    WRONG_VEC_COORD = 4,
    EMPTY_MAT = 5,
    WRONG_MAT_ROW = 6,
    WRONG_MAT_COL = 7
} error;

#endif
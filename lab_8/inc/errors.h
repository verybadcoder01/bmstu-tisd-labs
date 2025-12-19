#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    INPUT_ERROR,
    WRONG_VERT_NUMBER,
    EULER_NO_PATH,
    EULER_DISCONNECTED,
    WRONG_DIM,
} error;

#endif
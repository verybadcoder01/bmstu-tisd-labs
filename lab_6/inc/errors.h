#ifndef ERRORS_H__
#define ERRORS_H__

typedef enum {
    ALLOC_ERROR = 1,
    DUPLICATE_KEY,
    KEY_NOT_FOUND,
    WRONG_INPUT,
    UNKNOWN_ACTION,
    WRONG_GPA_INPUT,
    WRONG_OUT_FILE,
} error;

void interface_printf(const char *format, ...);

#endif
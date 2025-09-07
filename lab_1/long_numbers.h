#ifndef LONG_NUMBERS_H__
#define LONG_NUMBERS_H__

#include "constants.h"
#include "stddef.h"

typedef struct {
    int digits[MAX_INTEGER_LENGTH + 1];
    int dig_count;
    int sign;
} big_integer;

typedef struct {
    int mantis_before_point[MAX_MANTISS_LENGTH];
    int mantis_after_point[MAX_MANTISS_LENGTH];
    int before_point_count, after_point_count;
    int exponent;
    int mantis_sign;
} big_decimal;

error str_to_int(const char *str, int *result);

error input_big_decimal(big_decimal *result);

error input_big_integer(big_integer *result);

void print_big_decimal(big_decimal *data);

void init_big_decimal_zero(big_decimal *num);

int is_zero_decimal(const big_decimal *num);

int is_zero_integer(const big_integer *num);

void add_digit_to_end(int *num, int size, int digit);

void normalize(big_decimal *num);

void leading_zeros_to_exponent(big_decimal *num);

void round_big_decimal(big_decimal *num, int new_digit);

void shift_right(big_decimal *num, int places);

void shift_left(big_decimal *num, int places);

void reverse_array(int *arr, int size);

void subtract_big_integers(const big_integer *a, const big_integer *b, big_integer *result);

void mult_big_integer_digit(const big_integer *a, int digit, big_integer *result);

int compare_integer_decimal(const big_integer *a, const big_decimal *b);

int compare_integer_integer(const big_integer *a, const big_integer *b);

#endif

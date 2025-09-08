#ifndef LONG_NUMBERS_IO_H__
#define LONG_NUMBERS_IO_H__

#include "constants.h"
#include "long_numbers.h"

error sign(const char *str, int *res);

error str_to_int(const char *str, int *result);

error input_big_decimal(big_decimal *result);

error input_big_integer(big_integer *result);

void print_big_decimal(big_decimal *data);


#endif

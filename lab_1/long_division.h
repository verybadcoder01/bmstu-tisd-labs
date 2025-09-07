#ifndef LONG_DIVISION_H__
#define LONG_DIVISION_H__

#include "stddef.h"
#include "long_numbers.h"

error divide_by_big_decimal(const big_integer *a, const big_decimal *b, big_decimal *result);

#endif

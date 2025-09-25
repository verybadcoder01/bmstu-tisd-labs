#ifndef IO_H__
#define IO_H__

#include "errors.h"
#include "stdio.h"
#include "string.h"

error scan(FILE *f, char **result, size_t *res_size);

#endif
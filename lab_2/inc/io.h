#ifndef IO_H__
#define IO_H__

#include "errors.h"
#include "stdio.h"
#include "string.h"

void interface_printf(const char *msg);

void print_err_table();

void interface_print_error(error rc);

error scan(FILE *f, char **result, size_t *res_size);

#endif
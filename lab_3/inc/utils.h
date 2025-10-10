#ifndef UTILS_H__
#define UTILS_H__

#include "errors.h"

void interface_printf(const char *msg);

void interface_printf_err(error rc);

void print_err_table();

#endif
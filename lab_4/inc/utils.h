#ifndef UTILS_H__
#define UTILS_H__

#include "errors.h"

void interface_printf(const char *msg);

void interface_printf_ptr(const void* ptr);

void interface_printf_err(error rc);

void interface_printf_menu();

void clear_stdin();

#endif
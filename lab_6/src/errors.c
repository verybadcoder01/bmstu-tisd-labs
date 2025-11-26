#include "errors.h"
#include "stdio.h"
#include "stdarg.h"

void interface_printf(const char *format, ...) {
    #ifdef DEBUG_PRINT
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    #else
    (void)format;
    #endif
}

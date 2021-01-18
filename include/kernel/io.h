#ifndef IO_H
#define IO_H

#include <kernel/tty.h>
#include <stdarg.h>

int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);

#endif

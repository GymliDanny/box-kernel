#ifndef IO_H
#define IO_H

#include <stdarg.h>

int vkprintf(const char *fmt, va_list args);
int kprintf(const char *fmt, ...);

#endif

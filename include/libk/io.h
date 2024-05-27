#ifndef LIBK_IO_H
#define LIBK_IO_H

#include <stdarg.h>

char* convert(unsigned int num, int base);
int vkprintf(const char *fmt, va_list args);
int kprintf(const char *fmt, ...);

#endif

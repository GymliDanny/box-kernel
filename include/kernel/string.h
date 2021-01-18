#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int memcmp(const void *str1, const void *str2, size_t n);
void* memcpy(void* __restrict dest, const void* __restrict src, size_t n);
void* memmove(void* __restrict dest, const void* __restrict src, size_t n);
void* memset(void *str, int c, size_t n);
size_t strlen(const char *str);

#endif

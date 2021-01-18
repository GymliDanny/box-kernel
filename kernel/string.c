#include <kernel/string.h>

int memcmp(const void *str1, const void *str2, size_t n) {
        return 0;
}

void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
        return NULL;
}

void* memmove(void* __restrict dest, const void* __restrict src, size_t n) {
        return NULL;
}

void* memset(void *str, int c, size_t n) {
        return NULL;
}

size_t strlen(const char *str) {
        size_t i = 0;
        while (str[i] != '\0')
                i++;
        return i;
}

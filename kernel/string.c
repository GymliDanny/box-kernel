#include <kernel/string.h>

int memcmp(const void *str1, const void *str2, size_t n) {
        unsigned char const *p1 = str1;
        unsigned char const *p2 = str2;
        int ret = 0;

        if (str1 == str2)
                return 0;

        while (n > 0) {
                if (*p1 != *p2) {
                        ret = (*p1 > *p2)?1:-1;
                        break;
                }
                n--;
                p1++;
                p2++;
        }
        return ret;
}

void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
        if (dest == src)
                return dest;

        unsigned char *pdest = dest;
        unsigned char const *psrc = src;
        for (size_t i = 0; i < n; i++)
                pdest[i] = psrc[i];
        return dest;
}

void* memmove(void* __restrict dest, const void* __restrict src, size_t n) {
        if (dest == src)
                return dest;

        unsigned char const *psrc = src;
        unsigned char buffer[n];

        for (size_t i = 0; i < n; i++)
                buffer[i] = psrc[i];
        return memcpy(dest, (void*)buffer, n);
}

void* memset(void *str, int c, size_t n) {
        unsigned char *p = str;
        for (size_t i = 0; i < n; i++)
                p[i] = (unsigned char)c;
        return str;
}

size_t strlen(const char *str) {
        size_t i = 0;
        while (str[i] != '\0')
                i++;
        return i;
}

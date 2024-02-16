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

char* strcpy(char* __restrict dest, const char* __restrict src) {
        if (strlen(src) > strlen(dest))
                return NULL;
        return (char*)memcpy(dest, src, strlen(src));
}

char* strcat(char* __restrict dest, const char* __restrict src) {
        return (char*)memcpy(&dest[strlen(dest)], src, strlen(src));
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

int strncmp(const char *str1, const char *str2, size_t n) {
        return memcmp(str1, str2, n);
}

int strcmp(const char *str1, const char *str2) {
        size_t str1_sz = strlen(str1);
        size_t str2_sz = strlen(str2);
        if (str1_sz > str2_sz)
                return memcmp(str1, str2, str2_sz);
        return memcmp(str1, str2, str1_sz);
}

size_t strlen(const char *str) {
        size_t i = 0;
        while (str[i] != '\0')
                i++;
        return i;
}

int _is_delim(char c, const char *delim) {
        while (*delim != '\0') {
                if (c == *delim)
                        return 1;
                delim++;
        }
        return 0;
}

char* strtok(char* __restrict str, const char* __restrict delim) {
        static char *old_str;

        if (str == NULL)
                str = old_str;

        while (1) {
                if (_is_delim(*str, delim)) {
                        str++;
                        continue;
                }

                if (*str == '\0')
                        return NULL;
                break;
        }

        char *ret = str;
        while (1) {
                if (*str == '\0') {
                        old_str = str;
                        return ret;
                }

                if (_is_delim(*str, delim)) {
                        *str = '\0';
                        old_str = str + 1;
                        return ret;
                }
                str++;
        }
}

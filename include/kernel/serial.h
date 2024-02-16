#ifndef KERNEL_SERIAL_H
#define KERNEL_SERIAL_H

#include <stdint.h>
#include <stddef.h>

int serial_init(void);
void serial_putchar(char c);
char serial_getchar(void);

static inline void serial_write(const char *data, size_t size) {
        for (size_t i = 0; i < size; i++)
                serial_putchar(data[i]);
}

static inline void serial_read(char *data, size_t size) {
        for (size_t i = 0; i < size; i++)
                data[i] = serial_getchar();
}

#endif

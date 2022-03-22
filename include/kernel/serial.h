#ifndef SERIAL_H
#define SERIAL_H

#include <kernel/string.h>

int serial_init(void);
char read_serial(void);
void write_serial(char a);

inline void serial_writestring(const char *str) {
        for (size_t i = 0; i < strlen(str); i++)
                write_serial(str[i]);
}

#endif

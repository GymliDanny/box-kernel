#include <kernel/serial.h>
#include <kernel/string.h>
#include <stddef.h>
#include "pic.h"

#define PORT 0x3f8

int serial_init(void) {
        outb(PORT + 1, 0x00);
        outb(PORT + 3, 0x80);
        outb(PORT + 0, 0x03);
        outb(PORT + 1, 0x00);
        outb(PORT + 3, 0x03);
        outb(PORT + 2, 0xC7);
        outb(PORT + 4, 0x0B);
        outb(PORT + 4, 0x1E);
        outb(PORT + 0, 0xAE);

        if (inb(PORT + 0) != 0xAE)
                return -1;

        outb(PORT + 4, 0x0F);
        return 0;
}

int serial_recieved(void) {
        return inb(PORT + 5) & 1;
}

char read_serial(void) {
        while (serial_recieved() == 0);
        return inb(PORT);
}

int is_transmit_empty(void) {
        return inb(PORT + 5) & 0x20;
}

void write_serial(char a) {
        while (is_transmit_empty() == 0);
        outb(PORT, a);
}

void serial_writestring(const char *str) {
        for (size_t i = 0; i < strlen(str); i++)
                write_serial(str[i]);
}

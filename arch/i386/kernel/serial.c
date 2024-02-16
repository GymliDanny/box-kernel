#include <kernel/serial.h>
#include <kernel/pic.h>
#include <kernel/string.h>
#include <stddef.h>

#define PORT 0x3f8

static inline int _serial_received(void) {
        return inb(PORT + 5) & 1;
}

static inline int _is_transmit_empty(void) {
        return inb(PORT + 5) & 0x20;
}

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

void serial_putchar(char c) {
        while (_is_transmit_empty() == 0);
        outb(PORT, c);
}

char serial_getchar(void) {
        while (_serial_received() == 0);
        return inb(PORT);
}

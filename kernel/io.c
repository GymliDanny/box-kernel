#include <kernel/io.h>
#include <kernel/string.h>
#include <kernel/tty.h>

char* convert(unsigned int num, int base) {
        static char rep[] = "0123456789ABCDEF";
        static char buffer[50];
        char *ptr;

        ptr = &buffer[49];
        *ptr = '\0';

        do {
                *--ptr = rep[num%base];
                num /= base;
        } while (num != 0);

        return ptr;
}

int vkprintf(const char *fmt, va_list args) {
        char *s;
        int i;

        for (size_t n = 0; n < strlen(fmt); n++) {
                if (fmt[n] != '%') {
                        tty_putchar(fmt[n]);
                        continue;
                } else {
                        n++;
                }

                switch (fmt[n]) {
                        case 'c':
                                i = va_arg(args, int);
                                tty_putchar(i);
                                break;
                        case 'd':
                                i = va_arg(args, int);
                                if (i < 0) {
                                        i = -i;
                                        tty_putchar('-');
                                }
                                tty_writestring(convert(i, 10));
                                break;
                        case 'o':
                                i = va_arg(args, unsigned int);
                                tty_writestring(convert(i, 8));
                                break;
                        case 's':
                                s = va_arg(args, char*);
                                tty_writestring(s);
                                break;
                        case 'x':
                                i = va_arg(args, unsigned int);
                                tty_writestring(convert(i, 16));
                                break;
                }
        }
        return 0;
}

int kprintf(const char *fmt, ...) {
        va_list args;
        int done;

        va_start(args, fmt);
        done = vkprintf(fmt, args);
        va_end(args);

        return done;
}

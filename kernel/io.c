#include <kernel/io.h>
#include <kernel/framebuffer.h>
#include <kernel/data/ringbuf.h>
#include <kernel/string.h>

static int ringbuf_init = 0;
static struct ringbuf rb;

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
        if (ringbuf_init == 0) {
                rb_init(&rb, 1024, 4096);
                ringbuf_init = 1;
        }

        char *s;
        int i;

        char buffer[4096];
        for (size_t n = 0; n < strlen(fmt); n++) {
                if (fmt[n] != '%') {
                        buffer[strlen(buffer)] = fmt[n];
                        continue;
                } else {
                        n++;
                }

                switch (fmt[n]) {
                        case 'c':
                                i = va_arg(args, int);
                                buffer[strlen(buffer)] = i;
                                break;
                        case 's':
                                s = va_arg(args, char*);
                                strcat(buffer, s);
                                break;
                        case 'd':
                                i = va_arg(args, int);
                                if (i < 0) {
                                        i = -i;
                                        strcat(buffer, "-");
                                }
                                strcat(buffer, convert(i, 10));
                                break;
                        case 'o':
                                i = va_arg(args, unsigned int);
                                strcat(buffer, convert(i, 10));
                                break;
                        case 'x':
                                i = va_arg(args, unsigned int);
                                strcat(buffer, convert(i, 16));
                                break;
                }
        }
        rb_push_back(&rb, buffer, strlen(buffer));
        memset(buffer, 0, 4096);
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

#ifndef I386_FRAMEBUFFER_H
#define I386_FRAMEBUFFER_H

#include <stddef.h>
#include <stdint.h>

void fb_init(void);
void fb_setcolor(uint8_t color);
void fb_putchar(char c);
void fb_setpos(int x, int y);

static inline void fb_write(const char *data, size_t size) {
        for (size_t i = 0; i < size; i++)
                fb_putchar(data[i]);
}

#endif

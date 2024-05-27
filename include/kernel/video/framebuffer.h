#ifndef KERNEL_FRAMEBUFFER_H
#define KERNEL_FRAMEBUFFER_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH       80
#define VGA_HEIGHT      25

static uint16_t *const VGA_MEMORY = (uint16_t*)0xC03FF000;

void fb_init(void);
void fb_setcolor(uint8_t color);
void fb_putchar(char c);
void fb_setpos(int x, int y);
void fb_offsetpos(int dx, int dy);

static inline void fb_write(const char *data, size_t size) {
        for (size_t i = 0; i < size; i++)
                fb_putchar(data[i]);
}

#endif

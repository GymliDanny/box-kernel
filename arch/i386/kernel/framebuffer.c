#include <kernel/vga.h>
#include <kernel/paging.h>
#include <kernel/pic.h>
#include <kernel/string.h>
#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t *const VGA_MEMORY = (uint16_t*)0xC03FF000;

static size_t fb_row;
static size_t fb_column;
static uint8_t fb_color;
static uint16_t *framebuffer;

void _enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
        outb(0x3D4, 0x0A);
        outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

        outb(0x3D4, 0x0B);
        outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void _update_cursor(size_t x, size_t y) {
        uint16_t pos = y * VGA_WIDTH + x;
        
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t)(pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void _fb_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
        const size_t index = y * VGA_WIDTH + x;
        framebuffer[index] = vga_entry(c, color);
}

void _fb_scroll(void) {
        for (size_t i = 0; i < VGA_HEIGHT-1; i++) {
                for (size_t j = 0; j < VGA_WIDTH; j++)
                        framebuffer[i * VGA_WIDTH + j] = VGA_MEMORY[(i+1) * VGA_WIDTH + j];
        }
}

void fb_init(void) {
        map_page(NULL, 0xB8000, (uintptr_t)VGA_MEMORY, 0x003);

        fb_row = 0;
        fb_column = 0;
        fb_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        framebuffer = VGA_MEMORY;
        for (size_t y = 0; y < VGA_HEIGHT; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++)
                        _fb_putentryat(' ', fb_color, x, y);
        }
        _enable_cursor(0, 0);
        _update_cursor(0, 0);
}

void fb_setcolor(uint8_t color) {
        fb_color = color;
}

void fb_setpos(int x, int y) {
        fb_row = y;
        fb_column = x;
}

void fb_putchar(char c) {
        unsigned char uc = c;
        switch (uc) {
                case '\r':
                        fb_column = 0;
                        break;
                case '\n':
                        fb_column = 0;
                        if (++fb_row == VGA_HEIGHT) {
                                fb_row--;
                                _fb_scroll();
                        }
                        break;
                case '\t':
                        fb_column += 4;
                        if (++fb_column == VGA_WIDTH) {
                                fb_column = 4;
                                if (++fb_row == VGA_HEIGHT) {
                                        fb_row--;
                                        _fb_scroll();
                                }
                        }
                        break;
                default:
                        _fb_putentryat(uc, fb_color, fb_column, fb_row);
                        if (++fb_column == VGA_WIDTH) {
                                fb_column = 0;
                                if (++fb_row == VGA_HEIGHT) {
                                        fb_row--;
                                        _fb_scroll();
                                }
                        }
                        break;
        }
        _update_cursor(fb_column, fb_row+1);
}

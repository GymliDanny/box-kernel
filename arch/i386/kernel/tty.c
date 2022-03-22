#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/string.h>
#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t *const VGA_MEMORY = (uint16_t*)0xC03FF000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void tty_init(void) {
        terminal_row = 0;
        terminal_column = 0;
        terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        terminal_buffer = VGA_MEMORY;
        for (size_t y = 0; y < VGA_HEIGHT; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                        const size_t index = y * VGA_WIDTH + x;
                        terminal_buffer[index] = vga_entry(' ', terminal_color);
                }
        }
}

void tty_setcolor(uint8_t color) {
        terminal_color = color;
}

void tty_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
        const size_t index = y * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(void) {
        for (size_t i = 0; i < VGA_HEIGHT; i++) {
                for (size_t j = 0; j < VGA_WIDTH; j++)
                        terminal_buffer[i * VGA_WIDTH + j] = terminal_buffer[(i+1) * VGA_WIDTH + j];
        }
}

void tty_putchar(char c) {
        unsigned char uc;

        uc = c;
        switch (uc) {
                case '\n':
                        terminal_column = 0;
                        if (++terminal_row == VGA_HEIGHT) {
                                terminal_row--;
                                terminal_scroll();
                        }
                        break;
                case '\t':
                        terminal_column += 4;
                        if (++terminal_column == VGA_WIDTH) {
                                terminal_column = 4;
                                if (++terminal_row == VGA_HEIGHT) {
                                        terminal_row--;
                                        terminal_scroll();
                                }
                        }
                        break;
                default:
                        tty_putentryat(uc, terminal_color, terminal_column, terminal_row);
                        if (++terminal_column == VGA_WIDTH) {
                                terminal_column = 0;
                                if (++terminal_row == VGA_HEIGHT) {
                                        terminal_row--;
                                        terminal_scroll();
                                }
                        }
                        break;
        }
}

void tty_write(const char *data, size_t size) {
        for (size_t i = 0; i < size; i++)
                tty_putchar(data[i]);
}

void tty_writestring(const char *data) {
        tty_write(data, strlen(data));
}
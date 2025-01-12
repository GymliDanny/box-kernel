#include <kernel/input/keyboard.h>
#include <kernel/tty/tty_vga.h>
#include <kernel/tty.h>
#include <kernel/kthread.h>
#include <kernel/pic.h>
#include <libk/string.h>

static uint8_t keymap_modifiers[256] = {
        [0x1D] KB_CTL,
        [0x2A] KB_SHIFT,
        [0x36] KB_SHIFT,
        [0x38] KB_ALT,
        [0x9D] KB_CTL,
        [0xB8] KB_ALT
};

static uint8_t keymap_toggles[256] = {
        [0x3A] KB_CPSLK,
        [0x45] KB_NUMLK,
        [0x46] KB_SCLLK
};

static uint8_t keymap_normal[256] = {
        0, 0x1B, '1', '2', '3', '4', '5', '6',
        '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
        'o', 'p', '[', ']', '\n', 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
        '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', 0, '*',
        0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7',
        '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0, 0, 0, 0,
        [0x9C] '\n',
        [0xB5] '/',
        [0xC8] KB_UP,
        [0xD0] KB_DOWN,
        [0xC9] KB_PGUP,
        [0xD1] KB_PGDN,
        [0xCB] KB_LEFT,
        [0xCD] KB_RGHT,
        [0x97] KB_HOME,
        [0xCF] KB_END,
        [0xD2] KB_INS,
        [0xD3] KB_DEL
};

static uint8_t keymap_shifted[256] = {
        0, 0x33, '!', '@', '#', '$', '%', '^',
        '&', '*', '(', ')', '_', '+', '\b', '\t',
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}', '\n', 0, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
        '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', 0, '*',
        0, ' ', 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, '7',
        '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0, 0, 0, 0,
        [0x9C] '\n',
        [0xB5] '/',
        [0xC8] KB_UP,
        [0xD0] KB_DOWN,
        [0xC9] KB_PGUP,
        [0xD1] KB_PGDN,
        [0xCB] KB_LEFT,
        [0xCD] KB_RGHT,
        [0x97] KB_HOME,
        [0xCF] KB_END,
        [0xD2] KB_INS,
        [0xD3] KB_DEL
};

static uint8_t keymap_control[256] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        C('Q'), C('W'), C('E'), C('R'), C('T'), C('Y'), C('U'), C('I'),
        C('O'), C('P'), 0, 0, '\r', 0, C('A'), C('S'),
        C('D'), C('F'), C('G'), C('H'), C('J'), C('K'), C('L'), 0,
        0, 0, 0, C('\\'), C('Z'), C('X'), C('C'), C('V'),
        C('B'), C('N'), C('M'), 0, 0, C('/'), 0, 0,
        [0x9C] '\r',
        [0xB5] C('/'),
        [0xC8] KB_UP,
        [0xD0] KB_DOWN,
        [0xC9] KB_PGUP,
        [0xD1] KB_PGDN,
        [0xCB] KB_LEFT,
        [0xCD] KB_RGHT,
        [0x97] KB_HOME,
        [0xCF] KB_END,
        [0xD2] KB_INS,
        [0xD3] KB_DEL
};

static uint8_t* keymaps[4] = {
        keymap_normal,
        keymap_shifted,
        keymap_control,
        keymap_control,
};

char keyboard_getchar(void) {
        static int shift = 0;
        uint8_t st = inb(KB_STAT);
        if ((st & KB_DIB) == 0)
                return -1;

        uint8_t data = inb(KB_DATA);

        if (data == 0xE0) {
                shift |= KB_E0ESC;
                return -1;
        } else if (data & 0x80) {
                if (!(shift & KB_E0ESC))
                        data &= 0x7F;
                shift &= ~(keymap_modifiers[data] | KB_E0ESC);
                return -1;
        } else if (shift & KB_E0ESC) {
                data |= 0x80;
                shift &= ~KB_E0ESC;
        }

        shift |= keymap_modifiers[data];
        shift ^= keymap_toggles[data];
        char c = keymaps[shift & (KB_CTL | KB_SHIFT)][data];
        if (shift & KB_CPSLK) {
                if ('a' <= c && c <= 'z')
                        c += 'A' - 'a';
                else if ('A' <= c && c <= 'Z')
                        c += 'a' - 'A';
        }
        return c;
}

void keyboard_handler(struct isr_frame *frame) {
        char c = keyboard_getchar();
        if (c != -1)
                tty_getchar(c);
        return;
}

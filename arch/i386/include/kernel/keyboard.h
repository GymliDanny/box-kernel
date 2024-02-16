#ifndef I386_KEYBOARD_H
#define I386_KEYBOARD_H

#include <kernel/isr.h>
#include <stdint.h>

#define KB_STAT         0x64
#define KB_DATA         0x60

#define KB_DIB          0x01
#define KB_SHIFT        (1<<0)
#define KB_CTL          (1<<1)
#define KB_ALT          (1<<2)
#define KB_CPSLK        (1<<3)
#define KB_NUMLK        (1<<4)
#define KB_SCLLK        (1<<5)
#define KB_E0ESC        (1<<6)

#define KB_HOME         0xE0
#define KB_END          0xE1
#define KB_UP           0xE2
#define KB_DOWN         0xE3
#define KB_LEFT         0xE4
#define KB_RGHT         0xE5
#define KB_PGUP         0xE6
#define KB_PGDN         0xE7
#define KB_INS          0xE8
#define KB_DEL          0xE9

char keyboard_getchar(void);
void keyboard_handler(struct isr_frame *frame);

#endif

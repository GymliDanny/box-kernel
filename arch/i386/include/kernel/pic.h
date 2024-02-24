#ifndef I386_PIC_H
#define I386_PIC_H

#include <kernel/asm.h>
#include <stdint.h>

#define PIC1            0x20
#define PIC2            0x28
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define ICW1_ICW4       0x01
#define ICW1_SINGLE     0x02
#define ICW1_INTERVAL4  0x04
#define ICW1_LEVEL      0x08
#define ICW1_INIT       0x10

#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10

#define PIC_READ_IRR    0x0A
#define PIC_READ_ISR    0x0B

static inline void io_wait(void) {
        outb(0x80, 0);
}

void pic_eoi(uint8_t irq);
void pic_remap(void);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

void register_irq_handler(uint8_t irq, void (*handler)(struct regs *regs));
void irq_dispatch(struct regs *regs);

void irq_set_mask(uint8_t irq);
void irq_clear_mask(uint8_t irq);

#endif

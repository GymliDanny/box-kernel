#include <kernel/pic.h>

void pic_eoi(unsigned char irq) {
        if (irq >= 8)
                outb(PIC2_COMMAND, 0x20);
        outb(PIC1_COMMAND, 0x20);
}

void pic_remap(void) {
        unsigned char a1 = inb(PIC1_DATA);
        unsigned char a2 = inb(PIC2_DATA);

        outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
        outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
        outb(PIC1_DATA, PIC1);
        outb(PIC2_DATA, PIC2);
        outb(PIC1_DATA, 4);
        outb(PIC2_DATA, 2);
        outb(PIC1_DATA, ICW4_8086);
        outb(PIC2_DATA, ICW4_8086);
        outb(PIC1_DATA, a1);
        outb(PIC2_DATA, a2);
}

static uint16_t __pic_get_irq_reg(int ocw3) {
        outb(PIC1_COMMAND, ocw3);
        outb(PIC2_COMMAND, ocw3);
        return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_irr(void) {
        return __pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(void) {
        return __pic_get_irq_reg(PIC_READ_ISR);
}

void irq_set_mask(uint8_t irq) {
        uint16_t port;
        uint8_t data;

        if (irq < 8) {
                port = PIC1_DATA;
        } else {
                port = PIC2_DATA;
                irq -= 8;
        }

        data = inb(port) | (1 << irq);
        outb(port, data);
}

void irq_clear_mask(unsigned char irq) {
        uint16_t port;
        uint8_t data;

        if (irq < 8) {
                port = PIC1_DATA;
        } else {
                port = PIC2_DATA;
                irq -= 8;
        }

        data = inb(port) & ~(1 << irq);
        outb(port, data);
}

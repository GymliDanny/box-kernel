#include <kernel/io.h>
#include <stdint.h>

struct idt_entry {
        uint16_t        isr_low;
        uint16_t        kernel_cs;
        uint8_t         reserved;
        uint8_t         attributes;
        uint16_t        isr_high;
} __attribute__((packed));

struct idt_ptr {
        uint16_t        limit;
        uint32_t        base;
} __attribute__((packed));

__attribute__((aligned(0x10)))
static struct idt_entry idt[256];
static struct idt_ptr idtr;

__attribute__((noreturn))
void exception_handler(int in) {
        kprintf("EXCEPTION %d CAUGHT\n", in);
}

extern void* isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
        struct idt_entry *desc = &idt[vector];

        desc->isr_low           = (uint32_t)isr & 0xFFFF;
        desc->kernel_cs         = 0x08;
        desc->attributes        = flags;
        desc->isr_high          = (uint32_t)isr >> 16;
        desc->reserved          = 0;
}

void idt_install(void) {
        idtr.base = (uintptr_t)&idt[0];
        idtr.limit = (uint16_t)sizeof(struct idt_entry) * 256 - 1;

        for (uint8_t vector = 0; vector < 32; vector++) {
                idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
                isr_stub_table[vector] = 1;
        }

        __asm__ volatile("lidt %0" : : "memory"(idtr));
        __asm__ volatile("sti");
        kprintf("Interrupts on\n");
}

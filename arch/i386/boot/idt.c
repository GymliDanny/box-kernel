#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/io.h>
#include <kernel/pic.h>
#include <kernel/string.h>
#include <stdint.h>

__attribute__((aligned(0x10)))
struct idt_entry idt[256];
struct idt_ptr idtr;

__attribute__((noreturn))
void exception_handler(int number) {
        kprintf("EXCEPTION %d CAUGHT\n", number);
        __asm__ volatile("cli;hlt");
}

extern void idt_set_gate(uint8_t num, void (*handler)(void), uint16_t cs, uint8_t flags) {
        idt[num].isr_low = (uint16_t)(((uint32_t)handler >> 0) & 0xFFFF);
        idt[num].isr_high = (uint16_t)(((uint32_t)handler >> 16) & 0xFFFF);
        idt[num].reserved = 0;
        idt[num].kernel_cs = cs;
        idt[num].flags = flags;
}

void idt_install(void) {
        idtr.limit = (uint16_t)sizeof(struct idt_entry) * 256 - 1;
        idtr.base = (uint32_t)&idt[0];

        memset(&idt, 0, sizeof(struct idt_entry) * 256);
        idt_set_gate(0, isr_stub_0, 0x08, 0x8e);
        idt_set_gate(1, isr_stub_1, 0x08, 0x8e);
        idt_set_gate(2, isr_stub_2, 0x08, 0x8e);
        idt_set_gate(3, isr_stub_3, 0x08, 0x8e);
        idt_set_gate(4, isr_stub_4, 0x08, 0x8e);
        idt_set_gate(5, isr_stub_5, 0x08, 0x8e);
        idt_set_gate(6, isr_stub_6, 0x08, 0x8e);
        idt_set_gate(7, isr_stub_7, 0x08, 0x8e);
        idt_set_gate(8, isr_stub_8, 0x08, 0x8e);
        idt_set_gate(9, isr_stub_9, 0x08, 0x8e);
        idt_set_gate(10, isr_stub_10, 0x08, 0x8e);
        idt_set_gate(11, isr_stub_11, 0x08, 0x8e);
        idt_set_gate(12, isr_stub_12, 0x08, 0x8e);
        idt_set_gate(13, isr_stub_13, 0x08, 0x8e);
        idt_set_gate(14, isr_stub_14, 0x08, 0x8e);
        idt_set_gate(15, isr_stub_15, 0x08, 0x8e);
        idt_set_gate(16, isr_stub_16, 0x08, 0x8e);
        idt_set_gate(17, isr_stub_17, 0x08, 0x8e);
        idt_set_gate(18, isr_stub_18, 0x08, 0x8e);
        idt_set_gate(19, isr_stub_19, 0x08, 0x8e);
        idt_set_gate(20, isr_stub_20, 0x08, 0x8e);
        idt_set_gate(21, isr_stub_21, 0x08, 0x8e);
        idt_set_gate(22, isr_stub_22, 0x08, 0x8e);
        idt_set_gate(23, isr_stub_23, 0x08, 0x8e);
        idt_set_gate(24, isr_stub_24, 0x08, 0x8e);
        idt_set_gate(25, isr_stub_25, 0x08, 0x8e);
        idt_set_gate(26, isr_stub_26, 0x08, 0x8e);
        idt_set_gate(27, isr_stub_27, 0x08, 0x8e);
        idt_set_gate(28, isr_stub_28, 0x08, 0x8e);
        idt_set_gate(29, isr_stub_29, 0x08, 0x8e);
        idt_set_gate(30, isr_stub_30, 0x08, 0x8e);
        idt_set_gate(31, isr_stub_31, 0x08, 0x8e);

        __asm__ volatile("lidt %0" : : "memory"(idtr));
        __asm__ volatile("sti");
}

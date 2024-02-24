#include <kernel/idt.h>
#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <kernel/gdt.h>
#include <kernel/asm.h>
#include <kernel/io.h>
#include <kernel/pic.h>
#include <kernel/string.h>
#include <kernel/paging.h>
#include <stdint.h>

__attribute__((aligned(0x10)))
struct idt_entry idt[256];
struct idt_ptr idtr;

const char* exceptions[] = {
        "Division by zero",
        "Debug",
        "Non-maskable interrupt",
        "Breakpoint",
        "Overflow",
        "Out-of-bounds",
        "Invalid opcode",
        "FPU not available",
        "Double fault",
        "RESERVED",
        "Invalid TSS",
        "Segment not present",
        "Stack fault",
        "General protection fault",
        "Page fault",
        "RESERVED",
        "FPU exception",
        "Alignment check",
        "Machine check",
        "FPU-SIMD exception",
        "Virtualization exception",
        "Control protection"
        "RESERVED",
        "Hypervisor injection",
        "VMM exception",
        "Security exception",
        "RESERVED",
        "Triple fault",
        "RESERVED"
};

void exception_handler(struct regs *regs) {
        switch (regs->isr_vector) {
                case 0x00:
                        panic("Division by zero in kernel address space");
                        break;
                case 0x06:
                        panic("Invalid opcode in kernel address space");
                        break;
                case 0x08:
                        panic("Double fault in interrupt handler");
                        break;
                case 0x0D:
                        panic("Protection fault in kernel address space");
                        break;
                case 0x0E:
                        page_fault_handler(regs);
                        break;
                default:
                        panic("Unhandled exception");
        }
}

void interrupt_handler(struct regs regs) {
        if (regs.isr_vector < 32) {
                exception_handler(&regs);
        } else if (regs.isr_vector < 48) {
                irq_dispatch(&regs);
        } else {
                switch (regs.isr_vector) {
                        case 0x80:
                                handle_syscall(&regs);
                                break;
                        default:
                                panic("Unmapped interrupt");
                }
        }
}

void idt_set_gate(uint8_t num, void (*handler)(void), uint16_t cs, uint8_t flags) {
        struct idt_entry *desc = &idt[num];
        desc->isr_low = (uint16_t)(((uint32_t)handler >> 0) & 0xFFFF);
        desc->isr_high = (uint16_t)(((uint32_t)handler >> 16) & 0xFFFF);
        desc->reserved = 0;
        desc->kernel_cs = cs;
        desc->flags = flags;
}

void idt_install(void) {
        idtr.limit = (uint16_t)sizeof(struct idt_entry) * 256 - 1;
        idtr.base = (uint32_t)idt;

        idt_set_gate(0x0, isr_stub_0, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1, isr_stub_1, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x2, isr_stub_2, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x3, isr_stub_3, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x4, isr_stub_4, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x5, isr_stub_5, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x6, isr_stub_6, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x7, isr_stub_7, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x8, isr_stub_8, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x9, isr_stub_9, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xA, isr_stub_10, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xB, isr_stub_11, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xC, isr_stub_12, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xD, isr_stub_13, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xE, isr_stub_14, 0x08, IDT_EXCEPTION);
        idt_set_gate(0xF, isr_stub_15, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x10, isr_stub_16, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x11, isr_stub_17, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x12, isr_stub_18, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x13, isr_stub_19, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x14, isr_stub_20, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x15, isr_stub_21, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x16, isr_stub_22, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x17, isr_stub_23, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x18, isr_stub_24, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x19, isr_stub_25, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1A, isr_stub_26, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1B, isr_stub_27, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1C, isr_stub_28, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1D, isr_stub_29, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1E, isr_stub_30, 0x08, IDT_EXCEPTION);
        idt_set_gate(0x1F, isr_stub_31, 0x08, IDT_EXCEPTION);

        idt_set_gate(0x20, irq_stub_0, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x21, irq_stub_1, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x22, irq_stub_2, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x23, irq_stub_3, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x24, irq_stub_4, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x25, irq_stub_5, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x26, irq_stub_6, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x27, irq_stub_7, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x28, irq_stub_8, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x29, irq_stub_9, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2A, irq_stub_10, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2B, irq_stub_11, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2C, irq_stub_12, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2D, irq_stub_13, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2E, irq_stub_14, 0x08, IDT_INTERRUPT);
        idt_set_gate(0x2F, irq_stub_15, 0x08, IDT_INTERRUPT);

        idt_set_gate(0x80, syscall_stub, 0x08, IDT_INTERRUPT);

        __asm__ volatile("lidt %0" : : "memory"(idtr));
}

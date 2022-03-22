#include <kernel/syscall.h>
#include <kernel/isr.h>
#include <kernel/io.h>
#include <kernel/pic.h>
#include <kernel/string.h>
#include <stdint.h>

#define SEGMENT_PRESENT         0x80
#define SEGMENT_RING0           0x00
#define SEGMENT_RING3           0x60
#define SEGMENT_STORAGE         0x00
#define SEGMENT_INTERRUPT       0x0E
#define IDT_EXCEPTION           (SEGMENT_PRESENT | SEGMENT_INTERRUPT)
#define IDT_INTERRUPT           (SEGMENT_PRESENT | SEGMENT_INTERRUPT)

#define IDT_MAX_DESCRIPTORS     256

struct idt_entry {
        uint16_t        isr_low;
        uint16_t        kernel_cs;
        uint8_t         reserved;
        uint8_t         flags;
        uint16_t        isr_high;
} __attribute__((packed));

struct idt_ptr {
        uint16_t        limit;
        uint32_t        base;
} __attribute__((packed));

__attribute__((aligned(0x10)))
struct idt_entry idt[256];
struct idt_ptr idtr;

char *exceptions[] = {
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

__attribute__((noreturn))
void halt_catch_fire(struct isr_frame *frame) {
        dump_reg(frame);
        __asm__ volatile("cli;hlt");
}

__attribute__((noreturn))
void exception_handler(struct isr_frame *frame) {
        switch (frame->vector) {
                case 0x0E:
                        kprintf("PAGE FAULT\n");
                        halt_catch_fire(frame);
                        break;
                default:
                        kprintf("Unhandled exception: %s\n", exceptions[frame->vector]);
                        halt_catch_fire(frame);
        }
}

void irq_dispatch(uint8_t irq, struct isr_frame *frame) {
        return;
}

__attribute__((noreturn))
void interrupt_handler(struct isr_frame *frame) {
        if (frame->vector < 32)
                exception_handler(frame);
        else if (frame->vector > 32 && frame->vector < 48)
                irq_dispatch(frame->vector - 16, frame);
        switch (frame->vector) {
                case 0x80:
                        syscall_dispatch(frame);
                        break;
                default:
                        kprintf("Error: Unmapped interrupt: %d\n", frame->vector);
                        halt_catch_fire(frame);
                        __asm__ volatile("cli;hlt");
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

        idt_set_gate(0, isr_stub_0, 0x08, IDT_EXCEPTION);
        idt_set_gate(1, isr_stub_1, 0x08, IDT_EXCEPTION);
        idt_set_gate(2, isr_stub_2, 0x08, IDT_EXCEPTION);
        idt_set_gate(3, isr_stub_3, 0x08, IDT_EXCEPTION);
        idt_set_gate(4, isr_stub_4, 0x08, IDT_EXCEPTION);
        idt_set_gate(5, isr_stub_5, 0x08, IDT_EXCEPTION);
        idt_set_gate(6, isr_stub_6, 0x08, IDT_EXCEPTION);
        idt_set_gate(7, isr_stub_7, 0x08, IDT_EXCEPTION);
        idt_set_gate(8, isr_stub_8, 0x08, IDT_EXCEPTION);
        idt_set_gate(9, isr_stub_9, 0x08, IDT_EXCEPTION);
        idt_set_gate(10, isr_stub_10, 0x08, IDT_EXCEPTION);
        idt_set_gate(11, isr_stub_11, 0x08, IDT_EXCEPTION);
        idt_set_gate(12, isr_stub_12, 0x08, IDT_EXCEPTION);
        idt_set_gate(13, isr_stub_13, 0x08, IDT_EXCEPTION);
        idt_set_gate(14, isr_stub_14, 0x08, IDT_EXCEPTION);
        idt_set_gate(15, isr_stub_15, 0x08, IDT_EXCEPTION);
        idt_set_gate(16, isr_stub_16, 0x08, IDT_EXCEPTION);
        idt_set_gate(17, isr_stub_17, 0x08, IDT_EXCEPTION);
        idt_set_gate(18, isr_stub_18, 0x08, IDT_EXCEPTION);
        idt_set_gate(19, isr_stub_19, 0x08, IDT_EXCEPTION);
        idt_set_gate(20, isr_stub_20, 0x08, IDT_EXCEPTION);
        idt_set_gate(21, isr_stub_21, 0x08, IDT_EXCEPTION);
        idt_set_gate(22, isr_stub_22, 0x08, IDT_EXCEPTION);
        idt_set_gate(23, isr_stub_23, 0x08, IDT_EXCEPTION);
        idt_set_gate(24, isr_stub_24, 0x08, IDT_EXCEPTION);
        idt_set_gate(25, isr_stub_25, 0x08, IDT_EXCEPTION);
        idt_set_gate(26, isr_stub_26, 0x08, IDT_EXCEPTION);
        idt_set_gate(27, isr_stub_27, 0x08, IDT_EXCEPTION);
        idt_set_gate(28, isr_stub_28, 0x08, IDT_EXCEPTION);
        idt_set_gate(29, isr_stub_29, 0x08, IDT_EXCEPTION);
        idt_set_gate(30, isr_stub_30, 0x08, IDT_EXCEPTION);
        idt_set_gate(31, isr_stub_31, 0x08, IDT_EXCEPTION);

        idt_set_gate(128, syscall_stub, 0x08, IDT_INTERRUPT);

        register_syscall(sys_stop, 0);
        register_syscall(sys_status, 1);

        __asm__ volatile("lidt %0" : : "memory"(idtr));
        __asm__ volatile("sti");
}

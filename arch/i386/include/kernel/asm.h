#ifndef I386_ASM_H
#define I386_ASM_H

#include <kernel/gdt.h>
#include <stdint.h>

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define COM_PORT        0x3F8

#define MAX_ISR         256

struct regs {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;
        uint32_t esp;

        uint32_t cr0;
        uint32_t cr2;
        uint32_t cr3;
        uint32_t cr4;
};

struct isr_frame {
        uint32_t cr4;
        uint32_t cr3;
        uint32_t cr2;
        uint32_t cr0;

        uint32_t edi;
        uint32_t esi;
        uint32_t edx;
        uint32_t ecx;
        uint32_t ebx;
        uint32_t eax;

        uint32_t isr_vector;
        uint32_t isr_err;
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
} __attribute__((packed));

void isr_stub_0(void);
void isr_stub_1(void);
void isr_stub_2(void);
void isr_stub_3(void);
void isr_stub_4(void);
void isr_stub_5(void);
void isr_stub_6(void);
void isr_stub_7(void);
void isr_stub_8(void);
void isr_stub_9(void);
void isr_stub_10(void);
void isr_stub_11(void);
void isr_stub_12(void);
void isr_stub_13(void);
void isr_stub_14(void);
void isr_stub_15(void);
void isr_stub_16(void);
void isr_stub_17(void);
void isr_stub_18(void);
void isr_stub_19(void);
void isr_stub_20(void);
void isr_stub_21(void);
void isr_stub_22(void);
void isr_stub_23(void);
void isr_stub_24(void);
void isr_stub_25(void);
void isr_stub_26(void);
void isr_stub_27(void);
void isr_stub_28(void);
void isr_stub_29(void);
void isr_stub_30(void);
void isr_stub_31(void);

void irq_stub_0(void);
void irq_stub_1(void);
void irq_stub_2(void);
void irq_stub_3(void);
void irq_stub_4(void);
void irq_stub_5(void);
void irq_stub_6(void);
void irq_stub_7(void);
void irq_stub_8(void);
void irq_stub_9(void);
void irq_stub_10(void);
void irq_stub_11(void);
void irq_stub_12(void);
void irq_stub_13(void);
void irq_stub_14(void);
void irq_stub_15(void);

void isr_stub_128(void);

void aquire_lock(int *lock);
void release_lock(int *lock);

void enable_paging(uint32_t new_cr3);
void flush_gdt(void);

static inline void outb(uint16_t port, uint8_t value) {
        __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t value) {
        __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t value) {
        __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
}

static inline uint16_t inw(uint16_t port) {
        uint16_t ret;
        __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
}

static inline uint32_t inl(uint16_t port) {
        uint32_t ret;
        __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
}

static inline void enable_ints(void) {
        __asm__ volatile("sti");
}

static inline void disable_ints(void) {
        __asm__ volatile("cli");
}

static inline void flush_tss(void) {
        __asm__ volatile("movw $0x28, %ax; ltr %ax");
}

static inline void flush_tlb(void) {
        __asm__ volatile("movl %cr3, %eax; movl %eax, %cr3");
}

static inline void invlpg(void *addr) {
        __asm__ volatile("invlpg (%0)" : : "b"(addr) : "memory");
}

#endif

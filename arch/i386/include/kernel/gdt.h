#ifndef I386_GDT_H
#define I386_GDT_H

#include <stdint.h>

struct gdt_entry {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t gran;
        uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
        uint16_t limit;
        uint32_t base;
} __attribute__((packed));

struct tss_entry {
        uint32_t link;
        uint32_t esp0;
        uint32_t ss0;
        uint32_t esp1;
        uint32_t ss2;
        uint32_t cr3;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
        uint32_t es;
        uint32_t cs;
        uint32_t ss;
        uint32_t ds;
        uint32_t fs;
        uint32_t gs;
        uint32_t ldtr;
        uint16_t trap;
        uint16_t iomap_base;
} __attribute__((packed));

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void write_tss(int num, uint32_t ss0, uint32_t esp0);
void set_kernel_esp(uint32_t esp);
void gdt_install(void);

void flush_gdt(void);

#endif

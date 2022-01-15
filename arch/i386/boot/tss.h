#ifndef I386_TSS_H
#define I386_TSS_H

#include <stdint.h>

struct tss {
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
        uint32_t io_offset;
}__attribute__((packed));

void flush_tss(void);

uint64_t create_gdt_entry(uint32_t base, uint32_t limit, uint16_t flag);
uint64_t create_tss_entry(uint16_t ss0, uint32_t esp0);
void set_kernel_stack(uint32_t stack);

#endif

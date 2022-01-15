#include <kernel/string.h>
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

struct gdt_entry desc[6];
struct gdt_ptr gp;
struct tss_entry tss_entry;

extern void flush_gdt(void);
extern void flush_tss(void);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
        desc[num].base_low = (base & 0xFFFF);
        desc[num].base_middle = (base >> 16) & 0xFF;
        desc[num].base_high = (base >> 24) & 0xFF;

        desc[num].limit_low = (limit & 0xFFFF);
        desc[num].gran = (limit >> 16) & 0x0F;
        desc[num].gran |= (gran & 0xF0);
        desc[num].access = access;
}

void write_tss(int num, uint16_t ss0, uint16_t esp0) {
        uint32_t base = (uint32_t)&tss_entry;
        uint32_t limit = base + sizeof(tss_entry);

        gdt_set_gate(num, base, limit, 0xE9, 0x00);
        memset(&tss_entry, 0x0, sizeof(tss_entry));
        
        tss_entry.ss0 = ss0;
        tss_entry.esp0 = esp0;
        tss_entry.cs = 0x0B;
        tss_entry.ss = 0x13;
        tss_entry.ds = 0x13;
        tss_entry.es = 0x13;
        tss_entry.fs = 0x13;
        tss_entry.gs = 0x13;
        tss_entry.iomap_base = sizeof(tss_entry);
}

void gdt_install(void) {
        gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
        gp.base = (uint32_t)&desc;

        gdt_set_gate(0, 0, 0, 0, 0);
        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
        gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
        gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
        write_tss(5, 0x10, 0x0);

        flush_gdt();
        flush_tss();
}

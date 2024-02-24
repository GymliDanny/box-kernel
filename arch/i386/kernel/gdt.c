#include <kernel/gdt.h>
#include <kernel/asm.h>
#include <kernel/string.h>

struct gdt_entry desc[6];
struct gdt_ptr gp;
struct tss_entry tss_entry;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
        desc[num].base_low = (base & 0xFFFF);
        desc[num].base_middle = (base >> 16) & 0xFF;
        desc[num].base_high = (base >> 24) & 0xFF;

        desc[num].limit_low = (limit & 0xFFFF);
        desc[num].gran = (limit >> 16) & 0x0F;
        desc[num].gran |= (gran & 0xF0);
        desc[num].access = access;
}

void write_tss(int num, uint32_t ss0, uint32_t esp0) {
        uint32_t base = (uint32_t)&tss_entry;
        uint32_t limit = base + sizeof(struct tss_entry);

        gdt_set_gate(num, base, limit, 0x89, 0x00);
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

void set_kernel_esp(uint32_t esp) {
        tss_entry.esp0 = esp;
}

void gdt_install(void) {
        gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
        gp.base = (uint32_t)&desc;

        gdt_set_gate(0, 0, 0, 0, 0);
        gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);
        gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);
        gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xCF);
        gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xCF);
        
        uint32_t esp;
        __asm__ volatile("movl %%esp, %0" : "=r"(esp));
        write_tss(5, 0x10, esp);

        flush_gdt();
        flush_tss();
}

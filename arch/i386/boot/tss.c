#include "tss.h"
#include <kernel/string.h>

struct tss tss_entry;

uint64_t create_tss_entry(uint16_t ss0, uint32_t esp0) {
        uint32_t base = (uint32_t)&tss_entry;
        uint32_t limit = base + sizeof(&tss_entry);
        uint64_t ret = create_gdt_entry(base, limit, 0xE9);

        memset(&tss_entry, 0, sizeof(struct tss));
        tss_entry.ss0 = ss0;
        tss_entry.esp0 = esp0;
        tss_entry.cs = 0x08;
        tss_entry.ss = 0x13;
        tss_entry.ds = 0x13;
        tss_entry.es = 0x13;
        tss_entry.fs = 0x13;
        tss_entry.gs = 0x13;
        tss_entry.io_offset = sizeof(struct tss) & 0xFFFF;
        return ret;
}

void set_kernel_stack(uint32_t stack) {
        tss_entry.esp0 = stack;
}

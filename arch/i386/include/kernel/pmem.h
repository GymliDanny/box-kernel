#ifndef I386_PMEM_H
#define I386_PMEM_H

#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>

struct pfa_page {
        struct pfa_page *next;
};

void pfa_init(struct mboot_info *header);

void pfa_free_dma(uintptr_t paddr, size_t num_pages);
void pfa_free(uintptr_t paddr, size_t num_pages);

#endif

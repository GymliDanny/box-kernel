#ifndef I386_PMEM_H
#define I386_PMEM_H

#include <stdint.h>
#include <stddef.h>

struct pfa_page {
        struct pfa_page *next;
};

void pfa_init(struct mboot_info *header);

uintptr_t pfa_alloc_dma(size_t num_pages);
uintptr_t pfa_alloc(size_t num_pages);

void pfa_free_dma(uintptr_t paddr, size_t num_pages);
void pfa_free(uintptr_t paddr, size_t num_pages);

#endif

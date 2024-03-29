#ifndef I386_PMEM_H
#define I386_PMEM_H

#include <kernel/multiboot.h>
#include <stdint.h>

#define PFA_BLOCK_FREE  1
#define PFA_BLOCK_ALLOC 3

#define PFA_ALLOC_ERR   0xFFFFFFFF

struct pfa_page {
        struct pfa_page *next;
};

struct pfa_zone {
        uintptr_t start;
        uintptr_t size;
        struct pfa_page *freelist;
};

int pfa_init(struct mboot_info *header);

uintptr_t pfa_alloc(void);
void pfa_free(struct pfa_zone *zone, uintptr_t paddr);
void pfa_free_range(struct pfa_zone *zone, uintptr_t pstart, uintptr_t pend);

#endif

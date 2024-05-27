#ifndef I386_PMEM_H
#define I386_PMEM_H

#include <kernel/multiboot.h>
#include <stdint.h>

#define PFA_ALLOC_ERR   0xFFFFFFFF

struct pfa_page {
        struct pfa_page *next;
};

void pfa_init(struct mboot_info *header);

uintptr_t pfa_alloc(void);
void pfa_free(uintptr_t paddr);
void pfa_free_range(uintptr_t start, uintptr_t end);

#endif

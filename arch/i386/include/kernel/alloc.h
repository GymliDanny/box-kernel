#ifndef I386_ALLOC_H
#define I386_ALLOC_H

#include <kernel/multiboot.h>
#include <stdint.h>
#include <stddef.h>

int alloc_init(struct mboot_info *info);
void mark_bitmap(uint32_t paddr, int present);
uint32_t pfa_alloc_frame(void);
void pfa_free(uint32_t paddr, int num_frames);

#endif

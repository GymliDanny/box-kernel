#ifndef KERNEL_MEM_H

#include <kernel/paging.h>
#include <stddef.h>

int init_kmalloc(void);
void* kmalloc(size_t sz);

#endif

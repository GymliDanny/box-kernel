#ifndef KERNEL_KMALLOC_H
#define KERNEL_KMALLOC_H

#include <stdint.h>
#include <stddef.h>

struct mem_block {
        uintptr_t start;
        size_t size;
        int alloc;
        struct mem_block *next;
};

void kmalloc_init(void);
void* kmalloc(size_t sz);
void kfree(void *ptr);

#endif

#ifndef LIBK_KMALLOC_H
#define LIBK_KMALLOC_H

#include <stdint.h>
#include <stddef.h>

struct mem_block {
        void *start;
        size_t size;
        int alloc;
        struct mem_block *next;
        struct mem_block *prev;
};

void kmalloc_init(void);
void* kmalloc(size_t sz);
void kfree(void *ptr);

#endif

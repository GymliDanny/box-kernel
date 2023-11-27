#ifndef MEM_H

#include <kernel/alloc.h>
#include <kernel/paging.h>
#include <stddef.h>

void* kmalloc(size_t sz);

#endif

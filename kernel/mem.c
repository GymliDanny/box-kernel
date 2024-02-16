#include <kernel/mem.h>
#include <kernel/paging.h>

extern uintptr_t _kernel_end;
static uintptr_t heap_start = (uintptr_t)&_kernel_end;
static uintptr_t heap_end = (uintptr_t)&_kernel_end;

void* kmalloc(size_t sz) {
        void *tmp = heap_end;
        heap_end += sz;
        return tmp;
}

void kfree(void *ptr) {
}

#include <kernel/mem.h>
#include <kernel/alloc.h>
#include <kernel/paging.h>

extern uint32_t _kernel_end;
static uint32_t *heap_start = &_kernel_end;
static uint32_t *heap_end = &_kernel_end;

int _alloc_new_page(void) {
        uint32_t paddr = pfa_alloc_frame();
        if (paddr == 0xFFFFFFFF)
                return -1;
        map_page(paddr, heap_end, 0x003);
}

void* kmalloc(size_t sz) {
        void *ret = heap_end;
        heap_end += sz;
        return ret;
}

void kfree(void *ptr) {
}

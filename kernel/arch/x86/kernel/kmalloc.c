#include <libk/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/pmem.h>
#include <kernel/asm.h>
#include <kernel/vmem.h>
#include <kernel/panic.h>
#include <stdint.h>

static uintptr_t heap_start = KEND + 0xC0001000;
static uintptr_t heap_end = 0xFFBFF000;

static void *kbrk = NULL;
static struct mem_block *first = NULL;

void _request_page(void) {
        uintptr_t paddr = pfa_alloc(1);
        uintptr_t vaddr = PGROUNDDN((uintptr_t)kbrk + PAGE_SIZE);
        kbrk += PAGE_SIZE;
        map_page(paddr, vaddr, PD_PRES | PD_RW);
}

void kmalloc_init(void) {
        kbrk = PGROUNDDN(heap_start);
        _request_page();
        first = (struct mem_block*)kbrk;
        first->start = (uintptr_t)kbrk + sizeof(struct mem_block);
        first->size = 0;
        first->alloc = 1;
        first->next = NULL;
}

void* kmalloc(size_t sz) {
        if (kbrk == NULL)
                panic("Attmpted to malloc before initialization");
        if ((uintptr_t)kbrk >= heap_end)
                return NULL;

        struct mem_block *temp = first;
        while (temp->next != NULL) {
                if (temp->next->size <= sz && temp->next->alloc == 0) {
                        temp->next->alloc = 1;
                        return (void*)temp->next->start;
                }
                temp = temp->next;
        }

        temp->next = (struct mem_block*)((uintptr_t)temp + sizeof(struct mem_block) + temp->size);
        if (temp->next > kbrk)
                _request_page();
        temp->next->prev = temp;
        temp->next->start = (uintptr_t)(temp->next + sizeof(struct mem_block));
        temp->next->size = sz;
        temp->next->alloc = 1;
        temp->next->next = NULL;
        return (void*)temp->start;
}

void kfree(void *ptr) {
        if (ptr == NULL)
                return;
        struct mem_block *temp = first;
        while (temp != NULL) {
                if (temp->start == (uintptr_t)ptr) {
                        temp->alloc = 0;
                        return;
                }
                temp = temp->next;
        }
        panic("Attempted to free memory not alloc'd");
}

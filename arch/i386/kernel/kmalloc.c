#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/pmem.h>
#include <kernel/asm.h>
#include <kernel/errno.h>
#include <kernel/panic.h>
#include <stdint.h>

static uintptr_t heap_start = KEND + 0xC0001000;
static uintptr_t heap_end = 0xFFFFF000;

static void *kbrk = NULL;
static struct mem_block *first = NULL;

void _request_page(void) {
        uintptr_t paddr = pfa_alloc();
        if (paddr == PFA_ALLOC_ERR)
                panic("Out of physical memory");

        uintptr_t vaddr = PGROUNDDN((uintptr_t)kbrk + PAGE_SIZE);
        kbrk += PAGE_SIZE;
        map_page(NULL, paddr, vaddr, PD_PRES | PD_RW);
}

void kmalloc_init(void) {
        kbrk = (void*)(heap_start & 0xFFFFF000);
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
                if (temp->size <= sz && temp->alloc == 0)
                        return (void*)temp->start;
                temp = temp->next;
        }

        temp->next = (struct mem_block*)((uintptr_t)temp + sizeof(struct mem_block) + temp->size);
        temp = temp->next;
        temp->start = (uintptr_t)(temp + sizeof(struct mem_block));
        temp->size = sz;
        temp->alloc = 1;
        temp->next = NULL;
        return (void*)temp->start;
}

void kfree(void *ptr) {
        struct mem_block *temp = first;
        while (temp->next != NULL) {
                if (temp->start == (uintptr_t)ptr) {
                        temp->alloc = 0;
                        return;
                }
        }
        panic("Attempted to free memory not alloc'd");
}

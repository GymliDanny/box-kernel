#include <kernel/paging.h>
#include <kernel/io.h>
#include <kernel/syscall.h>
#include <kernel/string.h>

extern uintptr_t _kernel_start;
extern uintptr_t _kernel_end;
static uintptr_t kstart = (uintptr_t)&_kernel_start;
static uintptr_t kend = (uintptr_t)&_kernel_end - 0xC0000000;

static uintptr_t page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t first_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

static int paging_enabled = 0;

static uint32_t dma_zone_bitmap[DMA_BITMAP_SZ];
static uint32_t buddy_bitmap[BDY_BITMAP_SZ];
static struct pfa_buddy first_buddy;

static void _pfa_init(void) {
        first_buddy.start = 0x01000000;
        first_buddy.bitmap = buddy_bitmap;
}

static void _pfa_alloc(uintptr_t paddr) {
        uintptr_t index = (paddr & 0xFFFFF000) / 4096 / 32;
        uintptr_t bit = (paddr & 0xFFFFF000) / 4096 % 32;
        uint32_t *bitmap;
        if (paddr < 0x01000000)
                bitmap = dma_zone_bitmap;
        else
                bitmap = first_buddy.bitmap;

        bitmap[index] |= (1 << bit);
}

static void _pfa_free(uintptr_t paddr) {
        uintptr_t index = (paddr & 0xFFFFF000) / 4096 / 32;
        uintptr_t bit = (paddr & 0xFFFFF000) / 4096 % 32;
        uint32_t *bitmap;
        if (paddr < 0x01000000)
                bitmap = dma_zone_bitmap;
        else
                bitmap = first_buddy.bitmap;

        bitmap[index] &= ~(1 << bit);
}

void paging_init(void) {
        _pfa_init();
        for (int i = 0; i < 1024; i++)
                page_directory[i] = 0x00000002;

        page_directory[1023] = ((uintptr_t)&page_directory - 0xC0000000) | 3;
        page_directory[0] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        page_directory[768] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        for (uintptr_t i = kstart; i < kend; i += 4096) {
                _pfa_alloc(get_vaddr(i));
                map_page(page_directory, i, get_vaddr(i), 0x003);
        }

        load_page_dir(((uintptr_t)&page_directory) - 0xC0000000);
        enable_paging();
        paging_enabled = 1;
        return;
}

void page_fault_handler(struct isr_frame *frame) {
        uintptr_t errno = frame->errno;

        uintptr_t fault_addr;
        __asm__ volatile("movl %%cr2, %0" : "=r"(fault_addr));
        int present = errno & ERR_PRESENT;
        int rw = errno & ERR_RW;
        int user = errno & ERR_USER;
        int reserved = errno & ERR_RESERVED;
        int ifetch = errno & ERR_INST;

        if (!present)
                _pfa_alloc(fault_addr);
                map_page(NULL, fault_addr, fault_addr, 0x003);
        if (user)
                panic("Usermode attempted to read supervisor page");
        if (rw)
                panic("Task wrote to read only page");
        if (reserved)
                panic("Task overwrote reserved page bits");
        if (ifetch)
                panic("Task paging instruction fetch failure");
}

uintptr_t get_vaddr(uintptr_t paddr) {
        return paddr + 0xC0000000;
}

void map_page(uint32_t *pd, uintptr_t paddr, uintptr_t vaddr, uint32_t flags) {
        if (pd == NULL)
                pd = page_directory;

        uintptr_t paddr_aligned = paddr & 0xFFFFF000;
        uintptr_t vaddr_aligned = vaddr & 0xFFFFF000;

        uintptr_t pdindex = vaddr_aligned >> 22;
        uintptr_t ptindex = vaddr_aligned >> 12 & 0x03FF;

        uintptr_t *pt = (pd[pdindex] + 0xC0000000 & 0xFFFFF000);
        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte |= paddr_aligned | (flags & 0xFFF) | 0x01;
}

void unmap_page(uint32_t *pd, uintptr_t vaddr) {
        if (pd == NULL)
                pd = page_directory;

        uintptr_t pdindex = vaddr >> 22;
        uintptr_t ptindex = vaddr >> 12 & 0x03FF;

        uintptr_t *pt = (pd[pdindex] + 0xC0000000 & 0xFFFFF000);
        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte &= 0;
}

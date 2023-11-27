#include <kernel/paging.h>
#include <kernel/io.h>
#include <kernel/syscall.h>
#include <kernel/string.h>
#include <kernel/alloc.h>
#include <stddef.h>

extern int halt_catch_fire();
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
static uint32_t *kstart = &_kernel_start;
static uint32_t *kend = (uint32_t)(&_kernel_end) - 0xC0000000;

static uint32_t page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
static uint32_t first_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

static int paging_enabled = 0;

void paging_init(void) {
        for (int i = 0; i < 1024; i++)
                page_directory[i] = 0x00000002;

        page_directory[1023] = ((uint32_t)&page_directory - 0xC0000000) | 3;
        page_directory[0] = ((uint32_t)&first_page_table - 0xC0000000) | 3;
        page_directory[768] = ((uint32_t)&first_page_table - 0xC0000000) | 3;
        for (uint32_t i = kstart; i < kend; i += 4096) {
                map_page(i, get_vaddr(i), 0x003);
                mark_bitmap(i, 1);
        }

        map_page(0xB8000, 0xC03FF000, 0x003);
        mark_bitmap(0xB8000, 1);

        load_page_dir(((uint32_t)&page_directory) - 0xC0000000);
        enable_paging();
        paging_enabled = 1;
        return;
}

void page_fault_handler(struct isr_frame *frame) {
        kprintf("Page fault detected\n");
        uint32_t errno = frame->errno;

        uint32_t fault_addr;
        __asm__ volatile("movl %%cr2, %0" : "=r"(fault_addr));
        kprintf("Fault occured at address %x\n", fault_addr);
        uint32_t present = errno & ERR_PRESENT;
        uint32_t rw = errno & ERR_RW;
        uint32_t user = errno & ERR_USER;
        uint32_t reserved = errno & ERR_RESERVED;
        uint32_t ifetch = errno & ERR_INST;
        kprintf("Possible causes:\n");
        if (!present) kprintf("\tPage not present\n");
        if (rw || user) kprintf("\tPage is read only\n");
        if (reserved) kprintf("\tOverwrote reserved bits\n");
        if (ifetch) kprintf("\tInstruction fetch\n");
        halt_catch_fire(frame);
}

uint32_t get_paddr(uint32_t vaddr) {
        uint32_t pdindex = vaddr >> 22;
        uint32_t ptindex = vaddr >> 12 & 0x03FF;

        uint32_t *page_table = (page_directory[pdindex] + 0xC0000000 & 0xFFFFF000);
        return (page_table[ptindex] & 0xFFFFF000) | (vaddr & 0x00000FFF);
}

uint32_t get_vaddr(uint32_t paddr) {
        return paddr + 0xC0000000;
}

void map_page(uint32_t paddr, uint32_t vaddr, uint32_t flags) {
        uint32_t pdindex = vaddr >> 22;
        uint32_t ptindex = vaddr >> 12 & 0x03FF;

        uint32_t *page_table = (page_directory[pdindex] + 0xC0000000 & 0xFFFFF000);
        uint32_t *pte = (uint32_t*)(&page_table[ptindex]);
        *pte |= paddr | (flags & 0xFFF) | 0x01;
}

void unmap_page(uint32_t vaddr) {
        uint32_t pdindex = vaddr >> 22;
        uint32_t ptindex = vaddr >> 12 & 0x03FF;

        uint32_t *page_table = (page_directory[pdindex] + 0xC0000000 & 0xFFFFF000);
        uint32_t *pte = (uint32_t*)(&page_table[ptindex]);
        *pte &= 0;
}

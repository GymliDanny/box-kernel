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
static uintptr_t second_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

static int paging_enabled = 0;

uint32_t* init_page_table(uint32_t flags) {
        uint32_t *ret = kmalloc(sizeof(uint32_t)*1024);
        for (int i = 0; i < 1024; i++)
                ret[i] = flags;
}

uint32_t* init_page_dir(uint32_t flags) {
        uint32_t *ret = init_page_table(flags);
        ret[0] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        ret[768] = ((uintptr_t)&second_page_table - 0xC0000000) | 3;
}

void paging_init(void) {
        for (int i = 0; i < 1024; i++) {
                page_directory[i] = PD_RW;
                first_page_table[i] = PD_RW;
                second_page_table[i] = PD_RW;
        }

        page_directory[1023] = ((uintptr_t)&page_directory - 0xC0000000) | 3;
        page_directory[0] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        page_directory[768] = ((uintptr_t)&second_page_table - 0xC0000000) | 3;
        for (uintptr_t i = kstart; i < kend; i += PAGE_SIZE)
                map_page(page_directory, i, i + 0xC0000000, 0x003);

        enable_paging(((uintptr_t)&page_directory) - 0xC0000000);
        paging_enabled = 1;
        return;
}

void page_fault_handler(struct regs *regs) {
        uintptr_t errno = regs->isr_err;

        uintptr_t fault_addr;
        __asm__ volatile("movl %%cr2, %0" : "=r"(fault_addr));
        int present = errno & ERR_PRESENT;
        int rw = errno & ERR_RW;
        int user = errno & ERR_USER;
        int reserved = errno & ERR_RESERVED;
        int ifetch = errno & ERR_INST;

        uintptr_t first_free;
        if (!present)
                map_page(regs->cr3, fault_addr, fault_addr, PD_PRES | PD_RW | PD_USR);
        if (user)
                panic("Usermode attempted to read supervisor page");
        if (rw)
                panic("Task wrote to read only page");
        if (reserved)
                panic("Task overwrote reserved page bits");
        if (ifetch)
                panic("Task paging instruction fetch failure");
}

uintptr_t get_paddr(uintptr_t vaddr) {
        uint32_t pdindex = (uint32_t)vaddr >> 22;
        uint32_t ptindex = (uint32_t)vaddr >> 12 & 0x03FF;

        uint32_t *pd = (uint32_t*)0xFFFFF000;
        if (*pd & PD_PRES != 1)
                return NULL;
        uint32_t *pt = ((uint32_t*)0xFFC00000) + (0x400 * pdindex);
        if (*pt & PD_PRES != 1)
                return NULL;
        return (uintptr_t)((pt[ptindex] & ~0xFFF) + ((uint32_t)vaddr & 0xFFF));
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

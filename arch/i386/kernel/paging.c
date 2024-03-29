#include <kernel/paging.h>
#include <kernel/asm.h>
#include <kernel/pmem.h>
#include <kernel/panic.h>
#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/syscall.h>
#include <kernel/string.h>

static uintptr_t page_directory[1024] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t first_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

static int paging_enabled = 0;

int init_page_table(uintptr_t *pd, int index, int user) {
        if (pd == NULL)
                return 0;

        uintptr_t paddr = pfa_alloc();
        if (paddr == PFA_ALLOC_ERR)
                return -ENOMEM;

        map_page(NULL, paddr, paddr, PD_PRES | PD_RW);
        if (user)
                pd[index] = paddr | PD_USR;
        else
                pd[index] = paddr;
}

int init_page_directory(uintptr_t *pd, int user) {
        uintptr_t paddr = pfa_alloc();
        if (paddr == PFA_ALLOC_ERR)
                return -ENOMEM;

        map_page(NULL, paddr, pd, PD_PRES | PD_RW);
        for (int i = 0; i < 1023; i++)
                init_page_table(pd, i, 0);
        pd[1023] = paddr | 3;
        return paddr;
}

static void _alloc_new_page(uint32_t *cr3, uintptr_t vaddr, int user, int rw) {
        uintptr_t paddr = pfa_alloc();
        if (paddr == PFA_ALLOC_ERR)
                panic("Cannot allocate physical page");

        uint32_t flags = PD_PRES;
        if (rw == 1)
                flags |= PD_RW;
        if (user == 1)
                flags |= PD_USR;
        map_page(cr3, paddr, vaddr, flags);
}

void paging_init(void) {
        if (paging_enabled == 1)
                return;

        for (int i = 0; i < 1024; i++) {
                page_directory[i] = PD_RW;
                first_page_table[i] = PD_RW;
        }

        page_directory[1023] = ((uintptr_t)&page_directory - 0xC0000000) | 3;
        page_directory[0] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        page_directory[768] = ((uintptr_t)&first_page_table - 0xC0000000) | 3;
        for (uintptr_t i = KSTART; i < KEND; i += PAGE_SIZE)
                map_page(page_directory, i, i + 0xC0000000, PD_PRES);

        enable_paging(((uintptr_t)&page_directory) - 0xC0000000);
        paging_enabled = 1;

        for (int i = 0; i < 1023; i++) {
                if (i == 0 || i == 768)
                        continue;
                init_page_table(page_directory, i, 0);
        }
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

        if (reserved || ifetch)
                panic("Unknown error in page fault");
        if (present && user)
                panic("User process generated protection fault"); // TODO: this shouldn't panic
        if (present && !user)
                panic("Kernel process generated protection fault");

        if (user && rw)
                _alloc_new_page((uint32_t*)regs->cr3, fault_addr, 1, 1);
        if (user && !rw)
                _alloc_new_page((uint32_t*)regs->cr3, fault_addr, 1, 0);
        if (rw)
                _alloc_new_page((uint32_t*)regs->cr3, fault_addr, 0, 1);
        if (!rw)
                _alloc_new_page((uint32_t*)regs->cr3, fault_addr, 0, 0);
}

uintptr_t get_paddr(uintptr_t vaddr) {
        uint32_t pdindex = (uint32_t)vaddr >> 22;
        uint32_t ptindex = (uint32_t)vaddr >> 12 & 0x03FF;

        uint32_t *pd = (uint32_t*)0xFFFFF000;
        if ((*pd & PD_PRES) != 1)
                return 0;
        uint32_t *pt = ((uint32_t*)0xFFC00000) + (0x400 * pdindex);
        if ((*pt & PD_PRES) != 1)
                return 0;
        return (uintptr_t)((pt[ptindex] & ~0xFFF) + ((uint32_t)vaddr & 0xFFF));
}

void map_page(uint32_t *pd, uintptr_t paddr, uintptr_t vaddr, uint32_t flags) {
        if (pd == NULL)
                pd = page_directory;

        paddr = PGROUNDDN(paddr);
        vaddr = PGROUNDDN(vaddr);

        uintptr_t pdindex = vaddr >> 22;
        uintptr_t ptindex = vaddr >> 12 & 0x03FF;

        uintptr_t *pt = (uintptr_t*)((pd[pdindex] + 0xC0000000) & 0xFFFFF000);
        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte |= paddr | (flags & 0xFFF) | 0x01;
}

void unmap_page(uint32_t *pd, uintptr_t vaddr) {
        if (pd == NULL)
                pd = page_directory;

        uintptr_t pdindex = vaddr >> 22;
        uintptr_t ptindex = vaddr >> 12 & 0x03FF;

        uintptr_t *pt = (uintptr_t*)((pd[pdindex] + 0xC0000000) & 0xFFFFF000);
        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte &= 0;
}

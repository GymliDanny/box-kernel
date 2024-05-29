#include <kernel/paging.h>
#include <kernel/asm.h>
#include <kernel/pmem.h>
#include <kernel/panic.h>
#include <kernel/syscall.h>
#include <libk/io.h>
#include <libk/string.h>

uintptr_t *kpgdir = NULL;

static uintptr_t kernel_pd[1024] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t kernel_pt[1024] __attribute__((aligned(PAGE_SIZE)));
static uintptr_t accnt_pt[1024] __attribute__((aligned(PAGE_SIZE)));
static int paging_enabled = 0;

void _copy_kernel_maps(uintptr_t *new_pd) {
        int pdx = GET_PDX(GET_VADDR(KSTART));
        new_pd[pdx] = GET_PADDR(kernel_pt) | PD_PRES | PD_RW;
        new_pd[832] = GET_PADDR(accnt_pt) | PD_PRES | PD_RW;
}

uintptr_t* init_page_table(void) {
        uintptr_t *ret = (uintptr_t*)pfa_alloc();
        if ((uintptr_t)ret == PFA_ALLOC_ERR)
                return NULL;

        uintptr_t *temp_map = (uintptr_t*)0xD0001000;
        map_page(NULL, (uintptr_t)ret, (uintptr_t)temp_map, PD_PRES | PD_RW);
        memset((char*)temp_map, 0, PAGE_SIZE);
        unmap_page(NULL, (uintptr_t)temp_map);
        return ret;
}

uintptr_t* init_page_directory(void) {
        uintptr_t *ret = (uintptr_t*)pfa_alloc();
        if ((uintptr_t)ret == PFA_ALLOC_ERR)
                return NULL;

        map_page(NULL, (uintptr_t)ret, (uintptr_t)ret, PD_PRES | PD_RW);
        memset(ret, 0, PAGE_SIZE);
        _copy_kernel_maps(ret);
        unmap_page(NULL, (uintptr_t)ret);

        return ret;
}

void paging_init(void) {
        if (paging_enabled == 1)
                return;
        
        _copy_kernel_maps(kernel_pd);
        for (uintptr_t i = KSTART; i < KEND; i += PAGE_SIZE)
                map_page(kernel_pd, i, GET_VADDR(i), PD_PRES);

        map_page(kernel_pd, 0xB8000, 0xC03FF000, PD_PRES | PD_RW);
        kpgdir = kernel_pd;
        enable_paging(GET_PADDR(kpgdir));
        paging_enabled = 1;
        return;
}

void map_page(uint32_t *pd, uintptr_t paddr, uintptr_t vaddr, uint32_t flags) {
        if (pd == NULL)
                pd = kpgdir;

        paddr = PGROUNDDN(paddr);
        vaddr = PGROUNDDN(vaddr);

        uintptr_t pdindex = GET_PDX(vaddr);
        uintptr_t ptindex = GET_PTX(vaddr);

        uintptr_t *pt = (uintptr_t*)(GET_VADDR(pd[pdindex]) & 0xFFFFF000);
        if (pd[pdindex] == 0)
                pd[pdindex] = (uintptr_t)init_page_table();

        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte |= paddr | (flags & 0xFFF) | 0x01;
        invlpg((void*)vaddr);
}

void unmap_page(uint32_t *pd, uintptr_t vaddr) {
        if (pd == NULL)
                pd = kpgdir;

        uintptr_t pdindex = GET_PDX(vaddr);
        uintptr_t ptindex = GET_PTX(vaddr);

        uintptr_t *pt = (uintptr_t*)(GET_VADDR(pd[pdindex]) & 0xFFFFF000);
        if (pd[pdindex] == 0)
                return;

        uintptr_t *pte = (uintptr_t*)(&pt[ptindex]);
        *pte &= 0;
        invlpg((void*)vaddr);
}

void page_fault_handler(struct isr_frame *frame) {
        uintptr_t fault_addr;
        __asm__ volatile("movl %%cr2, %0" : "=r"(fault_addr));

        switch (frame->isr_err) {
                case 0:
                        map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES);
                        break;
                case 1:
                        panic("Kernel process caused protection fault on read\n");
                        break;
                case 2:
                        map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_RW);
                        break;
                case 3:
                        panic("Kernel process caused protection fault on write\n");
                        break;
                case 4:
                        map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_USR);
                        break;
                case 5:
                        // TODO: instead of panicking, kill process
                        panic("User process caused protection fault on read\n");
                        break;
                case 6:
                        map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_RW | PD_USR);
                        break;
                case 7:
                        // TODO: see case 101
                        panic("USER process caused protection fault on write\n");
                        break;
                default:
                        kprintf("Unknown paging error occured on address %x\n", fault_addr);
                        panic("Paging error");
                        break;
        }
}

#include <kernel/paging.h>
#include <kernel/asm.h>
#include <kernel/vmem.h>
#include <kernel/pmem.h>
#include <kernel/panic.h>
#include <kernel/syscall.h>
#include <libk/io.h>
#include <libk/string.h>

static int paging_enabled = 0;

void paging_init(void) {
        if (paging_enabled == 1)
                return;

        uintptr_t phys_pt = pfa_alloc(1);
        map_page(phys_pt, PAGE_TMP_MAP, PD_PRES | PD_RW);
        uintptr_t *pt = (uintptr_t*)PAGE_TMP_MAP;
        for (uintptr_t i = KSTART; i < KEND; i += PAGE_SIZE)
                pt[GET_PTX(GET_VADDR(i))] = i | PD_PRES;
        unmap_page(PAGE_TMP_MAP);

        uintptr_t temp_pt = pfa_alloc(1);

        uintptr_t phys_pd = pfa_alloc(1);
        map_page(phys_pd, PAGE_TMP_MAP, PD_PRES | PD_RW);
        uintptr_t *pd = (uintptr_t*)PAGE_TMP_MAP;
        pd[GET_PDX(GET_VADDR(KSTART))] = phys_pt | PD_PRES | PD_RW;
        pd[GET_PDX(PAGE_TMP_MAP)] = temp_pt | PD_PRES | PD_RW;
        pd[1023] = phys_pd | PD_PRES | PD_RW;
        unmap_page(PAGE_TMP_MAP);

        enable_paging(phys_pd);
        paging_enabled = 1;
        return;
}

void init_page_table(int pdindex, uint32_t flags) {
        uintptr_t *pd = (uintptr_t*)PAGE_DIR_MAP;
        if ((pd[pdindex] & PD_PRES) != 0)
                return;

        uintptr_t paddr = pfa_alloc(1);
        pd[pdindex] = paddr | PD_PRES | PD_RW | flags;
        
        uintptr_t *pt = ((uintptr_t*)PAGE_TAB_MAP) + (pdindex << 12);
        pd[GET_PDX(pt)] = paddr | PD_PRES | PD_RW;
}

uintptr_t init_page_directory(void) {
        uintptr_t ret = pfa_alloc(1);
        map_page(ret, PAGE_TMP_MAP, PD_RW);

        uintptr_t *new_pd = (uintptr_t*)PAGE_TMP_MAP;
        uintptr_t *cur_pd = (uintptr_t*)PAGE_DIR_MAP;
        int kernel_pdx = GET_PDX(GET_VADDR(KSTART));
        new_pd[kernel_pdx] = cur_pd[kernel_pdx];

        unmap_page(PAGE_TMP_MAP);
        return ret;
}

void map_page(uintptr_t paddr, uintptr_t vaddr, uint32_t flags) {
        paddr = PGROUNDDN(paddr);
        vaddr = PGROUNDDN(vaddr);

        int pdindex = GET_PDX(vaddr);
        int ptindex = GET_PTX(vaddr);

        uintptr_t *pd = (uintptr_t*)PAGE_DIR_MAP;
        if ((pd[pdindex] & PD_PRES) == 0)
                init_page_table(pdindex, 0);

        uintptr_t *pt = (uintptr_t*)(PAGE_TAB_MAP + (pdindex << 12));
        pt[ptindex] = paddr | PD_PRES | flags;
        invlpg((void*)vaddr);
}

void unmap_page(uintptr_t vaddr) {
        uintptr_t pdindex = GET_PDX(vaddr);
        uintptr_t ptindex = GET_PTX(vaddr);

        uintptr_t *pd = (uintptr_t*)PAGE_DIR_MAP;
        if ((pd[pdindex] & PD_PRES) == 0)
                return;

        uintptr_t *pt = (uintptr_t*)(PAGE_TAB_MAP + (pdindex << 12));
        pt[ptindex] = 0;
        invlpg((void*)vaddr);
}

void page_fault_handler(struct isr_frame *frame) {
        uintptr_t fault_addr;
        __asm__ volatile("movl %%cr2, %0" : "=r"(fault_addr));

        kprintf("Faulting address: %x\n", fault_addr);
        panic("Page fault not operational");
        switch (frame->isr_err) {
                case 0:
                        //map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES);
                        break;
                case 1:
                        panic("Kernel process caused protection fault on read\n");
                        break;
                case 2:
                        //map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_RW);
                        break;
                case 3:
                        panic("Kernel process caused protection fault on write\n");
                        break;
                case 4:
                        //map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_USR);
                        break;
                case 5:
                        // TODO: instead of panicking, kill process
                        panic("User process caused protection fault on read\n");
                        break;
                case 6:
                        //map_page((uintptr_t*)GET_VADDR(frame->cr3), pfa_alloc(), fault_addr, PD_PRES | PD_RW | PD_USR);
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

#include <kernel/pmem.h>
#include <kernel/asm.h>
#include <kernel/panic.h>
#include <kernel/paging.h>
#include <kernel/string.h>

static struct pfa_zone bios_area;
static struct pfa_zone himem;

int pfa_init(struct mboot_info *header) {
        bios_area.start = 0;
        bios_area.size = 0;
        bios_area.freelist = NULL;

        himem.start = HIMEM_START;
        himem.size = 0;
        himem.freelist = NULL;

        struct mboot_mmap_entry *mme = (struct mboot_mmap_entry*)(header->mmap_addr);
        map_page(NULL, (uintptr_t)mme, (uintptr_t)mme, PD_PRES);
        while ((uintptr_t)mme < (header->mmap_addr + header->mmap_length)) {
                if (mme->addr_low >= KSTART && mme->addr_low <= KEND)
                        continue;
                if (mme->type == MBOOT_MEM_AVAILABLE) {
                        if (mme->addr_low < HIMEM_START)
                                pfa_free_range(&bios_area, (uintptr_t)mme->addr_low, (uintptr_t)(mme->addr_low+mme->len_low));
                        else
                                pfa_free_range(&himem, (uintptr_t)mme->addr_low, (uintptr_t)(mme->addr_low+mme->len_low));
                }
                unmap_page(NULL, (uintptr_t)mme);
                mme += sizeof(struct mboot_mmap_entry);
                map_page(NULL, (uintptr_t)mme, (uintptr_t)mme, PD_PRES);
        }
        unmap_page(NULL, (uintptr_t)mme);
        return 0;
}

uintptr_t pfa_alloc(void) {
        struct pfa_page *temp = bios_area.freelist;
        map_page(NULL, (uintptr_t)temp, (uintptr_t)temp, PD_RW | PD_PRES);
        if (temp == NULL)
                return PFA_ALLOC_ERR;

        bios_area.freelist = temp->next;
        memset(temp, PFA_BLOCK_ALLOC, 32);
        unmap_page(NULL, (uintptr_t)temp);
        return (uintptr_t)temp;
}

void pfa_free(struct pfa_zone *zone, uintptr_t paddr) {
        if (paddr % PAGE_SIZE != 0)
                panic("Task attempted to free non-aligned memory");
        if (paddr >= KSTART && paddr < KEND)
                panic("Task attempted to free kernel memory");

        map_page(NULL, paddr, paddr, PD_PRES | PD_RW);
        memset((void*)paddr, PFA_BLOCK_FREE, 32);
        struct pfa_page *temp = (struct pfa_page*)paddr;
        temp->next = zone->freelist;
        zone->freelist = temp;
        unmap_page(NULL, paddr);
        zone->size += PAGE_SIZE;
}

void pfa_free_range(struct pfa_zone *zone, uintptr_t pstart, uintptr_t pend) {
        uintptr_t p = PGROUNDUP(pstart);
        while (p <= pend) {
                pfa_free(zone, p);
                p += PAGE_SIZE;
        }
}

#include <kernel/pmem.h>
#include <kernel/asm.h>
#include <kernel/panic.h>
#include <kernel/paging.h>
#include <libk/io.h>
#include <libk/string.h>

struct pfa_page freelist;

void pfa_init(struct mboot_info *header) {
        struct mboot_mmap_entry *mme;
        struct mboot_mmap_entry *temp_map;
        for (uintptr_t i = 0; i < header->mmap_length; i += sizeof(struct mboot_mmap_entry)) {
                mme = (struct mboot_mmap_entry*)(header->mmap_addr + i);
                temp_map = (struct mboot_mmap_entry*)GET_VADDR(mme);
                map_page(NULL, (uintptr_t)mme, (uintptr_t)temp_map, PD_PRES);
                if (temp_map->type == MBOOT_MEM_AVAILABLE)
                        pfa_free_range(temp_map->addr_low, temp_map->addr_low + temp_map->len_low);
                unmap_page(NULL, (uintptr_t)temp_map);
        }
}

uintptr_t pfa_alloc(void) {
        struct pfa_page *ret = freelist.next;
        if (ret == NULL)
                return PFA_ALLOC_ERR;

        struct pfa_page *temp_map = (struct pfa_page*)0xD0000000;
        map_page(NULL, (uintptr_t)ret, (uintptr_t)temp_map, PD_PRES | PD_RW);
        memset((char*)temp_map, 0, PAGE_SIZE);
        unmap_page(NULL, (uintptr_t)temp_map);
        return (uintptr_t)ret;
}

void pfa_free(uintptr_t paddr) {
        if (paddr % PAGE_SIZE != 0)
                panic("Task attempted to free non-aligned memory");
        if (paddr == 0)
                return;
        if (paddr >= KSTART && paddr < KEND)
                return;
        if (paddr >= 0x80000 && paddr < 0x100000)
                return;

        struct pfa_page *temp_map = (struct pfa_page*)0x1000;
        map_page(NULL, paddr, (uintptr_t)temp_map, PD_PRES | PD_RW);
        memset(temp_map, 1, 32);
        temp_map->next = freelist.next;
        freelist.next = (struct pfa_page*)paddr;
        unmap_page(NULL, (uintptr_t)temp_map);
}

void pfa_free_range(uintptr_t start, uintptr_t end) {
        for (uintptr_t i = start; i < end; i += PAGE_SIZE)
                pfa_free(i);
}

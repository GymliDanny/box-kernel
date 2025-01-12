#include <kernel/pmem.h>
#include <kernel/asm.h>
#include <kernel/vmem.h>
#include <kernel/panic.h>
#include <kernel/paging.h>
#include <libk/io.h>
#include <libk/string.h>

static uintptr_t bitmap[5];
static struct pfa_page freelist;

void pfa_init(struct mboot_info *header) {
        struct mboot_mmap_entry *mme;
        for (uintptr_t i = 0; i < header->mmap_length; i += sizeof(struct mboot_mmap_entry)) {
                mme = (struct mboot_mmap_entry*)(header->mmap_addr + i);
                if (mme->type == MBOOT_MEM_AVAILABLE)
                        pfa_free(mme->addr_low, mme->len_low / PAGE_SIZE);
        }
}

uintptr_t pfa_alloc_dma(size_t num_pages) {
}

uintptr_t pfa_alloc(size_t num_pages) {
        struct pfa_page *temp = (struct pfa_page*)PAGE_TMP_MAP;
        map_page(freelist.next, PAGE_TMP_MAP, PD_PRES | PD_RW);
        uintptr_t ret = freelist.next;
        freelist.next = temp->next;
        memset(temp, 0, 32);
        unmap_page(PAGE_TMP_MAP);
        return ret;
}

void pfa_free_dma(uintptr_t paddr, size_t num_pages) {
        if (paddr % PAGE_SIZE != 0)
                panic("Task attempted to free non-aligned memory");
        if (paddr >= KSTART && paddr < KEND)
                return;
        if (paddr >= 0x00080000 && paddr < 0x00100000)
                return;

        int index = 0;
        int bit = 0;
        for (uintptr_t i = paddr; i < paddr + (num_pages * PAGE_SIZE); i += PAGE_SIZE) {
                index = i / PAGE_SIZE / 32;
                bit = i / PAGE_SIZE % 32;
                bitmap[index] |= 1 << bit;
        }
}

void pfa_free(uintptr_t paddr, size_t num_pages) {
        uintptr_t addr;
        struct pfa_page *temp = (struct pfa_page*)PAGE_TMP_MAP;
        for (size_t i = 0; i < num_pages; i++) {
                addr = (i * PAGE_SIZE + paddr);
                if (addr >= KSTART && addr < KEND)
                        continue;
                if (addr == 0)
                        continue;
                map_page(addr, PAGE_TMP_MAP, PD_PRES | PD_RW);
                memset(PAGE_TMP_MAP, 1, 32);
                temp->next = freelist.next;
                freelist.next = addr;
                unmap_page(PAGE_TMP_MAP);
        }
}

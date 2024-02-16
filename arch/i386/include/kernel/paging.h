#ifndef I386_PAGING_H
#define I386_PAGING_H

#include <kernel/isr.h>
#include <kernel/multiboot.h>
#include <kernel/data/list.h>
#include <stdint.h>

#define PD_PRES         0x0001
#define PD_RW           0x0002
#define PD_USR          0x0004
#define PD_PWT          0x0008
#define PD_PCD          0x0010
#define PD_ACC          0x0020
#define PD_DIRTY        0x0040
#define PD_PS           0x0080
#define PD_GLOB         0x0100
#define PD_PAT          0x1000

#define ERR_PRESENT     0x1
#define ERR_RW          0x2
#define ERR_USER        0x4
#define ERR_RESERVED    0x8
#define ERR_INST        0x10

#define PAGE_SIZE       4096

#define DMA_BITMAP_SZ   128
#define BDY_BITMAP_SZ   32640

struct pfa_buddy {
        uintptr_t start;
        uintptr_t *bitmap;
        uint8_t size;
        struct list_head list;
};

void load_page_dir(uintptr_t pd_addr);
void enable_paging(void);

void paging_init(void);
void page_fault_handler(struct isr_frame *frame);

uint32_t get_vaddr(uintptr_t paddr);

void map_page(uint32_t *pd, uintptr_t paddr, uintptr_t vaddr, uint32_t flags);
void unmap_page(uint32_t *pd, uintptr_t vaddr);

#endif

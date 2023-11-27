#ifndef I386_PAGING_H
#define I386_PAGING_H

#include <kernel/isr.h>
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

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BUCKET 8
#define BLOCK_ALIGN(addr) (((addr) & 0xFFFFF000) + 0x1000)

#define SETBIT(x,i)     x[i/BLOCKS_PER_BUCKET] = x[i/BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET))
#define CLEARBIT(x,i)   x[i/BLOCKS_PER_BUCKET] = x[i/BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
#define ISSET(x,i)      ((x[i/BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
#define GETBUCKET(x,i)  (*((uint32_t*)&x[i/32]))

#define PAGE_SIZE 4096

struct page_frame {
        int used;
        uint32_t addr;
};

void load_page_dir(uint32_t pd_addr);
void enable_paging(void);

void paging_init();
void page_fault_handler(struct isr_frame *frame);

uint32_t get_paddr(uint32_t vaddr);
uint32_t get_vaddr(uint32_t paddr);

void map_page(uint32_t paddr, uint32_t vaddr, uint32_t flags);
void unmap_page(uint32_t vaddr);

#endif

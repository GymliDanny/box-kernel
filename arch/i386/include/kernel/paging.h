#ifndef I386_PAGING_H
#define I386_PAGING_H

#include <kernel/asm.h>
#include <kernel/multiboot.h>
#include <stdint.h>

#define PAGE_SIZE       4096

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

#define PGROUNDUP(size) (((size)+PAGE_SIZE-1) & ~(PAGE_SIZE-1))
#define PGROUNDDN(size) (((size)) & ~(PAGE_SIZE-1))

#define GET_PADDR(x)    (((uint32_t)(x)) - 0xC0000000)
#define GET_VADDR(x)    (((uint32_t)(x)) + 0xC0000000)

#define GET_PDX(x)      ((uintptr_t)(x) >> 22)
#define GET_PTX(x)      (((uintptr_t)(x) >> 12) & 0x03FF)

uintptr_t get_physaddr(void *vaddr);

void paging_init(void);

uintptr_t init_page_directory(void);
void map_page(uintptr_t paddr, uintptr_t vaddr, uint32_t flags);
void unmap_page(uintptr_t vaddr);

void page_fault_handler(struct isr_frame *frame);

#endif

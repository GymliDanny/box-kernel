#ifndef I386_VMEM_H
#define I386_VMEM_H

extern uintptr_t _kernel_start;
extern uintptr_t _kernel_end;

#define KSTART          ((uintptr_t)&_kernel_start)
#define KEND            ((uintptr_t)&_kernel_end - 0xC0000000)

#define KHEAP_START     (GET_VADDR(KEND) + 0x2000)
#define KHEAP_STOP      0xFF7FF000

#define PAGE_DIR_MAP    0xFFFFF000
#define PAGE_TAB_MAP    0xFFC00000
#define PAGE_TMP_MAP    0xFFBFF000

#endif

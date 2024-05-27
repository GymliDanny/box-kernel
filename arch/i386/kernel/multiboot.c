#include <kernel/multiboot.h>
#include <kernel/asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/paging.h>
#include <kernel/pmem.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/video/framebuffer.h>
#include <libk/io.h>
#include <libk/string.h>

extern void kernel_main(char *cmdline);
extern uintptr_t *kpgdir;

void i386_entry(uint32_t mboot_magic, struct mboot_info *header, uintptr_t *entry_pd) {
        kpgdir = entry_pd;

        fb_init();
        if (mboot_magic != MBOOT_LOADER_MAGIC) {
                kprintf("NOT BOOTED WITH MULTIBOOT BOOTLOADER\n");
                kprintf("RESET PC!\n");
                disable_ints();
                while (1);
        }
        map_page(NULL, (uintptr_t)header, (uintptr_t)GET_VADDR(header), PD_PRES);
        if (!(header->flags >> 6 & 0x1)) {
                kprintf("NO MEMORY MAP FROM BOOTLOADER\n");
                kprintf("RESET PC!\n");
                disable_ints();
                while (1);
        }

        struct mboot_info hcopy;
        memcpy(&hcopy, header, sizeof(struct mboot_info));
        unmap_page(NULL, (uintptr_t)header);

        pfa_init(&hcopy);
        paging_init();
        kmalloc_init();
        gdt_install();
        idt_install();
        pic_remap();
        timer_init();

        //enable_ints();
        kernel_main((char*)header->cmdline);

        while (1);
}

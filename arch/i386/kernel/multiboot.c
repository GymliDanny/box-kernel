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
#include <libk/kmalloc.h>

extern void kernel_main(char *cmdline);
extern uintptr_t *kpgdir;

void i386_entry(uint32_t mboot_magic, struct mboot_info *header, uintptr_t *entry_pd) {
        kpgdir = entry_pd;

        fb_init();
        if (mboot_magic != MBOOT_LOADER_MAGIC) {
                disable_ints();
                panic("Not booted with multiboot bootloader");
        }
        map_page(NULL, (uintptr_t)header, (uintptr_t)header, PD_PRES);
        if (!(header->flags >> 6 & 0x1)) {
                disable_ints();
                panic("Physical memory map not provided by bootloader");
        }

        struct mboot_info hcopy;
        char cmdcopy[1024];
        memcpy(&hcopy, header, sizeof(struct mboot_info));
        map_page(NULL, (uintptr_t)header->cmdline, (uintptr_t)header->cmdline, PD_PRES);
        strcpy(cmdcopy, header->cmdline);

        pfa_init(&hcopy);
        paging_init();
        kmalloc_init();
        gdt_install();
        idt_install();
        pic_remap();
        timer_init();

        enable_ints();
        kernel_main(cmdcopy);

        while (1);
}

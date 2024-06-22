#include <kernel/multiboot.h>
#include <kernel/asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/paging.h>
#include <kernel/pmem.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/vmem.h>
#include <kernel/video/framebuffer.h>
#include <libk/io.h>
#include <libk/string.h>
#include <libk/kmalloc.h>

void kernel_main(char *cmdline);

void i386_entry(uint32_t mboot_magic, struct mboot_info *header) {
        map_page(0xB8000, (uintptr_t)VGA_MEMORY, PD_RW);
        fb_init();

        if (mboot_magic != MBOOT_LOADER_MAGIC) {
                disable_ints();
                panic("Not booted with multiboot bootloader");
        }

        map_page(0x9000, 0x9000, PD_RW);
        if (!(header->flags >> 6 & 0x1)) {
                disable_ints();
                panic("Physical memory map not provided by bootloader");
        }

        char cmdline[4096];
        map_page(header->cmdline, PAGE_TMP_MAP, 0);
        memcpy(cmdline, (char*)PAGE_TMP_MAP, strlen((char*)PAGE_TMP_MAP));
        unmap_page(PAGE_TMP_MAP);
        pfa_init(header);
        gdt_install();
        idt_install();
        paging_init();
        map_page(0xB8000, (uintptr_t)VGA_MEMORY, PD_RW);
        pic_remap();
        timer_init();

        enable_ints();
        kernel_main(cmdline);

        while (1);
}

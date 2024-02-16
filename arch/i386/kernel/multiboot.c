#include <kernel/multiboot.h>
#include <kernel/asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/paging.h>
#include <kernel/mem.h>
#include <kernel/keyboard.h>
#include <kernel/framebuffer.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/io.h>

extern void kernel_main(char *cmdline);

void i386_entry(uint32_t mboot_magic, struct mboot_info *header) {
        paging_init();
        fb_init();
        gdt_install();
        idt_install();
        pic_remap();
        timer_init();

        register_irq_handler(1, keyboard_handler);
        enable_ints();

        if (mboot_magic != MBOOT_LOADER_MAGIC) {
                fb_write("NOT BOOTED WITH MULTIBOOT BOOTLOADER\n", 37);
                fb_write("RESET PC!\n", 10);
                disable_ints();
                while (1);
        }
        if (!(header->flags >> 6 & 0x1)) {
                fb_write("NO MEMORY MAP FROM BOOTLOADER\n", 30);
                fb_write("RESET PC!\n", 10);
                disable_ints();
                while (1);
        }

        kernel_main((char*)header->cmdline);

        while (1);
}

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/panic.h>
#include <kernel/io.h>

void i386_entry(uint32_t mboot_magic, struct mboot_info *header) {
        paging_init();
        tty_init();

        struct mboot_info *vheader = get_vaddr(header);
        mark_bitmap(header, 1);
        map_page(header, vheader, 0x003);

        if (mboot_magic != MBOOT_LOADER_MAGIC)
                panic("NOT BOOTED WITH MULTIBOOT BOOTLOADER");
        if (!(vheader->flags >> 6 & 0x1))
                panic("NO MEMORY MAP FROM BOOTLOADER");

        gdt_install();
        idt_install();
        alloc_init(vheader);
        kernel_main(vheader->cmdline);
}

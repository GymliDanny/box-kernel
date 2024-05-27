#ifndef I386_MULTIBOOT_H
#define I386_MULTIBOOT_H

#include <stdint.h>

#define MBOOT_HEADER_MAGIC      0x1BADB002
#define MBOOT_LOADER_MAGIC      0x2BADB002

// Multiboot flags
#define MBOOT_INFO_MMAP         0x00000001
#define MBOOT_INFO_BOOTDEV      0x00000002
#define MBOOT_INFO_CMDLINE      0x00000004
#define MBOOT_INFO_MODS         0x00000008
#define MBOOT_INFO_AOUT_SYMS    0x00000010
#define MBOOT_INFO_ELF_HEADER   0x00000020
#define MBOOT_INFO_MEM_MAP      0x00000040
#define MBOOT_INFO_DRIVE_INFO   0x00000080
#define MBOOT_INFO_CONFIG_TBL   0x00000100
#define MBOOT_INFO_BL_NAME      0x00000200
#define MBOOT_INFO_APM_TBL      0x00000400
#define MBOOT_INFO_VBE          0x00000800
#define MBOOT_INFO_FRAMEBUFFER  0x00001000

// Memory types
#define MBOOT_MEM_AVAILABLE     1
#define MBOOT_MEM_RESERVED      2
#define MBOOT_MEM_ACPI_REC      3
#define MBOOT_MEM_NVS           4
#define MBOOT_MEM_BADRAM        5

struct aout_symbol_table {
        uint32_t tabsize;
        uint32_t strsize;
        uint32_t addr;
        uint32_t reserved;
};

struct elf_section_header {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
};

struct mboot_header {
        uint32_t magic;
        uint32_t flags;
        uint32_t checksum;
        uint32_t header_addr;
        uint32_t load_addr;
        uint32_t load_end_addr;
        uint32_t bss_end_addr;
        uint32_t entry_addr;
        uint32_t mode_type;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
};

struct mboot_info {
        uint32_t flags;
        uint32_t mem_lower;
        uint32_t mem_upper;
        uint32_t boot_device;
        uint32_t cmdline;
        uint32_t mods_count;
        uint32_t mods_addr;
        union {
                struct aout_symbol_table aout_sym;
                struct elf_section_header elf_sec;
        } u;
        uint32_t mmap_length;
        uint32_t mmap_addr;
        uint32_t drives_length;
        uint32_t drives_addr;
        uint32_t config_table;
        uint32_t boot_loader_name;
        uint32_t apm_table;
        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint16_t vbe_mode;
        uint16_t vbe_interface_seg;
        uint16_t vbe_interface_off;
        uint16_t vbe_interface_len;
        uint64_t framebuffer_addr;
        uint32_t framebuffer_pitch;
        uint32_t framebuffer_width;
        uint32_t framebuffer_height;
        uint8_t framebuffer_bpp;
        uint8_t framebuffer_type;
};

struct mboot_mmap_entry {
        uint32_t size;
        uint32_t addr_low;
        uint32_t addr_high;
        uint32_t len_low;
        uint32_t len_high;
        uint32_t type;
} __attribute__((packed));

#endif

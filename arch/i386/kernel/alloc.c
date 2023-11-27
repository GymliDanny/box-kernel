#include <kernel/alloc.h>
#include <kernel/paging.h>
#include <kernel/io.h>
#include <stdint.h>

extern uint32_t _bitmap_start;
static uint32_t *bm_start = &_bitmap_start;

static struct mboot_info *info;

int alloc_init(struct mboot_info *info) {
        struct mboot_mmap_entry *mme;
        for (uint32_t i = 0; i < info->mmap_length; i += sizeof(struct mboot_mmap_entry)) {
                mme = (struct mboot_mmap_entry*)(info->mmap_addr + i);
                if (mme->type != MBOOT_MEM_AVAILABLE)
                        mark_bitmap(mme->addr_low, 1);
        }
        return 0;
}

void mark_bitmap(uint32_t paddr, int present) {
        uint32_t index = (paddr & 0xFFFFF000) / 4096 / 32;
        uint32_t bit = (paddr & 0xFFFFF000) / 4096 % 32;
        bm_start[index] |= (present << bit);
}

uint32_t pfa_alloc_frame(void) {
        for (uint32_t i = 0; i < 4096; i++) {
                uint32_t index = i / 32;
                uint32_t bit = i % 32;
                if ((bm_start[index] & (1 << bit)) == 0) {
                        mark_bitmap(i*4096, 1);
                        return i * 4096;
                }
        }
        return 0xFFFFFFFF;
}

void pfa_free(uint32_t paddr, int num_frames) {
        for (int i = 0; i < num_frames; i++)
                mark_bitmap(paddr + (i*4096), 0);
}

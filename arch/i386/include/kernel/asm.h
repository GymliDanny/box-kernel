#ifndef I386_ASM_H
#define I386_ASM_H

static inline void enable_ints(void) {
        __asm__ volatile("sti");
        return;
}

static inline void disable_ints(void) {
        __asm__ volatile("cli");
        return;
}

static inline void flush_tss(void) {
        __asm__ volatile("movw $0x28, %ax; ltr %ax");
        return;
}

#endif

#include <kernel/panic.h>
#include <kernel/spinlock.h>
#include <libk/io.h>
#include <stdint.h>
#include <stddef.h>

static struct spinlock panic_lock = {0};
static int panicked = 0;

void walk_stack(uintptr_t *addrs, size_t n);

void stack_trace(void) {
        kprintf("PRINTING STACK TRACE\n");
        uintptr_t strace[32];
        walk_stack(strace, 32);
        for (int i = 0; i < 32; i++) {
                if (strace[i] == 0)
                        break;
                kprintf("#%d: %x\n", i, strace[i]);
        }
}

void panic(const char *str) {
        disable_ints();
        spin_lock(&panic_lock);
        panicked = 1;
        spin_unlock(&panic_lock);
        kprintf("KERNEL PANIC - NOT SYNCING: %s\n", str);
        stack_trace();
        while (1);
}

#include <kernel/panic.h>
#include <libk/io.h>
#include <stdint.h>
#include <stddef.h>

static int panicked = 0;

void panic(const char *str) {
        kprintf("KERNEL PANIC: %s\n", str);
        panicked = 1;
        while (1);
}

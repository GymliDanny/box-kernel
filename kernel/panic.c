#include <kernel/panic.h>
#include <kernel/io.h>

static int panicked = 0;

void panic(const char *str) {
        kprintf("KERNEL PANIC\n");
        kprintf("ERROR: %s\n", str);
        panicked = 1;
        while (1);
}

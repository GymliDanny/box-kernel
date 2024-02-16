#include <kernel/panic.h>
#include <kernel/io.h>

void panic(const char *str) {
        kprintf("KERNEL PANIC\n");
        kprintf("ERROR: %s\n", str);
        while (1);
}

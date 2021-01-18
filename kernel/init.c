#include <kernel/tty.h>
#include <kernel/io.h>

void kernel_main(void) {
        tty_init();
        printf("Hello world %x", 100);
}

#include <kernel/tty.h>
#include <kernel/io.h>
#include <kernel/serial.h>

void kernel_main(void) {
        //tty_init();
        kprintf("Hello world\n");
}

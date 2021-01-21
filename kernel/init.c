#include <kernel/tty.h>
#include <kernel/io.h>
#include <kernel/serial.h>

void kernel_main(void) {
        tty_init();
        serial_init();
        serial_writestring("Hello world\n");
        printf("Hello world\n");
}

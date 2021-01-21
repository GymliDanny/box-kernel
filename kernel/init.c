#include <kernel/tty.h>
#include <kernel/io.h>

void kernel_main(void) {
        tty_init();
        serial_init();
        printf("Hello world\n");
}

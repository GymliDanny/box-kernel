#include <kernel/io.h>
#include <kernel/mem.h>
#include <kernel/string.h>
#include <kernel/serial.h>

void jump_userspace(void);

char rootfs[1024];

int start_init(int argc, char* argv[]) {
        return 0;
}

void process_cmd(char *cmdline) {
        char *token = strtok(cmdline, " ");
        while (token != NULL) {
                if (strncmp(token, "root=", 5) == 0)
                        strcpy(rootfs, &token[6]);
        }
}

void kernel_main(char *cmdline) {
        kprintf("Box Kernel version %s\n", VERSION);
        serial_init();
}

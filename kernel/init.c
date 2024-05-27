#include <libk/io.h>
#include <libk/string.h>
#include <libk/kmalloc.h>
#include <kernel/sched.h>
#include <kernel/kthread.h>
#include <kernel/pci.h>
#include <kernel/serial.h>

void jump_userspace(void);

char rootfs[1024];
char init_bin[1024];
int gdbstub = 0;

void process_cmd(char *cmdline) {
        char *token = strtok(cmdline, " ");
        while (token != NULL) {
                if (strncmp(token, "root=", 5) == 0)
                        strcpy(rootfs, &token[5]);
                if (strncmp(token, "init=", 5) == 0)
                        strcpy(init_bin, &token[5]);
                if (strncmp(token, "gdb", 3) == 0)
                        gdbstub = 1;
                token = strtok(NULL, " ");
        }
}

void kernel_main(char *cmdline) {
        kprintf("Box kernel version %s\n", VERSION);
        process_cmd(cmdline);

        serial_init();
        sched_init();
        //pci_check_buses();
}

#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <libk/string.h>
#include <libk/io.h>
#include <stddef.h>

void sys_read(struct isr_frame *frame) {
}

void sys_write(struct isr_frame *frame) {
}

void handle_syscall(struct isr_frame *frame) {
        switch (frame->eax) {
                case SYS_READ:
                        sys_read(frame);
                        break;
                case SYS_WRITE:
                        sys_write(frame);
                        break;
                default:
                        panic("Invalid system call number");
        }
        return 0;
}

#include "syscall.h"
#include <kernel/io.h>
#include <stddef.h>

void *(*syscall_handlers[30])(struct isr_frame *frame);

void syscall_dispatch(struct isr_frame *frame) {
        if (syscall_handlers[frame->eax] != NULL)
                syscall_handlers[frame->eax](frame);
        else
                kprintf("Error: Invalid system call number: %d\n", frame->eax);
        __asm__ volatile("cli;hlt");
}

void register_syscall(void *handler(struct isr_frame*), int num) {
        syscall_handlers[num] = handler;
}

void print_hello(struct isr_frame *frame) {
        kprintf("Hello syscall\n");
}

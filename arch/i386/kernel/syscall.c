#include <kernel/syscall.h>
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

void sys_stop(struct isr_frame *frame) {
        kprintf("SYSTEM CALL: STOP\n");
        halt_catch_fire(frame);
}

void sys_status(struct isr_frame *frame) {
        kprintf("SYSTEM CALL: STATUS\n");
        dump_reg(frame);
}

void dump_reg(struct isr_frame *frame) {
        kprintf("Registers at interrupt:\n");
        kprintf("\tEAX = %x\n", frame->eax);
        kprintf("\tEBX = %x\n", frame->ebx);
        kprintf("\tECX = %x\n", frame->ecx);
        kprintf("\tEDX = %x\n", frame->edx);
        kprintf("\tESI = %x\n", frame->esi);
        kprintf("\tEDI = %x\n", frame->edi);
        kprintf("\tESP = %x\n", frame->esp);
        kprintf("\tEBP = %x\n", frame->ebp);
        kprintf("\tEIP = %x\n", frame->eip);
        kprintf("\tEFLAGS = %x\n", frame->eflags);
        kprintf("Current code selector: %d\n", frame->cs);
}

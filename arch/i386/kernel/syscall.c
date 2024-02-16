#include <kernel/syscall.h>
#include <kernel/io.h>
#include <stddef.h>

int handle_syscall(struct isr_frame *frame) {
        switch (frame->eax) {
                case SYS_REBOOT:
                        kprintf("REBOOT NOT SUPPORTED\n");
                        break;
                default:
                        kprintf("Error: Invalid system call number: %d\n", frame->eax);
                        halt_catch_fire(frame);
        }
        return 0;
}

void dump_reg(struct isr_frame *frame) {
        kprintf("Registers at interrupt:\n");
        kprintf("\tEAX = %x\n", frame->eax);
        kprintf("\tEBX = %x\n", frame->ebx);
        kprintf("\tECX = %x\n", frame->ecx);
        kprintf("\tEDX = %x\n", frame->edx);
        kprintf("\tESI = %x\n", frame->esi);
        kprintf("\tEDI = %x\n", frame->edi);
        kprintf("\tEIP = %x\n", frame->eip);
        kprintf("Current code selector: %x\n", frame->cs);
}

void dump_stack(uint32_t esp, size_t len) {
        for (uint32_t i = 0; i < len; i++)
                kprintf("%x:\t%x\n", esp+i, *(uint32_t*)(esp+i));
}

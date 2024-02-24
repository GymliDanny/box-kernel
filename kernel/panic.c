#include <kernel/panic.h>
#include <kernel/io.h>
#include <stdint.h>
#include <stddef.h>

static int panicked = 0;

void panic(const char *str) {
        kprintf("KERNEL PANIC: %s\n", str);
        panicked = 1;
        while (1);
}

void dump_reg(struct regs *regs) {
        kprintf("Registers at interrupt:\n");
        kprintf("\tEAX = %x\n", regs->eax);
        kprintf("\tEBX = %x\n", regs->ebx);
        kprintf("\tECX = %x\n", regs->ecx);
        kprintf("\tEDX = %x\n", regs->edx);
        kprintf("\tESI = %x\n", regs->esi);
        kprintf("\tEDI = %x\n", regs->edi);
        kprintf("\tEIP = %x\n", regs->eip);
        kprintf("Current code selector: %x\n", regs->cs);
}

void dump_stack(struct regs *regs, size_t len) {
        //for (uint32_t i = 0; i < len; i++)
        //        kprintf("%x:\t%x\n", esp+i, *(uint32_t*)(esp+i));
}

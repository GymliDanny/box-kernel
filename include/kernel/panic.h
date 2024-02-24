#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include <kernel/asm.h>
#include <stdint.h>
#include <stddef.h>

void panic(const char *str);
void dump_reg(struct regs *regs);
void dump_stack(struct regs *regs, size_t length);

#endif

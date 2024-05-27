#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include <kernel/asm.h>
#include <stdint.h>
#include <stddef.h>

void panic(const char *str);

#endif

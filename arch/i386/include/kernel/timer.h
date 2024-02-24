#ifndef I386_TIMER_H
#define I386_TIMER_H

#include <kernel/asm.h>

void timer_handler(struct regs *regs);
void timer_init(void);

#endif

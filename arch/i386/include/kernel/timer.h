#ifndef I386_TIMER_H
#define I386_TIMER_H

#include <kernel/isr.h>

void timer_handler(struct isr_frame *frame);
void timer_init(void);

#endif

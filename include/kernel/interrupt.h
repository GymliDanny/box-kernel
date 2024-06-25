#ifndef KERNEL_INTERRUPT_H
#define KERNEL_INTERRUPT_H

#include <kernel/asm.h>
#include <stdint.h>
#include <stddef.h>

void register_isr_handler(unsigned int isr, void (*handler)(struct isr_frame *frame));
void clear_isr_handler(unsigned int isr);

void isr_dispatch(struct isr_frame frame);

#endif

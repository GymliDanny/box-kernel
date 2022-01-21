#ifndef I386_SYSCALL_H
#define I386_SYSCALL_H

#include <kernel/isr.h>

void syscall_dispatch(struct isr_frame *frame);
void register_syscall(void *handler(struct isr_frame*), int num);
void sys_stop(struct isr_frame *frame);
void sys_status(struct isr_frame *frame);

void dump_reg(struct isr_frame *frame);

#endif

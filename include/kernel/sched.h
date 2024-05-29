#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include <kernel/asm.h>
#include <kernel/kthread.h>
#include <stdint.h>

void sched_init(void);
void schedule_next(void);
void schedule_thread(struct kthread *thread);

void block_thread(struct kthread *thread);
void unblock_thread(struct kthread *thread);

void sched_lock(void);
void sched_unlock(void);

void switch_thread(struct kthread *old, struct kthread *new);

#endif

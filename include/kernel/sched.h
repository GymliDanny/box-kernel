#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include <kernel/asm.h>
#include <kernel/data/list.h>

#define THREAD_READY    0
#define THREAD_RUNNING  1
#define THREAD_WAIT     2

struct task_block {
        unsigned int pid;
        unsigned int parent_pid;
        int nice;
        unsigned int num_threads;
        struct thread_block *threads;
        struct list_head list;
};

void sched_init(void);
void schedule_next(void);

struct task_block* init_task(int nice, unsigned int ppid);
void schedule_thread(struct thread_block *thread);
void switch_thread(struct thread_block *old, struct thread_block *new);

#endif

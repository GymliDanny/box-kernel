#include <kernel/sched.h>
#include <kernel/container_of.h>
#include <kernel/data/list.h>
#include <kernel/string.h>
#include <kernel/kmalloc.h>
#include <stddef.h>

static struct list_head *ready_queue;
static struct list_head *wait_queue;

static struct task_block *cur = NULL;
static long int next_id = 1;

static void _enqueue(struct list_head *queue, struct task_block *task) {
        struct list_head *temp = queue;
        while (temp->next != NULL)
                temp = temp->next;
        list_add(&task->list, temp);
}

static struct task_block* _dequeue(struct list_head *queue) {
        if (queue->next == NULL)
                return NULL;
        struct list_head *temp = queue->next;
        list_del(temp);
        struct task_block *ret = container_of(temp, struct task_block, list);
        return ret;
}

void sched_init(void) {
        ready_queue = kmalloc(sizeof(struct list_head));
        wait_queue = kmalloc(sizeof(struct list_head));
        struct task_block *boot_task = init_task(0, 0);
        boot_task->threads->state = THREAD_RUNNING;
        cur = boot_task;

        switch_thread(boot_task->threads, boot_task->threads);
}

void schedule_next(void) {
        struct task_block *task = _dequeue(ready_queue);
        if (task == NULL)
                return;
        if (cur != NULL)
                _enqueue(ready_queue, cur);
        disable_ints();
        switch_thread(cur->threads, task->threads);
        enable_ints();
        task->threads->state = THREAD_RUNNING;
        cur->threads->state = THREAD_READY;
        cur = task;
}

struct task_block* init_task(int nice, unsigned int ppid) {
        struct task_block *ret = kmalloc(sizeof(struct task_block));
        ret->pid = next_id++;
        ret->parent_pid = ppid;
        ret->nice = nice;
        ret->num_threads = 1;
        ret->threads = kmalloc(sizeof(struct thread_block));
        ret->threads->tid = ret->pid;
        ret->threads->tgid = ret->pid;
        return ret;
}

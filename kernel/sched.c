#include <kernel/sched.h>
#include <kernel/kthread.h>
#include <kernel/panic.h>
#include <libk/io.h>
#include <libk/kmalloc.h>
#include <libk/string.h>
#include <stddef.h>

static struct kthread *run_queue = NULL;
static struct kthread *wait_queue = NULL;
static struct kthread *cur = NULL;

static int scheduler_enabled = 0;
static struct kmutex scheduler_lock;

void sched_init(void) {
        disable_ints();
        cur = kthread_create(NULL, NULL);
        cur->stack = NULL;
        switch_thread(cur, cur);
        cur->state = THREAD_RUNNING;
        scheduler_enabled = 1;
        enable_ints();
}

void schedule_next(void) {
        if (scheduler_enabled == 0)
                return;
        if (run_queue == NULL)
                return;

        struct kthread *thread = cur;
        schedule_thread(cur);
        cur = run_queue;
        run_queue = run_queue->next;
        cur->next = NULL;
        cur->state = THREAD_RUNNING;
        switch_thread(thread, cur);
}

void schedule_thread(struct kthread *thread) {
        if (run_queue == NULL) {
                run_queue = thread;
                return;
        }

        kmutex_lock(&scheduler_lock);
        struct kthread *temp = run_queue;
        while (temp->next != NULL)
                temp = temp->next;
        temp->next = thread;
        thread->state = THREAD_READY;
        kmutex_unlock(&scheduler_lock);
}

void block_thread(struct kthread *thread) {
        kmutex_lock(&scheduler_lock);
        thread->next = wait_queue;
        wait_queue = thread;
        kmutex_unlock(&scheduler_lock);
}

void unblock_thread(struct kthread *thread) {
        kmutex_lock(&scheduler_lock);
        struct kthread *temp = wait_queue;
        while (temp != NULL) {
                if (temp->next == thread) {
                        temp->next = thread->next;
                        return;
                }
                temp = temp->next;
        }
        kmutex_unlock(&scheduler_lock);
}

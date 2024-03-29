#ifndef KERNEL_RINGBUF_H
#define KERNEL_RINGBUF_H

#include <kernel/kmalloc.h>
#include <kernel/string.h>
#include <stdint.h>

struct ringbuf {
        void *buffer;
        void *buf_end;
        uint32_t capacity;
        uint32_t count;
        uint32_t size;
        void *head;
        void *tail;
};

static inline void rb_init(struct ringbuf *rb, uint32_t capacity, uint32_t size) {
        rb->buffer = kmalloc(capacity * size);
        rb->buf_end = (char*)rb->buffer + (capacity * size);
        rb->capacity = capacity;
        rb->size = size;
        rb->head = rb->buffer;
        rb->tail = rb->buffer;
}

static inline int rb_push_back(struct ringbuf *rb, const void *item, size_t size) {
        if (rb->count == rb->capacity)
                return -1;
        if (size > rb->size)
                return -1;

        void *tmp = rb->tail + rb->size;
        if (tmp > rb->head + rb->capacity * rb->size)
                rb->tail = rb->head;
        memcpy(rb->tail, item, size);
        rb->tail += rb->size;
        rb->count++;
        return 0;
}

static inline void rb_pop_front(struct ringbuf *rb, void *item) {
        memcpy(item, rb->tail, rb->size);
        rb->tail = (char*)rb->tail + rb->size;
        if (rb->tail == rb->buf_end)
                rb->tail = rb->buffer;
        rb->count--;
}

#endif

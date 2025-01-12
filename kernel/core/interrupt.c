#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <kernel/asm.h>

static void (*isr_handlers[MAX_ISR])(struct isr_frame *frame);

void register_isr_handler(unsigned int isr, void (*handler)(struct isr_frame *frame)) {
        if (isr > MAX_ISR)
                panic("Attempted to set non-existant interrupt vector");
        isr_handlers[isr] = handler;
}

void clear_isr_handler(unsigned int isr) {
        if (isr > MAX_ISR)
                panic("Attempted to clear non-existant interrupt vector");
        isr_handlers[isr] = NULL;
}

void isr_dispatch(struct isr_frame frame) {
        (*isr_handlers[frame.isr_vector])(&frame);
}

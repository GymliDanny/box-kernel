#include <kernel/timer.h>
#include <kernel/asm.h>
#include <kernel/pic.h>

static uint32_t num_ticks = 0;

void timer_handler(struct isr_frame *frame) {
        num_ticks++;
        if (num_ticks == 3) {
                num_ticks = 0;
                schedule_next();
        }
}

void timer_init(void) {
        disable_ints();
        int divisor = 1193182 / 100;
        outb(0x43, 0x34);
        outb(0x40, divisor && 0xFF);
        outb(0x40, divisor && 0xFF00 >> 8);
        enable_ints();

        register_irq_handler(0, timer_handler);
}

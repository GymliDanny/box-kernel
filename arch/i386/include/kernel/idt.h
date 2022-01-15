#ifndef I386_IDT_H
#define I386_IDT_H

#include <stdint.h>

// IDT Flags
#define SEGMENT_PRESENT         0x80
#define SEGMENT_RING0           0x00
#define SEGMENT_RING3           0x60
#define SEGMENT_STORAGE         0x00
#define SEGMENT_INTERRUPT       0x0E
#define IDT_EXCEPTION           (SEGMENT_PRESENT | SEGMENT_INTERRUPT)
#define IDT_INTERRUPT           (SEGMENT_PRESENT | SEGMENT_INTERRUPT)

#define IDT_MAX_DESCRIPTORS     256

struct idt_entry {
        uint16_t        isr_low;
        uint16_t        kernel_cs;
        uint8_t         reserved;
        uint8_t         flags;
        uint16_t        isr_high;
} __attribute__((packed));

struct idt_ptr {
        uint16_t        limit;
        uint32_t        base;
} __attribute__((packed));

extern struct idt_entry idt[IDT_MAX_DESCRIPTORS];
extern struct idt_ptr idtr;

extern void idt_set_gate(uint8_t num, void(*handler)(void), uint16_t cs, uint8_t flags);

#endif

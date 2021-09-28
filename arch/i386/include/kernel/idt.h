#ifndef I386_IDT_H
#define I386_IDT_H

#include <stdint.h>

// IDT Flags
#define SEGMENT_PRESENT 0x8
#define SEGMENT_RING0   0x0
#define SEGMENT_STORAGE 0x0
#define SEGMENT_GATE    0xE
#define SEGMENT_DPL     0x8

#define SEGMENT_FLAG    SEGMENT_PRESENT | SEGMENT_RING0 | SEGMENT_STORAGE |\
                        SEGMENT_GATE    | SEGMENT_DPL;

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

extern struct idt_entry idt[256];
extern struct idt_ptr idtr;

extern void idt_set_gate(uint8_t num, void(*handler)(void), uint16_t cs, uint8_t flags);
extern void exception_handler(int num);

#endif
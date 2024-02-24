#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <kernel/framebuffer.h>
#include <kernel/keyboard.h>
#include <kernel/string.h>
#include <kernel/io.h>
#include <stddef.h>

extern char *keyboard_buffer;
extern uint32_t kbuf_pos;

void sys_read(struct regs *regs) {
        if (regs->ebx == 1) {
                while (kbuf_pos > regs->edx);
                memcpy((char*)regs->ecx, keyboard_buffer, regs->edx);
        }
}

void sys_write(struct regs *regs) {
        if (regs->ebx == 0)
                fb_write((char*)regs->ecx, regs->edx);
}

int handle_syscall(struct regs *regs) {
        switch (regs->eax) {
                case SYS_READ:
                        sys_read(regs);
                        break;
                case SYS_WRITE:
                        sys_write(regs);
                        break;
                default:
                        panic("Invalid system call number");
        }
        return 0;
}

#ifndef I386_SYSCALL_H
#define I386_SYSCALL_H

#include <kernel/asm.h>
#include <stdint.h>
#include <stddef.h>

// Unix standard calls
#define SYS_FORK        1
#define SYS_EXIT        2
#define SYS_WAIT        3
#define SYS_PIPE        4
#define SYS_READ        5
#define SYS_WRITE       6
#define SYS_KILL        7
#define SYS_FSTAT       8
#define SYS_CHDIR       9
#define SYS_DUP         10
#define SYS_GETPID      11
#define SYS_SLEEP       12
#define SYS_OPEN        13
#define SYS_MKNOD       14
#define SYS_UNLINK      15
#define SYS_LINK        16
#define SYS_MKDIR       17
#define SYS_CLOSE       18
#define SYS_EXEC        19

#define SYS_HALT        87
#define SYS_REBOOT      88

int handle_syscall(struct isr_frame *frame);

#endif

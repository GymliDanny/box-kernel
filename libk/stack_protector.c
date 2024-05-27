#include <kernel/panic.h>
#include <stdint.h>
#include <stddef.h>

#define STACK_CHK_GUARD 0x32E3429E

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail(void) {
        panic("STACK SMASHING IN KERNEL ADDRESS SPACE");
}

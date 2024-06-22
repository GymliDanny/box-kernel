#include <kernel/panic.h>
#include <stdint.h>

uintptr_t __stack_chk_guard;

__attribute__((noreturn)) void __stack_chk_fail(void) {
        panic("Detected attempted stack manipulation in kernel");
        while(1);
}

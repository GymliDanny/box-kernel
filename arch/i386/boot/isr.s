.section .text

.macro isr_err_stub num
.global isr_stub_\num
.type isr_stub_\num, @function
isr_stub_\num:
        pushl $\num
        jmp isr_frame_asm
.endm
.macro isr_no_err_stub num
.global isr_stub_\num
.type isr_stub_\num, @function
isr_stub_\num:
        pushl $0
        pushl $\num
        jmp isr_frame_asm
.endm

isr_frame_asm:
        pushl %ebp
        movl %esp, %ebp
        pushl %eax
        pushl %ebx
        pushl %ecx
        pushl %ecx
        pushl %edx
        pushl %esi
        pushl %edi

        movl %cr0, %eax
        pushl %eax
        movl %cr2, %eax
        pushl %eax
        movl %cr3, %eax
        pushl %eax
        movl %cr4, %eax
        pushl %eax

        movw %ds, %ax
        pushl %eax
        pushl $0
        movw $0x10, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %ss

        lea 0xC(%esp), %edi
        pushl %edi
        call exception_handler

        popl %eax
        popl %eax
        movw %ax, %ds
        movw %ax, %es

        popl %eax
        movl %eax, %cr4
        popl %eax
        movl %eax, %cr3
        popl %eax
        movl %eax, %cr2
        popl %eax
        movl %eax, %cr0

        popl %edi
        popl %esi
        popl %edx
        popl %ecx
        popl %ebx
        popl %eax

        popl %ebp
        add $0x10, %esp
        iret

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

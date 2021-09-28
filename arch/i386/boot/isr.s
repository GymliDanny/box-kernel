.section .text

.macro isr_err_stub num
.global isr_stub_\num
.type isr_stub_\num, @function
isr_stub_\num:
        cli
        pushl %eax
        pushl %ecx
        pushl %edx
        pushl %ds
        pushl %esp
        pushl $\num
        call exception_handler
        popl %esp
        popl %ds
        popl %edx
        popl %ecx
        popl %eax
        addl $8, %esp
        iret
.endm
.macro isr_no_err_stub num
.global isr_stub_\num
.type isr_stub_\num, @function
isr_stub_\num:
        cli
        pushl %eax
        pushl %ecx
        pushl %edx
        pushl %ds
        pushl %esp
        pushl $\num
        call exception_handler
        popl %esp
        popl %ds
        popl %edx
        popl %ecx
        popl %eax
        addl $8, %esp
        iret
.endm

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

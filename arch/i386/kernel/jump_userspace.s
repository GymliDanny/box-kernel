.global jump_userspace
.type jump_userspace, @function
jump_userspace:
        movw $0x23, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs

        movl %esp, %eax
        pushl $0x23
        pushl %eax
        pushf
        push $0x1B
        pushl $start_init

        pushl %esp
        call set_kernel_esp
        addl $4, %esp

        movl $0x0004, %eax
        orl $0x0002, %eax
        pushl %eax
        call init_page_dir
        addl $4, %esp

        movl %eax, %cr3

        iret

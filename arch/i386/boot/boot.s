.set ALIGN,     1<<0
.set MEMINFO,   1<<1
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)

.section .multiboot.data, "aw"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.section .bss, "aw", @nobits
        .align 4096
boot_page_directory:
        .skip 4096
boot_page_table0:
        .skip 4096

.section .multiboot.text, "a"
.global _start
.type _start, @function
_start:
        movl $(boot_page_table0 - 0xC0000000), %edi
        movl $0, %esi
        movl $1023, %ecx

1:      cmpl $_kernel_start, %esi
        jl 2f
        cmpl $(_kernel_end - 0xC0000000), %esi
        jge 3f

        movl %esi, %edx
        orl $0x003, %edx
        movl %edx, (%edi)

2:      addl $4096, %esi
        addl $4, %edi
        loop 1b

3:      movl $(0x000B8000 | 0x003), boot_page_table0 - 0xC0000000 + 1023 * 4

        movl $(boot_page_table0 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000
        movl $(boot_page_table0 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

        movl $(boot_page_directory - 0xC0000000), %ecx
        movl %ecx, %cr3

        movl %cr0, %ecx
        orl $0x80010000, %ecx
        movl %ecx, %cr0

        lea 4f, %ecx
        jmp *%ecx

.section .text

4:      movl $0, boot_page_directory + 0

        movl %cr3, %ecx
        movl %ecx, %cr3

        movl $stack_top, %esp

        call gdt_install
        call idt_install
        call pic_remap

        call kernel_main
        call jump_userspace

        cli
1:      hlt
        jmp 1b

.global flush_gdt
.type flush_gdt, @function
flush_gdt:
        cli
        lgdt (gp)
        movw $0x10, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss
        jmp $0x08, $.flush
.flush:
        ret

.global flush_tss
.type flush_tss, @function
flush_tss:
        movw $0x28, %ax
        ltr %ax
        ret

.global jump_userspace
.type jump_userspace, @function
jump_userspace:
        cli
        movw $0x23, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs

        pushl $0x23
        pushl %esp
        pushf
        orl $0x200, (%esp)
        pushl $0x8
        pushl $test_user_function
        iret

test_user_function:
        movl $1, %eax
        int $0x80

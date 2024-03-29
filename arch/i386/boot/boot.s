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
        orl $0x80000000, %ecx
        movl %ecx, %cr0

        lea 4f, %ecx
        jmp *%ecx

.section .text

4:      movl $0, boot_page_directory + 0

        movl %cr3, %ecx
        movl %ecx, %cr3

        movl $stack_top, %esp
        and $-16, %esp

        pushl %ebx
        pushl %eax
        call i386_entry

        cli
1:      hlt
        jmp 1b

.global load_page_dir
.type load_page_dir, @function
load_page_dir:
        pushl %ebp
        movl %esp, %ebp

        movl 8(%ebp), %eax
        movl %eax, %cr3

        movl %ebp, %esp
        popl %ebp
        ret

.global enable_paging
.type enable_paging, @function
enable_paging:
        movl %cr0, %eax
        orl $0x80000001, %eax
        movl %eax, %cr0
        ret

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

.global jump_userspace
.type jump_userspace, @function
jump_userspace:
        movw $0x23, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs

        pushl $0x23
        pushl %esp
        pushf
        orl $0x200, (%esp)
        pushl $0x1B
        pushl test_user_function
        iret

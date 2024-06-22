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
boot_page_table1:
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


3:      movl $boot_page_directory, %ecx
        subl $0xC0000000, %ecx
        orl $0x003, %ecx
        movl %ecx, boot_page_directory - 0xC0000000 + 1023 * 4

        movl $boot_page_table0, %ecx
        subl $0xC0000000, %ecx
        orl $0x003, %ecx
        movl %ecx, boot_page_directory - 0xC0000000
        movl %ecx, boot_page_directory - 0xC0000000 + 768 * 4

        movl $boot_page_table1, %ecx
        subl $0xC0000000, %ecx
        orl $0x003, %ecx
        movl %ecx, boot_page_directory - 0xC0000000 + 1022 * 4

        movl $boot_page_directory, %ecx
        subl $0xC0000000, %ecx
        movl %ecx, %cr3

        movl %cr0, %ecx
        orl $0x80000000, %ecx
        movl %ecx, %cr0

        lea 4f, %ecx
        jmp *%ecx

.section .text

4:      movl $stack_top, %esp
        and $-16, %esp

        call setup_stack_guard

        pushl %ebx
        pushl %eax
        call i386_entry

        cli
1:      hlt
        jmp 1b

.global setup_stack_guard
.type setup_stack_guard, @function
setup_stack_guard:
        pushl %eax
        pushl %ebx
        pushl %ecx

        movl $1, %eax
        movl $0, %ecx
        cpuid
        shrl $30, %ecx
        andl $1, %ecx
        jnz start_loop
        jmp fail
start_loop:
        rdrand %eax
        jc done
        loop start_loop
fail:
        movl $-1, %eax
done:
        movl %eax, __stack_chk_guard
        popl %ecx
        popl %ebx
        popl %eax
        ret

.global enable_paging
.type enable_paging, @function
enable_paging:
        pushl %ebp
        movl %esp, %ebp

        movl 8(%esp), %eax
        movl %eax, %cr3

        movl %cr0, %eax
        orl $0x80000001, %eax
        movl %eax, %cr0

        movl $0, %eax
        popl %ebp
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

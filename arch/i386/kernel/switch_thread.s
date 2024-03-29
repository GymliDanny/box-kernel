.global switch_thread
.type switch_thread, @function
switch_thread:
        pushl %ebp
        movl %esp, %ebp

        pushl %ebx
        pushl %esi
        pushl %edi
        pushl %ebp

        movl 8(%ebp), %esi
        movl 12(%ebp), %edi

        movl %esp, (%edi);
        movl %cr3, %ecx
        movl %ecx, 4(%edi)

        movl (%esi), %esp
        movl 4(%esi), %ecx
        movl %ecx, %cr3

        popl %ebp
        popl %edi
        popl %esi
        popl %ebx
        popl %ebp
        ret

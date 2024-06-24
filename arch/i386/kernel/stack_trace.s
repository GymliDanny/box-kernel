.section .text

.global walk_stack
.type walk_stack, @function
walk_stack:
        pushl %ebp
        movl %esp, %ebp

        pushl %edi
        movl -4(%ebp), %edi
        pushl %ebx
        movl -8(%ebp), %ebx

        xorl %eax, %eax
        movl 8(%esp), %ebx
        movl 16(%esp), %edi
        movl 20(%esp), %ecx
walk:
        testl %ebx, %ebx
        jz done
        movl 4(%ebx), %edx
        movl 0(%ebx), %ebx
        movl %edx, (%edi)
        addl $4, %edi
        inc %eax
        loop walk
done:
        popl %ebx
        popl %edi
        popl %ebp
        ret

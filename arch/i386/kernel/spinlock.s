.section .text

.global aquire_lock
.type aquire_lock, @function
aquire_lock:
        pushl %ebp
        movl %esp, %ebp
locked:
        movl 8(%esp), %eax
        lock incl (%eax)
        jc spin_wait

        popl %ebp
        ret
spin_wait:
        test %eax, 1
        jnz spin_wait
        jmp locked

.global release_lock
.type release_lock, @function
release_lock:
        pushl %ebp
        movl %esp, %ebp

        movl 8(%ebp), %eax
        lock decl (%eax)

        popl %ebp
        ret

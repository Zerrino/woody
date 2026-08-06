bits 64
default rel

.entry:
    lea rax, [rel .entry]
    mov rcx, 0x4242424242424242
    sub rax, rcx
    jmp rax

    mov rax, 60
    mov rdi, 1
    syscall

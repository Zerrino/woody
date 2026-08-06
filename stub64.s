bits 64
default rel

entry:

    jmp after_woody
    woody: db '....WOODY....',  0xA
    woody_len equ $ - woody

after_woody:

    push rdi
    push rsi
    push rdx

    mov rax, 1
    mov rdi, 1
    lea rsi, [rel woody]
    mov rdx, woody_len
    syscall

    lea rax, [rel entry]
    mov rdi, 0x4242424242424242
    sub rax, rdi

    pop rdx
    pop rsi
    pop rdi

    jmp rax

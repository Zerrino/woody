bits 64
default rel


%define ROUNDS 27
%define BLOCK_SIZE 16

entry:

    jmp after_woody
    woody: db '....WOODY........WOODY........WOODY....',  0xA
    woody_len equ $ - woody
after_woody:

    push    rdi
    push    rsi
    push    rdx


    lea     rdi, [rel woody]
    mov     rsi, woody_len
    call    speak_decrypt

    mov     rax, 1
    mov     rdi, 1
    lea     rsi, [rel woody]
    mov     rdx, woody_len
    syscall

    lea     rax, [rel entry]
    mov     rdi, 0x4242424242424242
    sub     rax, rdi

    pop     rdx
    pop     rsi
    pop     rdi

    jmp     rax


speck_key_schedule:
    lea     rdx, [rel flag]
    cmp     byte [rdx], 1
    je      .end
    mov     byte [rdx], 1

    mov     rdx, [rel key]
    mov     [rel round_keys], rdx
    mov     rbx, [rel key + 8]
    lea     rsi, [rel round_keys]
    mov     rcx, ROUNDS - 1
    xor     rax, rax
.loop:
    ror     rbx, 8
    mov     rdx, [rsi + rax*8]
    add     rbx, rdx
    xor     rbx, rax
    rol     rdx, 3
    xor     rdx, rbx
    mov     [rsi + rax*8 + 8], rdx
    inc     rax
    loop    .loop
.end:
    ret

speck_decrypt_128:
    mov     rax, [rdi]            ; rax = *x
    mov     rbx, [rsi]            ; rbx = *y
    lea     rdx, [rel round_keys]
    mov     rcx, ROUNDS - 1

.loop:
    xor     rbx, rax
    ror     rbx, 3
    xor     rax, [rdx + rcx*8]
    sub     rax, rbx
    rol     rax, 8
    test    rcx, rcx
    jz      .done
    dec     rcx
    jmp     .loop
.done:
    mov     [rdi], rax
    mov     [rsi], rbx
    ret


speak_decrypt: ; speack_decrypt(char *mem, size_t len) rdi rsi

    push    rdi
    push    rsi
    call    speck_key_schedule
    pop     rsi
    pop     rdi

    cmp     rsi, BLOCK_SIZE
    jb      .end
    xor     rcx, rcx
.loop:

    push    rdi
    push    rsi
    push    rcx

    add     rdi, rcx
    lea     rsi, [rdi + 8]

    call    speck_decrypt_128

    pop     rcx
    pop     rsi
    pop     rdi
    
    add      rcx, BLOCK_SIZE
    cmp      rcx, rsi
    jb      .loop
.end:
    ret


    key:    dq  0x1918111009080100, 0x1110980801000908
    round_keys: times ROUNDS dq 0
    flag:   db 0

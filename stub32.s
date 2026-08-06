bits 32
default rel

entry:

push_s:
    push ebx
    push ecx
    push edx
push_len equ $ - push_s

after_push_s:
    fldz
    fnstenv [esp-0x0C]



    jmp after_woody
until_woody equ $ - after_push_s

    woody: db '....WOODY....',  0xA
    woody_len equ $ - woody
after_woody:

    pop ecx
    push ecx
    add ecx, until_woody

    mov eax, 4
    mov ebx, 1
    mov edx, woody_len
    int 0x80

    pop eax
    sub eax, 0x42424242
    sub eax, push_len

    pop edx
    pop ecx
    pop ebx

    jmp eax

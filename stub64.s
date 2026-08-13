; ============================================
; STUB64.ASM - 64-bit x86-64 Unpacking Stub with SPECK DECRYPTION
; ============================================
; This stub loads into memory when the packed ELF executes
; Unlike stub32.s, this ONE DOES ACTUAL DECRYPTION using Speck cipher
; The embedded data after "hello" contains encrypted sections + Speck key
; ============================================

bits 64
default rel

%define ROUNDS 27			; Speck 128/256 rounds (hardcoded for key size)
%define BLOCK_SIZE 16		; Speck block size = 128 bits = 16 bytes

entry:

    jmp after_woody			; Skip over the "WOODY" marker string to reach code
    woody: db '....WOODY....',  0xA, 0, 0
    woody_len equ $ - woody

after_woody:
    push    rdi
    push    rsi
    push    rdx
    push    rcx
    push    rbx

	; STEP 1: SELF-DECRYPT THE MARKER SECTION (the stub itself)
    call    speck_key_schedule

    lea     rdi, [rel woody]		; RDI = destination buffer (address of "woody")
    mov     rsi, woody_len
    call    speak_decrypt

	; STEP 2: PRINT "WOODY" TO STDOUT (Debug/Marker)
	; write(1, woody, woody_len)
    mov     rax, 1					; sys_write
    mov     rdi, 1					; stdout
    lea     rsi, [rel woody]
    mov     rdx, woody_len
    syscall

	; STEP 3: LOOP THROUGH ENCRYPTED SECTIONS
    lea     rax, [rel addresse]
    mov     rcx, [rax]
    test    rcx, rcx
    jz      .end_blabla				; If none, skip decryption loop

.blabla:							; === DECRYPTION LOOP ===

    add     rax, 8
    lea     rdi, [rel entry]		; base address
    mov     rdx, [rax]
    sub     rdi, rdx
    add     rax, 8
    mov     rsi, [rax]

    push    rax
    push    rcx

    call    speak_decrypt

    pop     rcx
    pop     rax

    dec     rcx
    jnz     .blabla

.end_blabla:						; === EXIT DECRYPTION LOOP ===

	; STEP 4: CALCULATE AND JUMP TO ORIGINAL ENTRY POINT
    lea     rax, [rel entry]
    mov     rdi, 0x4242424242424242		; PLACEHOLDER (MUST BE PATCHED BY create_woody!)
    sub     rax, rdi					; Adjust: removes placeholder to get real entry

    pop     rbx
    pop     rcx
    pop     rdx
    pop     rsi
    pop     rdi
    jmp     rax							; Jump to original program entry

	; SPECK KEY SCHEDULE
	; Expands master key into round keys for encryption/decryption
    lea     rdx, [rel flag]
    cmp     byte [rdx], 1
    je      .end
    mov     byte [rdx], 1

	; Load master key (two 64-bit words = 128-bit key total)
    mov     rdx, [rel key]				; First half of key
    mov     [rel round_keys], rdx
    mov     rbx, [rel key + 8]			; Second half of key
    lea     rsi, [rel round_keys]
    mov     rcx, ROUNDS - 1
    xor     rax, rax


.loop:
    ror     rbx, 8						; Rotate right key
    mov     rdx, [rsi + rax*8]
    add     rbx, rdx
    xor     rbx, rax
    rol     rdx, 3						; Round key rotate left
    xor     rdx, rbx
    mov     [rsi + rax*8 + 8], rdx		; Store NEXT round key
    inc     rax
    loop    .loop
.end:
    ret

; SPECK 128-BIT SINGLE BLOCK DECRYPTION
; Decrypts exactly 16 bytes (one block)
speck_decrypt_128:						; Note: label says "_128" but called via "speak_decrypt"
    mov     rax, [rdi]					; *x
    mov     rbx, [rsi]					; *y
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
    mov     [rdi], rax					; Write decrypted first word
    mov     [rsi], rbx					; Write decrypted second word
    ret

; SPECK BLOCK DECRYPTION (MULTIPLE BLOCKS)
speak_decrypt:							; speack_decrypt(char *mem, size_t len)

    cmp     rsi, BLOCK_SIZE
    jb      .end
    xor     rcx, rcx

.loop:

    push    rdi
    push    rsi
    push    rcx

    add     rdi, rcx					; RDI += counter (advance through buffer)
    lea     rsi, [rdi + 8]

    call    speck_decrypt_128

    pop     rcx
    pop     rsi
    pop     rdi

    add      rcx, BLOCK_SIZE
    cmp      rcx, rsi					; BUG?: Should compare against original length, not RSI
    jb      .loop
.end:
    ret

	; DATA SECTION
    key:    dq  0x1918111009080100, 0x1110980801000908
			; Master key (128-bit): Two 64-bit words

    round_keys: times ROUNDS dq 0
			; Array to store expanded round keys (27 × 8 bytes = 216 bytes)

    flag:   db 0

    hello: db 'hello'

    addresse:
			; *** CRITICAL: This is a POINTER ARRAY that MUST BE PATCHED BY PACKER ***
			; Format expected (by create_woody()):
			; [8 bytes] Count of sections
			; [8 bytes] Offset/Address of section 1
			; [8 bytes] Length of section 1
			; [8 bytes] Offset/Address of section 2
			; [8 bytes] Length of section 2
			; ...etc
			; The packer must fill these with actual values before injecting stub

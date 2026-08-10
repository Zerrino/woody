; ============================================
; STUB64.ASM - 64-bit x86-64 Unpacking Stub with SPECK DECRYPTION
; ============================================
; This stub loads into memory when the packed ELF executes
; Unlike stub32.s, this ONE DOES ACTUAL DECRYPTION using Speck cipher
; The embedded data after "hello" contains encrypted sections + Speck key
; ============================================

bits 64						; Assemble for 64-bit (x86-64)
default rel					; Use RIP-relative addressing (position independent)


%define ROUNDS 27			; Speck 128/256 rounds (hardcoded for key size)
%define BLOCK_SIZE 16		; Speck block size = 128 bits = 16 bytes

; ============================================
; ENTRY POINT
; ============================================
entry:

    jmp after_woody			; Skip over the "WOODY" marker string to reach code
    woody: db '....WOODY....',  0xA, 0, 0
    woody_len equ $ - woody

after_woody:
	; Save all callee-saved registers before any operations
    push    rdi
    push    rsi
    push    rdx
    push    rcx
    push    rbx

	; ============================================
	; STEP 1: SELF-DECRYPT THE MARKER SECTION
	; First decrypt the area containing the stub itself (anti-debug/anti-analysis)
	; ============================================
    call    speck_key_schedule		; Generate expanded round keys from master key

    lea     rdi, [rel woody]		; RDI = destination buffer (address of "woody")
    mov     rsi, woody_len			; RSI = length to decrypt
    call    speak_decrypt			; Decrypt in place

	; ============================================
	; STEP 2: PRINT "WOODY" TO STDOUT (Debug/Marker)
	; Linux syscall: write(1, woody, woody_len)
	; ============================================
    mov     rax, 1					; syscall number 1 = sys_write
    mov     rdi, 1					; fd = 1 (stdout)
    lea     rsi, [rel woody]		; buffer address
    mov     rdx, woody_len			; length
    syscall							; Invoke syscall (64-bit native interface)

	; ============================================
	; STEP 3: LOOP THROUGH ENCRYPTED SECTIONS
	; Decrypt additional segments stored at `addresse:` pointer array
	; ============================================
    lea     rax, [rel addresse]		; RAX = pointer to array of pointers
    mov     rcx, [rax]				; RCX = counter (first entry = number of sections?)
    test    rcx, rcx				; Check if counter is zero
    jz      .end_blabla				; If none, skip decryption loop

.blabla:							; === DECRYPTION LOOP ===

    add     rax, 8					; Move to next pointer in array (8 bytes each)
    lea     rdi, [rel entry]		; RDI = base address (relative to entry point)
    mov     rdx, [rax]				; RDX = offset/delta to apply (from pointer array)
    sub     rdi, rdx				; RDI = target address to decrypt
    add     rax, 8					; Advance pointer array again
    mov     rsi, [rax]				; RSI = length of this segment

    push    rax						; Save pointer array position
    push    rcx						; Save loop counter

    call    speak_decrypt			; Decrypt segment at RDI for RSI bytes

    pop     rcx						; Restore counter
    pop     rax						; Restore pointer array position

    dec     rcx						; Decrement remaining sections
    jnz     .blabla					; Continue if more sections to decrypt

.end_blabla:						; === EXIT DECRYPTION LOOP ===

	; ============================================
	; STEP 4: CALCULATE AND JUMP TO ORIGINAL ENTRY POINT
	; ============================================
    lea     rax, [rel entry]			; RAX = current stub address
    mov     rdi, 0x4242424242424242		; PLACEHOLDER (MUST BE PATCHED BY create_woody!)
    sub     rax, rdi					; Adjust: removes placeholder to get real entry

    pop     rbx
    pop     rcx
    pop     rdx
    pop     rsi
    pop     rdi
    jmp     rax							; Jump to original program entry point - DONE!


	; ============================================
	; SPECK KEY SCHEDULE
	; Expands master key into round keys for encryption/decryption
	; Only runs once (flag prevents re-execution)
	; ============================================
    lea     rdx, [rel flag]				; Address of initialization flag
    cmp     byte [rdx], 1				; Check if already initialized
    je      .end						; If yes, skip (already computed)
    mov     byte [rdx], 1				; Mark as initialized

	; Load master key (two 64-bit words = 128-bit key total)
    mov     rdx, [rel key]				; First half of key
    mov     [rel round_keys], rdx		; Store as first round key
    mov     rbx, [rel key + 8]			; Second half of key
    lea     rsi, [rel round_keys]		; RSI = base of round_keys array
    mov     rcx, ROUNDS - 1				; Loop counter (27 rounds - 1 = 26 iterations)
    xor     rax, rax					; Index counter = 0


.loop:
    ror     rbx, 8						; Rotate right key part by 8 bits
    mov     rdx, [rsi + rax*8]			; Load current round key
    add     rbx, rdx					; RBX += round_key[i]
    xor     rbx, rax					; RBX ^= index
    rol     rdx, 3						; Round key rotate left by 3
    xor     rdx, rbx					; RDX ^= modified key part
    mov     [rsi + rax*8 + 8], rdx		; Store NEXT round key
    inc     rax							; Increment index
    loop    .loop						; Decrement RCX, jump if != 0
.end:
    ret

; ============================================
; SPECK 128-BIT SINGLE BLOCK DECRYPTION
; Decrypts exactly 16 bytes (one block)
; Input: RDI = ptr to first word, RSI = ptr to second word
; ============================================
speck_decrypt_128:						; Note: label says "_128" but called via "speak_decrypt"
    mov     rax, [rdi]					; RAX = first 8 bytes (*x)
    mov     rbx, [rsi]					; RBX = second 8 bytes (*y)
    lea     rdx, [rel round_keys]		; RDX = round keys array base
    mov     rcx, ROUNDS - 1				; Loop counter (26 iterations)

.loop:
    xor     rbx, rax					; Y ^= X
    ror     rbx, 3						; Y rotate right 3
    xor     rax, [rdx + rcx*8]			; X ^= round_key[counter]
    sub     rax, rbx					; X -= Y
    rol     rax, 8						; X rotate left 8
    test    rcx, rcx					; Check if counter reached 0
    jz      .done						; If done, exit loop
    dec     rcx							; Decrement counter
    jmp     .loop						; Continue decrypting
.done:
    mov     [rdi], rax					; Write decrypted first word
    mov     [rsi], rbx					; Write decrypted second word
    ret

; ============================================
; SPECK BLOCK DECRYPTION (MULTIPLE BLOCKS)
; rdi = memory address, rsi = length in bytes
; ============================================
speak_decrypt:							; speack_decrypt(char *mem, size_t len) rdi rsi
										; Parameters: RDI=pointer, RSI=length

    cmp     rsi, BLOCK_SIZE				; Compare length vs minimum block size
    jb      .end						; If length < 16 bytes, nothing to decrypt
    xor     rcx, rcx					; Block counter = 0

.loop:

    push    rdi
    push    rsi
    push    rcx

    add     rdi, rcx					; RDI += counter (advance through buffer)
    lea     rsi, [rdi + 8]				; RSI = address of second word (x+8 bytes)

    call    speck_decrypt_128			; Decrypt one 16-byte block

    pop     rcx
    pop     rsi
    pop     rdi

    add      rcx, BLOCK_SIZE			; Counter += 16 bytes
    cmp      rcx, rsi					; Compare progress vs total length
										; BUG?: Should compare against original length, not RSI
    jb      .loop						; Continue if not finished
.end:
    ret									; All blocks decrypted

; ============================================
; DATA SECTION
; Embedded constants for Speck cipher
; ============================================
    key:    dq  0x1918111009080100, 0x1110980801000908
			; Master key (128-bit): Two 64-bit words
    		; This MUST match the key used by the packer to encrypt!

    round_keys: times ROUNDS dq 0
			; Array to store expanded round keys (27 × 8 bytes = 216 bytes)
        	; Generated dynamically by speck_key_schedule()

    flag:   db 0
			; Initialization flag (0 = not initialized, 1 = done)
			; Prevents re-running key schedule on repeated calls

    hello: db 'hello'
			; String at end - possibly padding or debug marker

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

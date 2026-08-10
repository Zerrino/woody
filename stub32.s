; ============================================
; STUB32.ASM - 32-bit x86 Unpacking Stub
; ============================================
; This stub loads into memory when the packed ELF executes
; It prints "WOODY" then jumps to the real program entry point
; NOTE: 0x42424242 is a PLACEHOLDER - must be patched by create_woody()
; ============================================

bits 32						; Assemble for 32-bit (x86)
default rel					; Use RIP-relative addressing (for position independence)

; --------------------------------------------
; Entry Point
; --------------------------------------------
entry:

; --------------------------------------------
; SAVE REGISTERS
; Preserve callee-saved registers for later restoration
; --------------------------------------------
push_s:
    push ebx
    push ecx
    push edx
push_len equ $ - push_s		; Calculate how many bytes were pushed (12 bytes = 3 regs × 4)

; --------------------------------------------
; CALCULATE BASE ADDRESS
; Use x87 FPU trick to get current EIP value
; fnstenv stores the instruction pointer in its stack frame
; --------------------------------------------
after_push_s:
    fldz
    fnstenv [esp-0x0C]

    jmp after_woody


until_woody equ $ - after_push_s	; Distance from start to jmp instruction (used for offset calculation)

    woody: db '....WOODY....',  0xA
    woody_len equ $ - woody

after_woody:

    pop ecx							; Recover saved ECX (also gets address after pushes)
    push ecx						; Push it again (preserve on stack)
    add ecx, until_woody			; Calculate position of "WOODY" string by adding offset
									; Now ECX points to 'W' in "...WOODY..."

    mov eax, 4						; syscall number 4 = sys_write
    mov ebx, 1						; fd = 1 (stdout)
    mov edx, woody_len
    int 0x80						; Invoke kernel syscall (32-bit interrupt)

    pop eax							; Pop the address after syscall setup
    sub eax, 0x42424242				; Remove placeholder - THIS MUST BE PATCHED BY PACKER!
									; If not patched, jumps to garbage address (crash)
    sub eax, push_len				; Adjust for the push instructions (account for register saves)

    pop edx
    pop ecx
    pop ebx

    jmp eax							; Jump to real program - UNPACKING COMPLETE

; ============================================
; IMPORTANT NOTES:
; ============================================
; 1. The 0x42424242 value MUST be patched by create_woody() with actual
;    original_entry_point address before the stub is injected
; 
; 2. This stub does NOT decrypt anything - it's a PASS-THROUGH only
;    The 64-bit version (stub64.s) likely contains Speck decryption
; 
; 3. This requires executable memory permissions - if mapped as RW only,
;    CPU will throw SIGSEGV on jump
; 
; 4. Position-dependent: uses relative offsets throughout, works with ASLR
; ============================================

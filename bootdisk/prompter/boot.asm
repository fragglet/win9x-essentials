    org 0x7c00

entrypoint:
    jmp real_start
    nop

    times 0x3e - ($ - $$) db 0  ; Leave space for BPB

real_start:
    cli
    push cs              ; ds=cs
    pop ds

    mov ah, 0x0e
    mov al, '!'          ; !
    mov bx, 0x000f
    int 0x10

forever:
    jmp forever

do_ret:
    ret

    ; PC BIOS boot signature
    times 0x1fe - ($ - $$) db 0
    db 0x55, 0xaa

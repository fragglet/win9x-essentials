%define BIOS_READ_KEY        0x00
%define BIOS_PRINT_CHAR      0x0e

org 0x7c00

entrypoint:
    jmp real_start
    nop

    times 0x3e - ($ - $$) db 0  ; Leave space for BPB

real_start:
    cli
    push cs              ; ds=cs
    pop ds

    mov al, '!'
    call print_char

loop:
    call read_key
    test al, al
    jz loop
    call print_char
    jmp loop

; Returns ah=scancode, al=ascii
read_key:
    mov ah, BIOS_READ_KEY
    int 0x16
    ret

; Call with al=key
print_char:
    mov ah, BIOS_PRINT_CHAR
    mov bx, 0x000f
    int 0x10
    ret

forever:
    jmp forever

do_ret:
    ret

    ; PC BIOS boot signature
    times 0x1fe - ($ - $$) db 0
    db 0x55, 0xaa

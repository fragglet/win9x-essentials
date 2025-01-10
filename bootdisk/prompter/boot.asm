%define BIOS_READ_KEY        0x00
%define BIOS_CHECK_KEY       0x01
%define BIOS_PRINT_CHAR      0x0e
%define BIOS_GET_SYSTEM_TIME 0x00

org 0x7c00

entrypoint:
    jmp real_start
    nop

    times 0x3e - ($ - $$) db 0  ; Leave space for BPB

real_start:
    cli
    push cs              ; ds=cs
    pop ds

    mov al, '?'
    call print_char

loop:
    call check_enter_pressed
    jz enter_pressed

    mov al, '.'
    call print_char

    call sleep_one_tick
    jmp loop

enter_pressed:
    mov al, '!'
    call print_char
    jmp forever

; Returns zf unset if enter has been pressed
check_enter_pressed:
    mov ah, BIOS_CHECK_KEY
    int 0x16
    jz enter_not_pressed

    mov ah, BIOS_READ_KEY
    int 0x16
    cmp al, 13
    ret

enter_not_pressed:
    mov ah, 0
    cmp ah, 1
    ret

; Call with al=key
print_char:
    mov ah, BIOS_PRINT_CHAR
    mov bx, 0x000f
    int 0x10
    ret

; Sleeps for 1/18.2 of a second and then returns
sleep_one_tick:
    mov ah, BIOS_GET_SYSTEM_TIME
    mov al, 0x00
    int 0x1a
    push cx
    push dx

sleep_loop:
    mov ah, BIOS_GET_SYSTEM_TIME
    mov al, 0x00
    int 0x1a
    pop bx
    pop ax

    cmp ax, cx              ; time advanced?
    jne do_ret
    cmp bx, dx
    jne do_ret

    push ax
    push bx
    jmp sleep_loop

forever:
    jmp forever

do_ret:
    ret

    ; PC BIOS boot signature
    times 0x1fe - ($ - $$) db 0
    db 0x55, 0xaa

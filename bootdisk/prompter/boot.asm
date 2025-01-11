%define BIOS_READ_KEY        0x00
%define BIOS_CHECK_KEY       0x01
%define BIOS_PRINT_CHAR      0x0e
%define BIOS_GET_SYSTEM_TIME 0x00
%define BIOS_READ_SECTOR     0x02

org 0x7c00

entrypoint:
    jmp real_start
    nop

    times 0x3e - ($ - $$) db 0  ; Leave space for BPB

real_start:
    cli
    push cs              ; ds=cs
    pop ds

    call relocate_high

    mov al, '?'
    call print_char

    mov cx, 54            ; 3 seconds

wait_loop:
    push cx
    call check_enter_pressed
    jz enter_pressed

    mov al, '.'
    call print_char

    call sleep_one_tick
    pop cx
    loop wait_loop

    mov al, '|'
    call print_char

    ; Enter wasn't pressed, so proceed to boot from the hard drive instead
    mov ch, 0                ; track
    mov cl, 1                ; boot sector
    mov dh, 0                ; head
    mov dl, 0x80             ; first hard drive
    push dx
    mov bx, entrypoint
    call load_sector
    pop dx                   ; bootloader expects dx=drive number
    ; TODO: on error, try other hard disks?

chainload:
    mov bx, cs
    sub bx, 32
    push bx
    push entrypoint
    mov cx, 1
    mov bx, entrypoint
    retf

enter_pressed:
    mov al, '!'
    call print_char
    jmp forever

; Returns zf unset if enter has been pressed
check_enter_pressed:
    mov ah, BIOS_CHECK_KEY
    int 0x16
    jz enter_not_pressed

    call read_key
    cmp al, 13
    ret

; Blocks until keypress, then returns al=key, ah=scancode
read_key:
    mov ah, BIOS_READ_KEY
    int 0x16
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

; Call with bx=message address
print:
    mov al, [bx]
    test al, al
    jz do_ret
    push bx
    call print_char
    pop bx
    inc bx
    jmp print

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

; Copy the whole boot sector to a different segment and jump to it
relocate_high:
    mov si, entrypoint
    mov di, entrypoint
    mov bx, cs
    add bx, 32                   ; segment +512 bytes up
    push bx
    pop es
    mov cx, 512
    rep movsb

    ; Move ds to new segment
    push es
    pop ds

    ; Jump to do_ret but in the copied space
    push es
    push do_ret
    retf

; Read sector given by cx:dx into memory at entrypoint location
load_sector:
    mov ah, BIOS_READ_SECTOR
    mov al, 1                       ; One sector
    mov bx, ds
    sub bx, 32
    push bx
    pop es
    mov bx, entrypoint
    int 0x13
    jc abort_error
    ret

abort_error:
    mov bx, error_msg
    call print
    call read_key
    int 0x19                   ; Reboot

error_msg:
    db 13, 10, "Error reading from disk. Press a key to reboot.", 13, 10, 0

    times 0x1fe - ($ - $$) db 0

    ; PC BIOS boot signature
    db 0x55, 0xaa

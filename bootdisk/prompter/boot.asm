; Copyright (c) 2025 Simon Howard
;
; Permission to use, copy, modify, and/or distribute this software
; for any purpose with or without fee is hereby granted, provided
; that the above copyright notice and this permission notice appear
; in all copies.
;
; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
; WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
; AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
; CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
; LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
; NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
; CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


; A minimal PC bootloader that prompts the user to press a key to
; boot from CDs, otherwise boots from hard drive.

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

    mov bx, boot_prompt_msg
    call print

    mov cx, 54            ; 3 seconds
    mov di, 18

wait_loop:
    push cx
    push di
    call check_enter_pressed
    jz enter_pressed

    call sleep_one_tick

    pop di
    dec di
    cmp di, 0
    jne wait_loop_repeat

    mov di, 18
    mov al, '.'
    call print_char

wait_loop_repeat:
    pop cx
    loop wait_loop

    ; Enter wasn't pressed, so proceed to boot from the hard drive instead
    mov bx, booting_from_hdd_msg
    call print

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
    ; Load the real boot sector from disk:
    mov ch, [real_track]
    mov cl, [real_sector]
    mov dh, [real_side]
    mov dl, 0
    mov bx, entrypoint
    call load_sector
    mov dx, 0                ; bootloader expects dx=drive number

    jmp chainload

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

boot_prompt_msg:
    db "Press enter to boot from CD.", 0
booting_from_hdd_msg:
    db " booting from hard drive.", 13, 10, 13, 10, 0
error_msg:
    db 13, 10, "Error reading from disk. Press a key to reboot."
newline_msg:
    db 13, 10, 0

    times 0x1fb - ($ - $$) db 0

    ; Location of real boot sector. Set by the install script.
real_track:
    db 79
real_side:
    db 1
real_sector:
    db 18

    ; PC BIOS boot signature
    db 0x55, 0xaa

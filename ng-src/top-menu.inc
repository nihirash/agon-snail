;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

;; Top menu part

menu.buttons.count:   EQU     6
menu.selected_button: db      0


menu.buttons_list:
    db "[Back]", 0
    db "[Home page]", 0
    db "[Reload]", 0
    db "[Open location]", 0
    db "[Save buffer]", 0
    db "[Quit]", 0

menu.functions:
    dl menu.back
    dl menu.home
    dl menu.reload
    dl menu.open_location
    dl menu.save_buffer
    dl menu.quit

menu.draw_buttons:
    ld bc, $0300
    call vdp.goto_xy
    call menu.draw_buttons.real
    call ui.clean_line

    ld b, menu.buttons.count
    ld a, (menu.selected_button)
    ld c, a
    ld a, menu.buttons.count + 1
    sub c
    ld c, a

    ld hl, menu.buttons_list
@loop:
    push bc

    push hl
    call menu.draw_buttons.real
    
    ld a, 32
    call putc

    pop hl
    pop bc
    push bc
    push hl

    ld a, b
    cp c
    push af
    call z, menu.draw_buttons.inv
    pop af
    call nz, menu.draw_buttons.real

    pop hl
    call menu.printHL

    pop bc
    djnz @loop


    ret

menu.printHL:
	ld a, (hl)
	inc hl
	and a
	ret z
	push hl
	call putc
	pop hl
	jr menu.printHL

menu.draw_buttons.inv:
    SEND_VDP_CMD
    ret
@cmd:
    VDP_SET_COLOR $84
    VDP_SET_COLOR $0F
@end:

menu.draw_buttons.real:
    SEND_VDP_CMD
    ret
@cmd:
    VDP_SET_COLOR $87
    VDP_SET_COLOR $04
@end:

menu.open:
    ld a, 1
    ld (menu.selected_button), a
menu.loop:
    call menu.draw_buttons
    MOSCALL mos_getkey

    cp KEY_ESC
    jp z, menu.exit

    cp KEY_RIGHT
    jr z, menu.right

    cp KEY_LEFT
    jr z, menu.left

    cp KEY_RET
    jr z, menu.item

    jr menu.loop


menu.right:
    ld a, (menu.selected_button)
    cp menu.buttons.count
    jr nc, menu.loop

    inc a
    ld (menu.selected_button), a
    jr menu.loop

menu.left:
    ld a, (menu.selected_button)
    or a
    jr z, menu.loop

    dec a
    ld (menu.selected_button), a
    jr menu.loop

menu.item:
    ld a, (menu.selected_button)
    dec a
    ld l, a
    ld h, 3
    mlt hl
    
    ld de, menu.functions
    add hl, de

    ld hl, (hl)
    jp (hl)

menu.back:
menu.home:
menu.reload:
menu.open_location:
menu.save_buffer:
    ld hl, @msg
    ld a, ui.msgbox.INFO
    call ui.msgbox
    MOSCALL mos_getkey
    
    ld hl, (process_function)
    jp (hl)
    
@msg:
    db "Generic menu item handler", 0

menu.exit:
    ld hl, (process_function)
    jp (hl)

menu.quit:
    ret
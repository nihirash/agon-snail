;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

text_page.process:
    ld hl, text_page.process
    ld (url.processor), hl
    call text_page.render
text_page.process.keys:
    MOSCALL mos_getkey

    cp KEY_ESC
    jp z, menu.open

    cp KEY_UP
    jr z, text_page.up

    cp KEY_DOWN
    jr z, text_page.down

    jr text_page.process.keys

text_page.up:
    ld hl, (gopher_page.offset)
    ld a, l
    or h
    jr z, text_page.process.keys

    ld de, gopher_page.PER_PAGE
    sbc hl, de 
    ld (gopher_page.offset), hl
    jr text_page.process

text_page.down:
    ld a, (gopher_page.last_page)
    or a
    jr nz, text_page.process.keys
    
    ld hl, (gopher_page.offset)
    ld de, gopher_page.PER_PAGE
    add hl, de
    ld (gopher_page.offset), hl

    jr text_page.process

text_page.render:
    call ui.draw_header
    xor a
;; Reuse vars
    ld (gopher_page.last_page), a
    
    SEND_VDP_CMD

    ld bc, (gopher_page.offset)
    call gopher_page.find_line
    ld b, gopher_page.PER_PAGE
@loop:
    push bc
    ld a, l
    or h 
    jr z, @last_page
    push hl
    call ui.clean_line
    pop hl

    ld b, 79
@print_loop:
    ld a, (hl)

    or a
    jr z, @done_print

    inc hl

    cp 13
    jr z, @find_eol

    cp 10
    jr z, @done_print

    cp 32
    jr c, @skip_char

    push bc
    push hl
    call putc
    pop hl
    pop bc

    dec b
@skip_char:
    ld a, b
    or a
    jr nz, @print_loop
    jr @find_eol
@done_print:
    ld a, (hl)
    or a
    jr z, @force_end

    call gopher_page.crlf

    pop bc
    djnz @loop
    xor a
    ld (gopher_page.last_page), a
    ret

@force_end:
    pop bc
@last_page:
    inc a
    ld (gopher_page.last_page), a
    ret
@cmd:
    VDP_SET_COLOR $80
    VDP_SET_COLOR $F
    VDP_GOTO_XY 00, 04
@end:
@find_eol:
    ld a, (hl)
    or a 
    jr z, @done_print
    
    inc hl
    cp 10
    jr @done_print

    cp 13
    jr nz, @find_eol

    jr @done_print
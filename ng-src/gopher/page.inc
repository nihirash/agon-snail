;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

gopher_page.PER_PAGE:   equ 54
gopher_page.PADDING:    equ 5

gopher_page.UNKNOWN:    equ 07
gopher_page.INFO:       equ 15
gopher_page.LINK:       equ 10
gopher_page.TEXT:       equ 02
gopher_page.DOWN:       equ 09
gopher_page.INPUT:      equ 14

gopher_page.offset:
    dl 0
gopher_page.cursor:
    db 0
gopher_page.on_screen:
    db 0
gopher_page.last_page:
    db 0

gopher_page.process:
    ld hl, gopher_page.process
    ld (url.processor), hl
    call gopher_page.render
gopher_page.process.keys:
    MOSCALL mos_getkey

    cp KEY_RET
    jp z, gopher_page.navigate

    cp KEY_ESC
    jp z, menu.open

    cp KEY_DOWN
    jp z, gopher_page.cursor_down

    cp KEY_UP
    jp z, gopher_page.cursor_up

    cp KEY_LEFT
    jp z, gopher_page.scroll_up

    cp KEY_RIGHT
    jp z, gopher_page.scroll_down

    jr gopher_page.process.keys

gopher_page.cursor_up:
    ld a, (gopher_page.cursor)
    or a
    jr z, gopher_page.scroll_up

    call gopher_page.hide_cursor

    ld a, (gopher_page.cursor)
    dec a
    ld (gopher_page.cursor), a

    call gopher_page.show_cursor
    jp gopher_page.process.keys

gopher_page.cursor_down:
    call gopher_page.hide_cursor

    ld a, (gopher_page.cursor)
    inc a
    cp gopher_page.PER_PAGE 
    jr z, gopher_page.scroll_down

    ld hl, gopher_page.on_screen
    cp (hl)
    jp z, gopher_page.cursor_down.skip

    ld (gopher_page.cursor), a
gopher_page.cursor_down.skip:
    call gopher_page.show_cursor

    jp gopher_page.process.keys

gopher_page.scroll_down:
    ld a, (gopher_page.last_page)
    or a
    jp nz, gopher_page.process

    xor a
    ld (gopher_page.cursor), a

    ld de, gopher_page.PER_PAGE
    ld hl, (gopher_page.offset)
    add hl, de
    ld (gopher_page.offset), hl

    jp gopher_page.process

gopher_page.scroll_up:
    ld hl, (gopher_page.offset)
    
    ld a, l
    or h
    jp z, gopher_page.cursor_down.skip

    ld de, gopher_page.PER_PAGE
    or a
    sbc hl, de
    ld (gopher_page.offset), hl

    ld a, gopher_page.PER_PAGE - 1
    ld (gopher_page.cursor), a

    jp gopher_page.process

gopher_page.hide_cursor:
    ld a, (gopher_page.cursor)
    
    add a, 4
    ld b, a
    ld c, 0

    call vdp.goto_xy
    call gopher_page.usual_row
    call gopher_page.find_under_cursor
    push hl
    jp gopher_page.render_row.clean

gopher_page.show_cursor:    
    ld a, (gopher_page.cursor)
    
    add a, 4
    ld b, a
    ld c, 0
    call vdp.goto_xy
    
    call gopher_page.active_row
    call gopher_page.find_under_cursor
    push hl
    jp gopher_page.render_row.clean


;; Usual gopher page rendering just from start of page

gopher_page.render:
;; Looking for first link
    ld bc, (gopher_page.offset)
    call gopher_page.find_line
;; Since we once found where page starts - we can remember it and don't search it each time
;; or even each line(like before)
    ld (gopher_page.render.page_start), hl

    ld a, (hl)
    or a 
    jp z, @do_render

    ld a, (gopher_page.cursor)
    or a
    jp nz, @do_render

    ld b, gopher_page.PER_PAGE
@search_link_loop:
    ld a, (hl)
    or a
    jp z, @do_render

    call gopher_page.get_type
    ex af, af'

    call gopher_page.printRow.find_eol

@do_check:
    ex af, af'
    cp gopher_page.UNKNOWN
    jr z, @next

    cp gopher_page.INFO
    jr z, @next

    ld a, gopher_page.PER_PAGE
    sub b
    ld (gopher_page.cursor), a

    jr @do_render

@next:
    djnz @search_link_loop
@do_render:

    call ui.draw_header

    xor a
    ld (gopher_page.last_page), a
    ld (gopher_page.on_screen), a

    ld bc, $0400
    call vdp.goto_xy

gopher_page.render.page_start:    equ $ + 1
    ld hl, $123456

    ld a, l
    or h
    jr z, @last_page

    ld b, gopher_page.PER_PAGE
@loop:
    push bc
    call gopher_page.render_row
    call gopher_page.crlf
    ld a, (hl)
    or a
    jr z, @render_force_end
    pop bc
    djnz @loop
    ret
@render_force_end:
    pop bc
@last_page:
    inc a
    ld (gopher_page.last_page), a
    ret

gopher_page.crlf:
    push af
    push hl
    
    ld hl, gopher_page.on_screen
    inc (hl)

    SEND_VDP_CMD
    pop hl
    pop af
    ret
@cmd:
    db 13, 10
@end:

gopher_page.render_row:
    push hl

    ld a, (gopher_page.cursor)
    ld b, a
    ld a, (gopher_page.on_screen)
    cp b
    
    push af
    call z, gopher_page.active_row
    pop af
    call nz, gopher_page.usual_row

gopher_page.render_row.clean:
    call ui.clean_line
    
    ld hl, @offset
    ld bc, gopher_page.PADDING
    rst.lil $18
    
    pop hl

    ld a, (hl)
    inc hl

    call gopher_page.get_type
    call vdp.set_color

    jp gopher_page.printRow
@offset:
    blkb gopher_page.PADDING, 32

gopher_page.printRow:
    ld b, 70
@loop:
    ld a, (hl)
    and a
    ret z
    
    inc hl
    cp 13
    jp z, gopher_page.printRow.check_eol

    cp 10
    ret z

    cp 09
    jp z, gopher_page.printRow.find_eol

    cp 32
    jr c, @skip

    push bc
    push hl
    call putc
    pop hl
    pop bc

    dec b
@skip:
    ld a, b
    or a 
    jp z, gopher_page.printRow.find_eol
    jr @loop
gopher_page.printRow.check_eol:
    ld a, (hl)
    cp 10
    ret nz 
    inc hl
    ret
gopher_page.printRow.find_eol:
    ld a, (hl)
    or a 
    ret z

    inc hl

    cp 13
    jr z, gopher_page.printRow.check_eol

    cp 10
    ret z
    jr gopher_page.printRow.find_eol


;; A - type character of gopher page
gopher_page.get_type:
    cp 'i'
    jr z, @info

    cp '7'
    jr z, @input

    cp '1'
    jr z, @link

    cp '0'
    jr z, @text

    cp '9'
    jr z, @down

    cp 'I'
    jr z, @down

    cp ';'
    jr z, @down

    cp ':'
    jr z, @down

    cp '4'
    jr z, @down

    cp '5'
    jr z, @down

    ld a, gopher_page.UNKNOWN
    ret
@down:
    ld a, gopher_page.DOWN
    ret
@info:
    ld a, gopher_page.INFO
    ret
@input:
    ld a, gopher_page.INPUT
    ret
@link:
    ld a, gopher_page.LINK
    ret
@text:
    ld a, gopher_page.TEXT
    ret

gopher_page.active_row:
    SEND_VDP_CMD
    ret
@cmd:
    VDP_SET_COLOR $84
    VDP_SET_COLOR $0F
@end:

gopher_page.usual_row:
    SEND_VDP_CMD
    ret
@cmd:
    VDP_SET_COLOR $80
    VDP_SET_COLOR $0F
@end:

gopher_page.find_under_cursor:
    ld hl, (gopher_page.render.page_start)
    
    ld a, (gopher_page.cursor)
    or a
    ret z
    ld b, a
@loop:
    call gopher_page.printRow.find_eol
    djnz @loop
    ret

gopher_page.find_line:
    ld hl, page_buffer

    ld a, b
    or c
    jr z, @check_empty
@loop:
    ld a, (hl)
    or a
    jr z, @empty

    call gopher_page.printRow.find_eol

    dec bc
    ld a, b
    or c
    jr nz, @loop
    ret
@check_empty:
    ld a, (hl)
    and a
    ret nz
@empty:
    ld hl, 0
    ret
;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

;; ------------------------------------------------------------
;; MessageBox part
;; ------------------------------------------------------------

ui.msgbox.INFO:     equ $ff
ui.msgbox.ERROR:    equ $fe
ui.msgbox.INPUT:    equ $fd
ui.msgbox.DOWN:     equ $fc

;; HL - message
;; A - flags
;; DE - buffer ptr
;; BC - buffer len
ui.inputbox:
    ld (@flags), a

    ld a, ui.msgbox.INPUT
    push de
    push bc
    call ui.msgbox
    SEND_VDP_CMD
    pop bc
    pop hl

@flags: equ $ + 1
    ld e, 0

    MOSCALL mos_editline
    push af
    call vdp.hide_cursor
    pop af
    ret
@cmd:
    VDP_GOTO_XY 15, 28
    VDP_CURSOR 1
@end:

;; HL - message
;; A - Icon
ui.msgbox:
    ld (@icon), a
    push hl
    SEND_VDP_CMD
    pop hl
    ld bc, 0
    xor a
    rst.lil $18
    ret
@cmd:
    VDP_SET_COLOR $84 ; Background
    VDP_SET_COLOR $0F ; Foreground
    
    VDP_GOTO_XY 8, 25
    blkb 64, 32       ; cleaning up with spaces
    
    VDP_GOTO_XY 8, 26
    blkb 64, 32       ; cleaning up with spaces

    VDP_GOTO_XY 8, 27
    blkb 64, 32       ; cleaning up with spaces

    VDP_GOTO_XY 8, 28
    blkb 64, 32       ; cleaning up with spaces

    VDP_GOTO_XY 8, 29
    blkb 64, 32       ; cleaning up with spaces

    VDP_GOTO_XY 8, 30
    blkb 64, 32       ; cleaning up with spaces
@icon:  equ $ + 3
    VDP_DRAW_BITMAP 72, 208, ui.msgbox.INFO

    VDP_GOTO_XY 15, 27

@end:

ui.image.info:
    incbin "binaries/info.agi"

ui.image.error:
    incbin "binaries/error.agi"

ui.image.input:
    incbin "binaries/input.agi"

ui.image.down:
    incbin "binaries/downloading.agi"


;; ------------------------------------------------------------
;; Basic UI part
;; ------------------------------------------------------------

ui.draw_header:
    SEND_VDP_CMD
    ld hl, url.rendered
    call printZ
    
    xor a
    ld (menu.selected_button), a

    call menu.draw_buttons
    ret
@cmd:
    VDP_SET_COLOR   $80 ;; BG
    VDP_SET_COLOR   $0F ;; FG
    VDP_CLS
    db 10
    VDP_SET_COLOR   $84
    VDP_SET_COLOR   $0B
    db " Snail - Gopher browser for Agon computers v.1.0 (c)"
    db " 2024 Aleksandr Sharikhin   "
    VDP_GOTO_XY 0, 58
    VDP_SET_COLOR   $8B
    VDP_SET_COLOR   $04
    db " Made just for fun and for good of all beings with great support of my friends! "
    VDP_SET_COLOR   $80
    VDP_SET_COLOR   $0F
    VDP_GOTO_XY 0, 2
    
    db " URL:["
    blkb 70, 32
    db "]   "
    VDP_GOTO_XY 07, 02
@end:


ui.clean_line:
    SEND_VDP_CMD
    ret
@cmd:
    db 13
    blkb 80, 32
    db 11
@end:
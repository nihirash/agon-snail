;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

ui.msgbox.INFO:     equ $ff
ui.msgbox.ERROR:    equ $fe
ui.msgbox.INPUT:    equ $fd
ui.msgbox.DOWN:     equ $fc

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
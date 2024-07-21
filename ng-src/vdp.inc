;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

    macro VDP_MODE n
    db 22, n
    endmacro

    macro VDP_CURSOR n
    db 23, 1, n
    endmacro
    

    macro VDP_DRAW_BITMAP x, y, n
    db 23, 27, $20
    dw n
    db 23, 27, 3
    dw x, y
    endmacro

    macro VDP_GOTO_XY x, y
    db 31, x, y
    endmacro

    macro VDP_SET_COLOR c
    db 17, c
    endmacro

    macro VDP_CLS
    db 12
    endmacro

    macro SEND_VDP_CMD
    ld hl, @cmd
    ld bc, @end-@cmd
    rst.lil $18
    endmacro

vdp.init:
    MOSCALL mos_sysvars
    ld  a, (ix + mosvar_vdp_mode)
    ld (vdp.free + 1), a
    ;; Loading images

    ; MsgBox INFO icon
    ld a, ui.msgbox.INFO
    ld hl, ui.image.info
    call agi.load_from_ram

    ; MsgBox ERROR icon
    ld a, ui.msgbox.ERROR
    ld hl, ui.image.error
    call agi.load_from_ram

    ; MsgBox INPUT icon
    ld a, ui.msgbox.INPUT
    ld hl, ui.image.input
    call agi.load_from_ram

    ; MsgBox DOWNLOADING icon
    ld a, ui.msgbox.DOWN
    ld hl, ui.image.down
    call agi.load_from_ram

vdp.reinit:
    SEND_VDP_CMD
    ret
@cmd:
    VDP_MODE   0  ;; 640x480
    VDP_CURSOR 0
@end:


;; Restoring videomode, restoring cursor and black and white attributes
vdp.free:
    ld a, 1
    ld (@cmd + 1), a

    SEND_VDP_CMD
    ret
@cmd:
    VDP_MODE 0
    VDP_SET_COLOR $80
    VDP_SET_COLOR $F
    VDP_CURSOR 1
    VDP_CLS
@end:
;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

show_splash:
    ld hl, splash_agi 
agi.show:
    call agi.center_image
    xor a
    call agi.load_from_ram
    jp nz, agi.image_error

    ld hl, agi.show_cmd
    ld bc, agi.show_end-agi.show_cmd
    rst.lil $18

    MOSCALL mos_getkey
    jp vdp.reinit
agi.show_cmd:
    VDP_MODE 8
    VDP_CURSOR 0 
    VDP_SET_COLOR 158
    VDP_SET_COLOR 15
    VDP_CLS
    db 23, 27, 3 ;; VDP DRAW BITMAP
agi.show_coords:
    dw 30, 30

    VDP_GOTO_XY 9, 29
    db "Press key to continue!"
agi.show_end:

;; Shows on error rendering
agi.image_error:
    call vdp.reinit

    ld a, ui.msgbox.ERROR
    ld hl, @txt
    call ui.msgbox
    MOSCALL mos_getkey
    ret
@txt:
    db "Image broken or format not supported!", 0


agi.center_image:
    push hl
    ld de, 4
    ld bc, 0
    add hl, de

    ld a, (hl)
    ld c, a
    inc hl

    ld a, (hl)
    ld b, a
    inc hl

    push hl

    sra b
    rr c

    ld hl, 160
    or a 
    sbc hl, bc

    ld a, l 
    ld (agi.show_coords), a
    ld a, h
    ld (agi.show_coords + 1), a

    pop hl

    ld a, (hl)
    ld c, a
    inc hl

    ld a, (hl)
    ld b, a

    sra b
    rr c

    or a
    ld hl, 120
    sbc hl, bc

    ld a, l
    ld (agi.show_coords + 2), a
    ld a, h
    ld (agi.show_coords + 3), a

    pop hl
    ret


splash_agi:
	incbin "binaries/splash.agi"
	
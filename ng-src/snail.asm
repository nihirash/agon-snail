;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

    include "init.inc"
    include "mos.inc"
    include "vdp.inc"
    include "ui.inc"
    include "agi-ram.inc"
    include "agi.inc"

_main:
    call vdp.init

    call show_splash

    ld hl, _main ;; NOT IMAGE :-)
    call agi.show
    
    ld a, ui.msgbox.INFO
    ld hl, test_msg
    call ui.msgbox
    MOSCALL mos_getkey

    ld a, ui.msgbox.INPUT
    ld hl, test_msg
    call ui.msgbox
    MOSCALL mos_getkey

    ld a, ui.msgbox.DOWN
    ld hl, test_msg
    call ui.msgbox
    MOSCALL mos_getkey


    call vdp.free
    ret

test_msg:
    db "Hello, world!", 0
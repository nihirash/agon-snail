;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

DEV:    equ     1

    include "init.inc"
    include "mos.inc"
    include "vdp.inc"
    include "ui.inc"
    include "top-menu.inc"
    include "agi-ram.inc"
    include "agi.inc"
    include "gopher/page.inc"
    include "gopher/row-handler.inc"
    include "gopher/binary-handler.inc"
    include "text/page.inc"
    include "url/parser.inc"
    include "transport/fetcher.inc"
    include "history/handler.inc"
_main:
    ld hl, __bss.start
    ld de, __bss.start + 1
    ld bc, __bss.end - __bss.start
    xor a
    ld (hl), a
    ldir

    call vdp.init
    
    ld hl, url.home
    call url.parse
    call url.render
    ld hl, gopher_page.process
    ld (url.processor), hl

    ld b, history.DEPTH
@history_fill:
    push bc
    call history.push
    pop bc
    djnz @history_fill

    .if DEV
    call dev_version
    .else
    call show_splash
    .endif

    call ui.draw_header
    call fetcher.get  
    call gopher_page.process

    call vdp.free
    ret

.if DEV
dev_version:
    ld hl, @msg
    ld a, ui.msgbox.INFO
    call ui.msgbox
    MOSCALL mos_getkey
    ret
@msg:
    db "This is development version!", 0
.endif

not_implemented:
    ld a, ui.msgbox.ERROR
    ld hl, @msg
    call ui.msgbox
    MOSCALL mos_getkey
    ret
@msg:
    db "NOT IMPLEMENTED", 0

__bss.start:

buffer_ends:
    dl      page_buffer

    include "history/buffers.inc"
page_buffer:

__bss.end:
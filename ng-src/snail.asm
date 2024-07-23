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
    include "url/parser.inc"
_main:
    call vdp.init

    ld hl, url.test
    call url.parse
    call url.render

    .if DEV
    call dev_version
    .else
    call show_splash
    .endif

    call ui.draw_header
    call buffer.home_page
    call gopher_page.init

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

process_function:
    dl 0

buffer.home_page:
    ld hl, start_page
    ld de, page_buffer
    ld bc, start_page.end - start_page
    ldir
    ld (buffer_ends), de
    ret

start_page:
    incbin "documents/test.gph"
    db 0
start_page.end:

buffer_ends:
    dl      page_buffer

    include "url/buffers.inc"
page_buffer:
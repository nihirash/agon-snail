;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

    include "transport/home.inc"
    include "transport/file.inc"
    include "transport/network.inc"

;; Gets document by 
fetcher.get:
    ld hl, @msg
    ld a, ui.msgbox.DOWN
    call ui.msgbox

    ld hl, 0
    ld (gopher_page.offset), hl
    ld (gopher_page.cursor), hl

;; Trying process special "domains" - for home page and for file
    ld hl, url.domain
    ld a, (hl) 
    ;; CAPSize
    and $df

    cp 'F'
    jr z, @try_file

    cp 'H'
    jr z, @try_home
@network:
    jp fetcher.network

@try_file:
    inc hl
    ld a, (hl)
    and $df
    cp 'I'
    jr nz, @network

    inc hl
    ld a, (hl)
    and $df
    cp 'L'
    jr nz, @network

    inc hl
    ld a, (hl)
    and $df
    cp 'E'
    jr nz, @network

    inc hl
    ld a, (hl)
    or a
    jr nz, @network

    jp fetcher.file

@try_home:
    inc hl
    ld a, (hl)
    and $df
    cp 'O'
    jr nz, @network

    inc hl
    ld a, (hl)
    and $df
    cp 'M'
    jr nz, @network

    inc hl
    ld a, (hl)
    and $df
    cp 'E'
    jr nz, @network

    inc hl
    ld a, (hl)
    or a
    jr nz, @network
    jp fetcher.home
@msg:
    db "Trying fetch document!", 0

fetcher.on_error:
    ld a, ui.msgbox.ERROR
    ld hl, @cmd
    call ui.msgbox
    MOSCALL mos_getkey

    call history.pull
    jp fetcher.get
@cmd:
    db "Document fetch error!"
    db 0
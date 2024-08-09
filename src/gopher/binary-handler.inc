;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

binary_handler:
    call gopher_page.find_filename
    
    ld ix, agi_ext
    call  binary_handler.check_extension
    jp z, binary_handler.process_agi

;; Download handler
    ld de, url.filename
    ld bc, 4096
    ld hl, @msg
    ld a, 0
    call ui.inputbox
    
    cp 27
    jr z, @exit

    ld c, 0
    MOSCALL mos_fclose

    ld hl, url.filename
    ld c, fa_cr_always + fa_write
    MOSCALL mos_fopen
    or a
    jp z, binary_handler.on_error
    ld c, a

    ld hl, (buffer_ends)
    ld de, page_buffer
    
    or a
    sbc hl, de
    ex de, hl
    MOSCALL mos_fwrite

    ld c, 0
    MOSCALL mos_fclose
@exit:
    jp menu.back
@msg:
    db "File name:", 0

binary_handler.on_error:
    ld c, 0
    MOSCALL mos_fclose

    ld a, ui.msgbox.ERROR
    ld hl, @cmd
    call ui.msgbox
    MOSCALL mos_getkey

    jp menu.back
@cmd:
    db "File saving error!"
    db 0

agi_ext:
    db "AGI"

binary_handler.process_agi:
    ld hl, page_buffer
    call agi.show
    jp menu.back

binary_handler.check_extension:
    ld hl, url.filename
    xor a
    ld bc, 4096
    cpir 
    dec hl
    dec hl
;; Last letter
    ld a, (hl)
    and $df
    cp (ix + 2)
    ret nz

    dec hl
;; Middle letter
    ld a, (hl)
    and $df
    cp (ix + 1)
    ret nz

    dec hl
;; First letter
    ld a, (hl)
    and $df
    cp (ix + 0)
    ret nz
    ret
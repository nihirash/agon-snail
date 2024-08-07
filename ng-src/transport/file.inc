;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

fetcher.file:
    ld c, 0
    MOSCALL mos_fclose

    ld hl, url.path + 1
    ld c, fa_read
    MOSCALL mos_fopen
    
    or a
    jr z, @on_error
    
    ld c, a
    ld hl, page_buffer
    ld de, $20000 
    MOSCALL mos_fread
    
    ld hl, page_buffer
    add hl, de
    ld (buffer_ends), hl
    
    inc de
    xor a
    ld (de), a


    ld c, 0
    MOSCALL mos_fclose
    call url.render
    jp history.push
    

@on_error:
    call url.render
    call ui.draw_header
    ld c, 0
    MOSCALL mos_fclose
    jp fetcher.on_error
;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

url.home:
    db "home:/", 0

fetcher.home:
    ld hl, fetcher.home.d
    ld bc, fetcher.home.d.end-fetcher.home.d
    ld de, page_buffer
    ldir
    
    ld hl, page_buffer + fetcher.home.d.end-fetcher.home.d
    ld (buffer_ends), hl
    
    call url.render
    jp history.push

fetcher.home.d:
    incbin "documents/home.gph"
    dl 0
fetcher.home.d.end:


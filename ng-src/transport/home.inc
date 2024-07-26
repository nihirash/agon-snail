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
    
    ld hl, fetcher.home.d.end-fetcher.home.d
    ld (buffer_ends), hl
    ret

fetcher.home.d:
    incbin "documents/test.gph"
    db 0
fetcher.home.d.end:


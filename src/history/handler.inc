;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

history.push:
    ld de, history.end - 1
    ld hl, history.past_element - 1
    ld bc, history.DEPTH*url.BUFFER_SIZE
    lddr
    ret

history.pull:
    ld de, url.domain
    ld hl, history.buffer
    ld bc, history.DEPTH-1*url.BUFFER_SIZE
    ldir
    
    or 1
    ret
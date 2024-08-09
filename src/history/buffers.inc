;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

history.DEPTH:      equ     16

history.level:
    ds 1
    
url.filename:
    ds 4096
    
url.rendered:
    ds 68

url.domain:
    ds 32
url.port:
    ds 6
url.path:
    ds 4096
url.processor:
    ds 3
url.BUFFER_SIZE:    equ     $ - url.domain

history.buffer:
    ds history.DEPTH * url.BUFFER_SIZE
history.end:            equ $
history.past_element:   equ history.end - url.BUFFER_SIZE
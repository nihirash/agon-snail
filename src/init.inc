;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

;; This file is entrypoint
    ASSUME ADL=1
    org $40000
    jp _init

args:
    dl 0

    align 64
    db "MOS"
    db 0
    db 1

;; HL - arg string
_init:
    push ix
    push iy
@args_load:
    ld a, (hl)
    and a
    jr z, @finish_args
    cp ' '
    jr nz, @finish_args
    inc hl
    jr @args_load

@finish_args:
    ld (args), hl

    call _main
    pop iy
    pop ix
    ret


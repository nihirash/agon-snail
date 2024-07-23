;; HL - URL
url.parse:
    push hl
    ld hl, url.default.port
    ld de, url.port
    ldi
    ldi
    ldi
    
    ld de, url.path
    ldi
    ldi
    pop hl

    ld de, url.domain
    ld b, 32
url.domain_parse:
    ld a, (hl)
    ld (de), a
    and a
    ret z
    
    inc hl

    cp ':'
    jr z, url.port_parse

    cp '/'
    jr z, url.path_parse

    
    inc de
    
    dec b
    jp z, url.on_error

    jr url.domain_parse
url.port_parse:
    xor a
    ld (de), a
    ld de, url.port
    ld b, 5
url.port_parse.loop:
    ld a, (hl)
    and a 
    ret z

    inc hl

    cp '/'
    jr z, url.path_parse

    ld (de), a
    inc de
    djnz url.port_parse.loop
    jr url.on_error
url.path_parse:
    xor a
    ld (de), a

    ld de, url.path + 1
    ld b, $ff
url.path_parse.loop:
    ld a, (hl)
    ld (de), a
    inc hl
    inc de
    
    and a
    ret z
    djnz url.path_parse.loop

url.on_error:
    ld hl, @msg
    ld a, ui.msgbox.ERROR
    call ui.msgbox
    MOSCALL mos_getkey
    ret
@msg:
    db "Can't parse URL! Possibly one of the part is too long!", 0

url.render:
    ld b, 65
    ld hl, url.domain
    ld de, url.rendered
@render_domain:
    ld a, (hl)
    and a
    jr z, @render_port
    ld (de), a
    inc hl
    inc de
    djnz @render_domain
@render_dots:
    ld a, '.'
    ld (de), a
    inc de
    ld (de), a
    xor a
    inc de
    ld (de), a
    ret
@render_port:
    ld a, ':'
    ld (de), a
    inc de
    ld hl, url.port
@port_loop:
    ld a, (hl)
    ld (de), a
    and a
    jr z, @render_path
    inc hl
    inc de
    djnz @port_loop
    jr @render_dots
@render_path:
    ld hl, url.path
@path_loop:
    ld a, (hl)
    ld (de), a
    and a
    ret z

    cp 9    ;; Search requests
    jr z, @finish

    inc hl
    inc de
    djnz @path_loop
    jr @render_dots
@finish:
    xor a
    ld (de), a
    ret

url.test:
    db "home-page", 0

url.default.port:
    db "70", 0

url.default.path:
    db "/", 0
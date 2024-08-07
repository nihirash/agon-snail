;; Snail - gopher browser for Agon's MOS
;; (c) 2023-2024 Aleksandr Sharikhin
;;
;; All rights are reserved

gopher_page.navigate:
    call gopher_page.find_under_cursor
    ld a, (hl)
    call gopher_page.get_type
    
    cp gopher_page.INFO
    jr z, @exit

    cp gopher_page.UNKNOWN
    jr z, @exit

    jr gopher_page.extract_row_data

@exit:
;; No rerender even
    jp gopher_page.process.keys

;; Extracts path, domain and port to URL structure
gopher_page.extract_row_data:
    ld (@data_type), a
@search_loop:

    ld a, (hl)

    or a
    jp z, @oops
    
    cp 13
    jp z, @oops

    inc hl
    
    cp 09
    jr z, @found

    jr @search_loop

@found:
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

    ld de, url.path + 1

    ld a, (hl)
    cp '/'
    jr nz, @copy_url
    inc hl
@copy_url:
    ld a, (hl)
    inc hl
    
    cp 09
    jr z, @copy_url_done

    cp 13
    jr z, @oops

    or a
    jr z, @oops

    ld (de), a
    inc de

    jr @copy_url

@copy_url_done:
    xor a
    ld (de), a

    ld de, url.domain
@copy_domain:
    ld a, (hl)
    inc hl

    cp 09
    jr z, @copy_domain_done

    cp 13
    jr z, @oops

    or a 
    jr z, @oops

    ld (de), a
    inc de
    jr @copy_domain
@copy_domain_done:
    xor a
    ld (de), a

    ld de, url.port
@copy_port:
    ld a, (hl)
    inc hl

    ld (de), a

    cp 09
    jr z, @copy_port_done

    cp 10
    jr z, @copy_port_done

    cp 13
    jr z, @copy_port_done

    or a
    jr z, @copy_port_done

    inc de
    jr @copy_port

@copy_port_done:
    xor a
    ld (de), a

    call fetcher.get

@data_type:  equ $ + 1
    ld a, 0
    
    ld hl, gopher_page.process
    cp gopher_page.LINK
    jr z, @exit

    cp gopher_page.DOWN
    jp z, binary_handler

    call not_implemented

    ld hl, menu.quit
@exit:
    ld (url.processor), hl
    jp menu.exit


@oops:
    call history.pull
    call history.push
    jp gopher_page.process.keys

gopher_page.find_filename:
    ld hl, url.path
    ld de, url.path
@loop:
    ld a, (de)
    inc de
    cp '/'
    jr nz, @skip
    push de
    pop hl
@skip:
    and a
    jr z, @copy

    cp '?'
    jr z, @copy

    cp 13
    jr z, @copy

    jr @loop
@copy:
    ld de, url.filename
@copy_loop:
    ld a, (hl)
    and a
    jr z, @done

    cp 13
    jr z, @done

    cp '?'
    jr z, @done

    ldi
    jr @copy_loop
@done:
    xor a
    ld (de), a
    ret

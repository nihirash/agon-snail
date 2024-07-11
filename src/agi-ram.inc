;; Agon Image Format loader(from RAM)
;; ========================
;; (c) 2024 Aleksandr Sharikhin
;;
;; Use for good of all beings

;; Extracts image from ram to VDP buffer
;; A - buffer number
;; HL - pointer to agon image
agon_image_load_from_ram:
;; Store buffer ID to vars
    ld (@buff), a
    ld (@buff1), a
    ld (@bmid), a
    ld (@decompress_buff), a
    ld (@source_buff), a

;; check header
    ld a, (hl)
    cp 'I'
    ret nz

    inc hl
    ld a, (hl)
    cp 'M'
    ret nz

    inc hl

;; Copy raw data lenght to command
    ld a, (hl)
    ld (@len), a
    ld (@notpacked+1), a
    inc hl
    ld a, (hl)
    ld (@len + 1), a
    ld (@notpacked+2), a

;; Send command that will await bytes of RGB2222 image
    push hl
    ld hl, @cmd1
    ld bc, @cmd1_end - @cmd1
    rst.lil $18
    pop hl

;; Storing pixel size of image(Width/height)
    ld de, @iw
    inc hl
    ld a, (hl)
    ld (de), a
    inc hl
    inc de
    ld a, (hl)
    ld (de), a
    inc hl
    inc de
    ld a, (hl)
    ld (de), a
    inc hl
    inc de
    ld a, (hl)
    ld (de), a
    inc hl

;; Is our image RLE encoded?
    ld a, (hl)
    inc hl
    and a
    jp z, @notpacked ; 0
    ;; Packed
    
    cp 2 ; TurboVega
    jp z, @turbo

;; Packed words count
    ld a, (hl)
    ld c, a
    inc hl
    ld a, (hl)
    ld b, a
    inc hl
    
@unpack_loop:
    push bc
;; How many copies of byte
    ld a, (hl)
    ld b, a

    inc hl
;; Byte itself
    ld a, (hl)
    inc hl

    push hl
;; Sending them to VDP
@send_loop:
    rst.lil $10
    djnz @send_loop
    pop hl
    pop bc
    
    dec bc
    
    ld a, b
    or c 
    jr nz, @unpack_loop
    
    jr @uploaded

;; If data wasn't RLE encoded
@notpacked:
    ld bc, 0
    rst.lil $18

@uploaded:
    ld hl, @cmd2
    ld bc, @cmd2_end - @cmd2
    rst.lil $18

    xor a
    ret

@turbo:
    ld bc, (@notpacked + 1)
    rst.lil $18

    ld hl, @unpack_cmd
    ld bc, @unpack_end - @unpack_cmd
    rst.lil $18
    jr @uploaded


@cmd1:
    ;; Clean buffer
    db 23, 0, $A0
@buff:
    db 0
    db 0
    db 2
    ;; Load to buffer
    db 23, 0, $A0
@buff1:
    db 0
    db 0
    db 0
@len:
    dw 0
@cmd1_end:

@cmd2:
    ;; Select image by buffer
    db 23, 27, $20
@bmid:
    dw 0
    ;; Make image from buffer
    db 23, 27, $21
@iw:
    dw 0
@ih:
    dw 0
    db 1
@cmd2_end:

@unpack_cmd:
    db 23, 0, $A0
@decompress_buff:
    dw 0
    db 65
@source_buff:
    dw 0
@unpack_end:
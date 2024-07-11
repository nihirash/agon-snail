	include "agi-ram.inc"

show_splash:
	ld	hl, splash_agi
show_agi:
	call	agi_center_image
	xor 	a
	call	agon_image_load_from_ram
	jp	nz, image_error

	ld 	hl, show_agi_cmd
	ld	bc, show_agi_end-show_agi_cmd
	rst.lil	$18

	MOSCALL	mos_getkey
	jp	vdp_init
show_agi_cmd:
	db 22, 8
	db 23, 1, 0
	db 17, 158
	db 12
	db 23, 27, 3
show_agi_coords:
	dw 30, 30

	db 31, 9, 29
	db "Press key to continue!"
show_agi_end:


image_error:
	call vdp_init
	call msg_box
	ld hl, @txt
	call printZ
	MOSCALL mos_getkey
	ret
@txt:
	db "Image broken or format not supported!"
	db 0

agi_center_image:
	push hl
	ld de, 4
	ld bc, 0
	add hl, de

	ld a, (hl)
	ld c, a
	inc hl

	ld a, (hl)
	ld b, a
	inc hl

	push hl

	sra b
	rr  c
	
	ld hl, 160

	or a
	sbc hl, bc

	ld a, l
	ld (show_agi_coords+0), a
	ld a, h
	ld (show_agi_coords+1), a

	pop hl
	ld a, (hl)
	ld c, a
	inc hl

	ld a, (hl)
	ld b, a
	
	sra b
	rr c

	or a
	ld hl, 120
	sbc hl, bc

	ld a, l
	ld (show_agi_coords+2), a
	ld a, h
	ld (show_agi_coords+3), a

	pop hl
	ret


splash_agi:
	incbin "splash.agi"
	

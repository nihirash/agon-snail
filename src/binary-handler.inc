process_binary:
	call find_filename

	ld ix, agi_ext
	call check_extension
	jp z, process_agi

	jp gopher_download

agi_ext:
	db "AGI"

;; Need for handling binary files
;; DE - REVERSED extension
check_extension:
	ld hl, down_filename
	xor a
	ld bc, 60
	cpir
	dec hl
	dec hl

;; LAST LETTER
	ld a, (hl)
	and $df
	cp (ix + 2)
	ret nz

	dec hl
;; MIDDLE LETTER	
	ld a, (hl)
	and $df
	cp (ix + 1)
	ret nz

	dec hl
;; FIRST LETTER
	ld a, (hl)
	and $df
	cp (ix)
	ret

process_agi:
	call gopher_load
	ld   hl, page_buffer
	jp   show_agi



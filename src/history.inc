HISTORY_DEPTH: EQU 4

history_push:
	ld hl, history_act
	inc (hl)

	ld de, history_end - 1
	ld hl, history_preend - 1
	ld bc, 4167*HISTORY_DEPTH
	lddr
	ret

history_pull:
	ld a, (history_act)
	or a
	ret z
	dec a
	ret z
	ld (history_act), a

	ld de, row_type
	ld hl, history_buffer
	ld bc, HISTORY_DEPTH-1*4167
	ldir

	or 1
	ret

history_act:
	db 0

row_type:
	db 0 ;; 0 for gopher, 1 for plain text
host:
	blkb 64, 0
str_port:
	blkb 6, 0
request:
	ds 4096

history_buffer:
	ds HISTORY_DEPTH-1*4167
history_preend:
	ds 4167
history_end:



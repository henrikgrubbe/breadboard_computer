	 ldi 1
	 sta a
out: sta b
	 out
	 add b
	 sta tmp
	 lda a
	 sta b
	 lda tmp
	 sta a
	 jmp out

tmp: .byte 0
a:   .byte 1
b:   .byte 0
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
	 nop
	 nop
tmp: .byte 12
a:   .byte 8
b:   .byte 28


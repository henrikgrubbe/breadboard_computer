	lda a
	sub c
	jc lbl
	lda d
	out
	hlt
lbl:
	sta a
	lda b
	add d
	sta d
	jmp 0
d: 	.byte 0
a: 	.byte 12
b: 	.byte 10
c: 	.byte 1

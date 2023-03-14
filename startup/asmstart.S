
#include <avr/io.h>

.extern __vector_default
.extern __vector_10
.extern __vector_14
.extern __vector_18
.extern __vector_19

.extern main

;# .vectors:
vectors:
	jmp cstart ;# reset vector
	jmp __vector_default ;# 1
	jmp __vector_default ;# 2
	jmp __vector_default ;# 3
	jmp __vector_default ;# 4
	jmp __vector_default ;# 5
	jmp __vector_default ;# 6
	jmp __vector_default ;# 7
	jmp __vector_default ;# 8
	jmp __vector_default ;# 9
	jmp __vector_10 ;# 10
	jmp __vector_default ;# 11
	jmp __vector_default ;# 12
	jmp __vector_default ;# 13
	jmp __vector_14 ;# 14
	jmp __vector_default ;# 15
	jmp __vector_default ;# 16
	jmp __vector_default ;# 17
	jmp __vector_18 ;# 18
	jmp __vector_19 ;# 19
	jmp __vector_default ;# 20
	jmp __vector_default ;# 21
	jmp __vector_default ;# 22
	jmp __vector_default ;# 23
	jmp __vector_default ;# 24
	jmp __vector_default ;# 25
	
; SREG notes:
;	I interrupt
;	T bit copy storage, for BLD (loads bit in reg into T) and BST (stores T into bit in reg)
;	H half carry flag
;	S sign bit (s = n xor v)
;	V twos complement overflow
;	N negative
;	Z zero
;	C carry

;# *(.ctors)
;# .init0 Start here after reset.
;# .init1
;# .init2 Clear __zero_reg__, set up stack pointer.

	.section .init0,"ax",@progbits
	.weak	__init
;	.func	__init

	.set	__stack, RAMEND

__init:

;#	.init2	Clear __zero_reg__, set up stack pointer
	.section .init2,"ax",@progbits
__ctors_end:
	clr __zero_reg__		;# ?? clr r1 eor	r1, r1
	out	AVR_STATUS_ADDR, __zero_reg__	;# store status register (is 0x3F)
	ldi	YL, lo8(__stack)	;# sets reg Y == 0x08FF
	ldi	YH, hi8(__stack)	;# but why? end of data, stack grows "down"
	out	AVR_STACK_POINTER_HI_ADDR, YH	;# set stack pointer
	out	AVR_STACK_POINTER_LO_ADDR, YL	;# sp is at 0x3D (L) and 0x3E (H)

;#	.init4	Initialize data and BSS.
	.section .init4,"ax",@progbits
	.global __do_copy_data
__do_copy_data:
	ldi		r17, hi8(__data_end)	;#
	ldi		XL, lo8(__data_start)	;#
	ldi		XH, hi8(__data_start)	;#
	ldi		ZL, lo8(__data_load_start)	;#
	ldi		ZH, hi8(__data_load_start)	;#
	rjmp	.do_copy_data_start		;#

.do_copy_data_loop:
	lpm		__tmp_reg__, Z+			;#
	st		X+, __tmp_reg__			;#

.do_copy_data_start:
	cpi		XL, lo8(__data_end)		;#
	cpc		XH, r17					;#
	brne	.do_copy_data_loop		;#

do_clear_bss:
	ldi		r17, hi8(__bss_end)		;#
	ldi		XL, lo8(__bss_start)	;#
	ldi		XH, hi8(__bss_start)	;#
	rjmp	.do_clear_bss_start		;#

.do_clear_bss_loop:
	st		X+, __zero_reg__		;#

.do_clear_bss_start:
	cpi		XL, lo8(__bss_end)		;#
	cpc 	XH, r17					;#
	brne	.do_clear_bss_loop		;#

;#	.init9	Call main().
	.section .init9,"ax",@progbits

	call	main					;#
	jmp		exit					;#
;	.endfunc

	;.text

;	.fini9	exit() starts here
	.section .fini9,"ax",@progbits
exit:
	cli

;	.fini0	Infinite loop after program termination
	.section .fini0,"ax",@progbits
stop_program:
	rjmp 	stop_program

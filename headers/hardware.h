#ifndef HARDWARE_H
#define HARDWARE_H

/*	includes	*/

//#include <avr/io.h>
#include <avr/sfr_defs.h>
#include "ports.h"

/*	constants	*/

#define ASM_TRAP_CODE	(0xFFFFU)

/*	asm ops	*/

// check out CBI, SBI, SBIS, AND SBIC
// CBI A,b where A is a register from 0 to 31, and b is the bit to clear
// works up to EECR
#define ASM_CLR_REGBIT(reg, bit) __asm__ __volatile__ (	\
		"cbi %0, %1 \n\t"	\
		: /* no outputs */	\
		: "I" _SFR_IO_ADDR(reg), "I" (bit)	\
);

#define ASM_SET_REGBIT(reg, bit) __asm__ __volatile__ (	\
		"sbi %0, %1 \n\t"	\
		: /* no outputs */	\
		: "I" _SFR_IO_ADDR(reg), "I" (bit)	\
);

/*!	all physical pin labels. don't know if we'll need it */
typedef enum {
	PIN_PD3 = 1,
	PIN_PD4,
	PIN_GND,
	PIN_VCC,
	PIN_GND2,
	PIN_VCC2,
	PIN_PB6,
	PIN_PB7,
	PIN_PD5,
	PIN_PD6,
	PIN_PD7,
	PIN_PB0,
	PIN_PB1,
	PIN_PB2,
	PIN_PB3,
	PIN_PB4,
	PIN_PB5,
	PIN_AVCC,
	PIN_ADC6, //PIN_PC6 OR 7?
	PIN_AREF,
	PIN_AGND,
	PIN_ADC7, //PIN_PC7?
	PIN_PC0,
	PIN_PC1,
	PIN_PC2,
	PIN_PC3,
	PIN_PC4,
	PIN_PC5,
	PIN_PC6,
	PIN_PD0,
	PIN_PD1,
	PIN_PD2, PIN_MAX = PIN_PD2,
	PIN_CAP
} hwPin_t;

/*!	don't know if we'll need it. Table that translates external pin number to
	internal port number */
extern unsigned char const pinport[PIN_CAP];


#endif // HARDWARE_H

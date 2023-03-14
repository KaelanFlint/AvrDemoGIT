/*! \file
	contains information about low level specific physical interfaces for
	hardware being used. This project uses the Arduino Ethernet 08d Rev3, which
	uses microcontroller ATMEGA328P-MU, */

/*	includes	*/

//#include "sysconfig.h"
//#include "debug.h"

#include "hardware.h"

//#include <avr/io.h>
#include "types.h"
#include "ports.h"

/*!	don't know if we'll need it. Table that translates external pin number to
	internal port number */
unsigned char const pinport[PIN_CAP] = {
	/*NULL*/		0,
	/*PIN_PD3*/		PORT_D3,
	/*PIN_PD4*/		PORT_D4,
	/*PIN_GND*/		0,
	/*PIN_VCC*/		0xFF,
	/*PIN_GND2*/	0,
	/*PIN_VCC2*/	0xFF,
	/*PIN_PB6*/		PORT_B6,
	/*PIN_PB7*/		PORT_B7,
	/*PIN_PD5*/		PORT_D5,
	/*PIN_PD6*/		PORT_D6,
	/*PIN_PD7*/		PORT_D7,
	/*PIN_PB0*/		PORT_B0,
	/*PIN_PB1*/		PORT_B1,
	/*PIN_PB2*/		PORT_B2,
	/*PIN_PB3*/		PORT_B3,
	/*PIN_PB4*/		PORT_B4,
	/*PIN_PB5*/		PORT_B5,
	/*PIN_AVCC*/	0xFF,
	/*PIN_ADC6*/	PORT_BAD,
	/*PIN_AREF*/	0xFF,
	/*PIN_AGND*/	0,
	/*PIN_ADC7*/	PORT_BAD,
	/*PIN_PC0*/		PORT_C0,
	/*PIN_PC1*/		PORT_C1,
	/*PIN_PC2*/		PORT_C2,
	/*PIN_PC3*/		PORT_C3,
	/*PIN_PC4*/		PORT_C4,
	/*PIN_PC5*/		PORT_C5,
	/*PIN_PC6*/		PORT_C6,
	/*PIN_PD0*/		PORT_D0,
	/*PIN_PD1*/		PORT_D1,
	/*PIN_PD2*/		PORT_D2
};

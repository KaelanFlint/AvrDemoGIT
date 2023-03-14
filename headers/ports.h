#ifndef PORTS_H
#define PORTS_H

/*!	\file ports.h
	low level system wide port control and interface. too "clever" for my own good
 */

#ifdef SYSCONFIG_H
#error no one but the mid level software should include this file
#endif

/* includes */

#include "types.h"
#include "hardware.h"

#define PDIR_OUT SET
#define PDIR_IN CLEAR

// B can be 0b00000000 through 0b00000111 (0x00 - 0x07)
// C can be 0b00001000 through 0b00001110 (0x08 - 0x0E) // skip 0x0F, no c7
// D can be 0b00010000 through 0b00010111 (0x10 - 0x17)
// shift down port arg by 3
// port>>3 to get port group: 0, 1, or 2
// mask low 3 bytes to get which port within the group
// port&0x07

#define PORT_GROUP(port) (port >> 3u)
#define PORT_BIT(port) (port & 0x07u)

#define PIN_REG_ADDR(port) ( (1u << PORT_GROUP(port))*3u )
#define DDR_REG_ADDR(port) ( (1u << PORT_GROUP(port))*3u + 1u)
#define PORT_REG_ADDR(port) ( (1u << PORT_GROUP(port))*3u + 2u)

#define PIN_REG(port) (_SFR_IO8(PIN_REG_ADDR(port)))
#define DDR_REG(port) (_SFR_IO8(DDR_REG_ADDR(port)))
#define PORT_REG(port) (_SFR_IO8(PORT_REG_ADDR(port)))

/* public types */

/*!	index to port function */
typedef enum {
	PORT_B0 = 0, PORT_ICP = PORT_B0, PORT_CLKO = PORT_B0,
	PORT_B1 = 1, PORT_OC1A = PORT_B1, PORT_LED0 = PORT_B1,
	PORT_B2 = 2, PORT_OC1B = PORT_B2, PORT_SS = PORT_B2,
	PORT_B3 = 3, PORT_OC2 = PORT_B3, PORT_MOSI = PORT_B3,
	PORT_B4 = 4, PORT_MISO = PORT_B4,
	PORT_B5 = 5, PORT_SCK = PORT_B5,
	PORT_B6 = 6, PORT_TOSC1 = PORT_B6, PORT_XTAL1 = PORT_B6,
	PORT_B7 = 7, PORT_TOSC2 = PORT_B7, PORT_XTAL2 = PORT_B7,
	PORT_C0 = 8, PORT_ADC0 = PORT_C0,
	PORT_C1 = 9, PORT_ADC1 = PORT_C1,
	PORT_C2 = 10, PORT_ADC2 = PORT_C2,
	PORT_C3 = 11, PORT_ADC3 = PORT_C3,
	//! 2-wire Serial Bus Data Input/Output Line)
	PORT_C4 = 12, PORT_ADC4 = PORT_C4, PORT_SDA = PORT_C4,
	//! 2-wire Serial Bus Clock Line)
	PORT_C5 = 13, PORT_ADC5 = PORT_C5, PORT_SCL = PORT_C5,
	//! ~RESET pin
	PORT_C6 = 14, PORT_RESET = PORT_C6,
	PORT_BAD = 15, // don't use, does not exist
	PORT_D0 = 16, PORT_RXD = PORT_D0,
	PORT_D1 = 17, PORT_TXD = PORT_D1,
	PORT_D2 = 18, PORT_INT0 = PORT_D2,
	PORT_D3 = 19, PORT_INT1 = PORT_D3,
	PORT_D4 = 20, PORT_T0 = PORT_D4, PORT_XCK = PORT_D4,
		PORT_SD_CS = PORT_D4, PORT_SEN3 = PORT_D4,
	PORT_D5 = 21, PORT_T1 = PORT_D5,
	PORT_D6 = 22, PORT_AIN0 = PORT_D6,
	PORT_D7 = 23, PORT_AIN1 = PORT_D7
} port_t;


#define PORT_VALID(portsel) ((portsel <= PORT_D7) && (portsel != PORT_BAD))
/*!	toggles the port output */
#define PORT_TOGGLE(port) do { \
		ASM_SET_REGBIT(PIN_REG(port), PORT_BIT(port));	\
} while (0)
/*!	sets the port output */
#define PORT_SET(port) do { \
		ASM_SET_REGBIT(PORT_REG(port), PORT_BIT(port));	\
} while (0)
/*!	clears the port output */
#define PORT_CLR(port) do { \
		ASM_CLR_REGBIT(PORT_REG(port), PORT_BIT(port));	\
} while (0)
/*!	sets the port to output */
#define PORT_OUTSET(port) do { \
		ASM_CLR_REGBIT(PORT_REG(port), PORT_BIT(port));	\
		ASM_SET_REGBIT(DDR_REG(port), PORT_BIT(port));	\
		ASM_CLR_REGBIT(PORT_REG(port), PORT_BIT(port));	\
} while (0)
/*!	sets the port to input */
#define PORT_INSET(port) do { \
		ASM_CLR_REGBIT(PORT_REG(port), PORT_BIT(port));	\
		ASM_CLR_REGBIT(DDR_REG(port), PORT_BIT(port));	\
		ASM_CLR_REGBIT(PORT_REG(port), PORT_BIT(port));	\
} while (0)

/* public functions */

/*! init basic system wide port settings */
void portsInit (void);
/*	test  functions	*/
void portTogglePB1 (void) __attribute__((always_inline)) ;
void portSetPB1 (void) __attribute__((always_inline)) ;
void portClearPB1 (void) __attribute__((always_inline)) ;

#endif // PORTS_H

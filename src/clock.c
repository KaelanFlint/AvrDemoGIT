
/* includes */

#include "sysconfig.h"
#include "clock.h"
#include <avr/io.h>
#include <avr/power.h>

/* private macros */

/*!
	\note Note that this oscillator is used to time EEPROM and Flash write
	accesses, and these write times will be affected accordingly. If the
	EEPROM or Flash are written, do not calibrate to more than 8.8MHz.
	Otherwise, the EEPROM or Flash write may fail.
*/
#define OSCCAL_SETTING

#define CLKREG_ENABLE (SET << CLKPCE)
#define CLKREG_DIV_BY_2 (0x01U << CLKPS0)

// use flash fuse bits CKSEL to select clock source
// low power crystal oscillator 0b1111 downto 0b1000 (0x0F downto 0x08)
// internal RC oscillator at 8MHz with CLKDIV8 = 1 resulting in 1MHz system clock

/* public functions */

/* init */
void clockInit (void) {

	// clock settings are written in fuse bits outside of normal software
	// this system has settings for 16MHz system clock
	// fuse bits LOW is 0xFF:
	// which translates into no clock divide by 8 (CKDIV8 = 1)
	// no clock output (CKOUT = 1)
	// select startup time (SUT = 0b11) slow rising power external crystal
	// clock source (CKSEL = 0b1111) external low power osc 8 - 16 MHz

	// dont write OSCCAL

	// CKDIV8 is unprogrammed, the CLKPS bits will be reset to “0000”
	// no need to change the clock just yet, I Like 16MHz
	// lower system clock frequency might be a good idea later
	// flash functionality requires it be 8MHz or less
	// and lower clock freq == lower power consumption
	//CLKPR = CLKREG_ENABLE;
	//CLKPR = CLKREG_DIV_BY_2;

	//alt version
	//clock_prescale_set(clock_div_2);

}

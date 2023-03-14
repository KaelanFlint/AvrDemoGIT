
/*	includes	*/

#include "ports.h"

#include "hardware.h"
#include <avr/io.h>
#include "types.h"

/*	private constants	*/

/*	public functions	*/

/*!	init */
void portsInit (void) {
	// MCUCR no PUD
	// MCUCR &= ~(SET << PUD);

	// DDR 0 w PORT 1 is a pull up input and can source current when PUD is 0 so don't do it
	// DDR 0 w PORT 0 is high impedance, so good for inputs
	// DDR 1 and PORT 1 or 0 is what you'd expect

	// from what I can understand of the documentation, don't do
	// 1. DDR 0 PORT 0 then 3. DDR 1 PORT 1,
	// do 2. DDR 1 PORT 0, then 3. or 2a. DDR 0 PORT 1
	// also don't do 1. DDR 0 PORT 1 then 3. DDR 1 PORT 0 (or vice versa)
	// put a 2. DDR 1 PORT 1 or 2a. DDR 0 PORT 0

	// PB0 was timer input capture (no longer needed)
	// PB1 is LED output
	DDRB = (PDIR_OUT << DDB1) | (PDIR_IN << DDB0);
	PORTB = RESET;

	// nothing on port c that needs to be made output
	//DDRC = RESET;
	//PORTC = RESET;

	// PD0 is uart rx in
	// PD1 is uart tx out
	DDRD = (PDIR_OUT << DDD1) | (PDIR_IN << DDD0);
	PORTD = RESET;
}

/*	test  functions	*/
void portTogglePB1 (void) {
	PORT_TOGGLE(PORTB1);
}

void portSetPB1 (void) {
	PORT_SET(PORTB1);
}

void portClearPB1 (void) {
	PORT_CLR(PORTB1);
}

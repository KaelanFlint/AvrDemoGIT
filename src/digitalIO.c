
/*	includes	*/
//ll things
#include "ports.h"
#include <avr/io.h>

//mid level things
#include "sysconfig.h"
#include "debug.h"

#include "digitalIO.h"

#include "types.h"

/*	private constants	*/


/*	public functions	*/

/*!	init */
void digitalIOInit (void) {
	portsInit();
}

void dioToggleLED0(void) {
	PORT_TOGGLE(PORT_LED0);
}

void dioSetLED0(void) {
	PORT_SET(PORT_LED0);
}

void dioClearLED0(void) {
	PORT_CLR(PORT_LED0);
}

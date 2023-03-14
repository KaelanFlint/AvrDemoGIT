#ifndef CLOCK_H
#define CLOCK_H

/*! \file clock.h
	basic interface for system clock
*/

#include "sysconfig.h"

/* public macros */

#define CLK_FREQ_HZ() ((unsigned long)SYSCFG_CLOCK_FREQ_HZ)

/* public functions */

/*! basic system clock init */
void clockInit (void);

#endif // CLOCK_H

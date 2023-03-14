#ifndef DIO_H
#define DIO_H

/*! \file digitalIO.h
	basic interface for digital IO pins
 */

/* includes */
#include "sysconfig.h"
#include "types.h"

/* public functions */

/*! init basic digitalIO settings */
void digitalIOInit (void);

void dioToggleLED0(void);
void dioSetLED0(void);
void dioClearLED0(void);


#endif // DIO_H

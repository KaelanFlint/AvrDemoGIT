#ifndef TIMER1_H
#define TIMER1_H

/*! \file timer1.h
	\ingroup TIM1
	interface to timer 1 for self monitoring and testing purposes
 */

/* includes */
#include "sysconfig.h"
#include "types.h"

typedef enum {
	T1ID_SYS = 0U,
	T1ID_TEST,
	T1ID_MAX
} t1MeasureleId_t;

/* public functions */

void timer1Init (void);
uint16_t timer1MeasureTimeElapsed (t1MeasureleId_t id);
void timer1MeasureTime2Start (void);
uint16_t timer1MeasureTime2Elapsed (void);
uint32_t timer1MeasureTime2Elapsed_uS (void);

#endif // TIMER1_H

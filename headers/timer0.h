#ifndef TIMER0_H
#define TIMER0_H
/*! \file timer0.h
	\ingroup TIM0
	interface to low level timer that is basis for system execution timing
 */

/* includes */

#include "sysconfig.h"
#include "types.h"

/* public functions */

void timer0Init (void);
callStatus_t const timer0SetHz (unsigned int hertz);

#if SYSCFG_TASKMODEL_POLLING
flag_t const timer0PollReset (void) ;
flag_t const timer0IsSet (void);
#endif

#endif // TIMER0_H

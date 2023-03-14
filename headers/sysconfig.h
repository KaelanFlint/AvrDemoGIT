#ifndef SYSCONFIG_H
#define SYSCONFIG_H

#include "mcuCommon.h"

/*	top level system configuration constants	*/

#if !defined(STRINGIFY)
#define STRINGIFY(x) #x
#define STRINGI(x) STRINGIFY(x)
#endif

#define SYSCFG_DEBUG 1

#define SYSCFG_UNCONFIGURED	0

#define SYSCFG_TASKMODEL_POLLING	0

#define SYSCFG_LL_CONTAINED_ISR		0

#define SYSCFG_TASKMODEL_THREADS	1

#if (SYSCFG_TASKMODEL_POLLING && SYSCFG_TASKMODEL_THREADS)
#error "pick threads or polling, can't have both"
#endif

#define SYSCFG_TASKMODEL	(SYSCFG_TASKMODEL_THREADS)

#define SYSCFG_CLOCK_FREQ_HZ	(16000000)
#if defined(F_CPU) && (F_CPU != SYSCFG_CLOCK_FREQ_HZ)
#error "mismatch between build flag F_CPU and system clock config"
#endif


#endif // SYSCONFIG_H

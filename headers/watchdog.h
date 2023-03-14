#ifndef WATCHDOG_H
#define WATCHDOG_H
/*! \file
	\ingroup WATCHDOG
	Interface to watchdog control
 */

#include "sysconfig.h"

#if (SYSCFG_TASKMODEL_THREADS) // {
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // }

/* public types */

typedef enum {
	WD_CLEAR = 0U,
	WD_PENDING_CLEAR,
	WD_PENDING_RESET
} watchdogStatus_t;

/* public functions */

/*! initalize watchdog */
void watchdogInit (void);

/*! periodic task for watchdog functions */
#if (SYSCFG_TASKMODEL_THREADS)
portTASK_FUNCTION_PROTO( watchdogOsTask, pvParameters );
#elif (SYSCFG_TASKMODEL_POLLING)
void watchdogPeriodicTask (void);
#endif

watchdogStatus_t watchdogStatusGet (void);

#endif // WATCHDOG_H

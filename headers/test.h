#ifndef TEST_H
#define TEST_H
/*! \file
	unit testing and debugging interface
 */

#include "sysconfig.h"

#if (SYSCFG_TASKMODEL_THREADS)
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

/*! initialize test module */
void testInit (void);

/*! periodic test task */
#if (SYSCFG_TASKMODEL_THREADS)
portTASK_FUNCTION_PROTO( testOsTask, pvParameters);
#elif (SYSCFG_TASKMODEL_POLLING)
void testPeriodicTask (void);
#endif

#endif // TEST_H

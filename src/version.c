
/*!	\file version.c
	system constant for reporting software version
	*/

/*	includes	*/

#include "sysconfig.h"
#include "debug.h"
#if (SYSCFG_TASKMODEL_THREADS)
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#endif
#include "version.h"

#include "types.h"
#include <avr/pgmspace.h>


/*	constants definitions	*/

prog_uchar const versionAPP[VERSION_STRING_SIZE]
							//__attribute__ ((section (".csect_version")))
		= "00.00.02";

#if (SYSCFG_TASKMODEL_THREADS)
prog_uchar const versionRTOS[VERSION_STRING_SIZE] =
		STRINGI(tskKERNEL_VERSION_MAJOR) "."
		STRINGI(tskKERNEL_VERSION_MINOR) "."
		STRINGI(tskKERNEL_VERSION_BUILD);
#endif


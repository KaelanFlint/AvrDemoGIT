#ifndef VERSION_H
#define VERSION_H

/*!	\file version.h
	interface to system constants for reporting software version
	*/


/*	includes	*/

#include <avr/pgmspace.h>

/*	public constants	*/

//!	allow two digits per version number with '.' between and null terminator
#define VERSION_STRING_SIZE (9)

extern prog_uchar const versionAPP[];
#if (SYSCFG_TASKMODEL_THREADS)
extern prog_uchar const versionRTOS[];
#endif

#endif // VERSION_H

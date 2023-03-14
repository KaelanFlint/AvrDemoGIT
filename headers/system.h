#ifndef SYSTEM_H
#define SYSTEM_H

/*! \file system.h
	\ingroup SYS
	system task execution and monitoring
 */

/* \note about current system configuration. Note that the fuses are read as
 * logical zero, “0”, if they are programmed
 * fuse bytes are  0xFF (L), 0xD7 (H), 0xFD (E)
 * Low 0xFF means:	CKSEL = 0b1111, Ext. Crystal Osc. 16.0- MHz; Start-up time PWRDWN/RESET: 16K CK/14 CK + 65 ms
 * 					SUT = 0b11, Ext. Crystal Osc. 16.0- MHz; Start-up time PWRDWN/RESET: 16K CK/14 CK + 65 ms
 * 					CKOUT = 1, allows the system clock to be output on PORTB0
 * 					CKDIV8 = 1, clock not divided by 8
 *
 * High 0xD7 means:	RSTDISBL = 1, external reset enabled, disable PC6 as IO pin
 * 					DWEN = 1, debugWire disabled
 * 					SPIEN = 0, serial program downloading enabled
 * 					WDTON = 1, watchdog not always on, determined by WDE and WDIE setting
 * 					EESAVE = 0, do preserve EEPROM through chip erase cycle
 * 					BOOTSZ = 0b11, Boot Flash size=256 words start address=$3F00
 * 					BOOTRST = 1, non boot loader reset vector
 * Extd 0xFD means:	BODLEVEL = 0b101, brown out detect at ~2.7v
 */

#include "sysconfig.h"

/* public functions */

void systemInit (void);

#if SYSCFG_TASKMODEL_POLLING
void systemExecution (void);
#endif

#endif // SYSTEM_H

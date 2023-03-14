#ifndef OS_TASK_THREADS_H
#define OS_TASK_THREADS_H
/*! \file
	\depreciated
	was used only for debugging and learning about FreeRTOS.
 */
/* includes */

#include "sysconfig.h"

#if (SYSCFG_TASKMODEL_THREADS) // {
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"
#endif // } (SYSCFG_TASKMODEL_THREADS)

typedef struct {
	uint8_t r31;
	uint8_t r30;
	uint8_t r29;
	uint8_t r28;
	uint8_t r27;
	uint8_t r26;
	uint8_t r25;
	uint8_t r24;
	uint8_t r23;
	uint8_t r22;
	uint8_t r21;
	uint8_t r20;
	uint8_t r19;
	uint8_t r18;
	uint8_t r17;
	uint8_t r16;
	uint8_t r15;
	uint8_t r14;
	uint8_t r13;
	uint8_t r12;
	uint8_t r11;
	uint8_t r10;
	uint8_t r9;
	uint8_t r8;
	uint8_t r7;
	uint8_t r6;
	uint8_t r5;
	uint8_t r4;
	uint8_t r3;
	uint8_t r2;
	uint8_t r1;
	uint8_t sreg;
	uint8_t r0;
	uint8_t taskfunc_addr_l;
	uint8_t taskfunc_addr_h;
} osStackThing_t;

#endif // OS_TASK_THREADS_H

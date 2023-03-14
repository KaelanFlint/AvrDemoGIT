/*! \file
	\ingroup SYS
	system task execution and monitoring implementation
 */

/******************************************************************************/
/*	includes	*/

#include "sysconfig.h"
#include "debug.h"
#if (SYSCFG_TASKMODEL_THREADS)
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#endif
#include <avr/io.h>
#if 1 //SYSCFG_LL_CONTAINED_ISR
#include <avr/interrupt.h>
#endif

#include "system.h"

#include "types.h"
#include "digitalIO.h"
#include "watchdog.h"
#include "clock.h"
#include "timer0.h"

#include "uart.h"
#include "timer1.h"

#include "test.h"

/******************************************************************************/
/*	private constants	*/

#if SYSCFG_TASKMODEL_POLLING
#define LOOPS_PER_SECOND (1000U)
#endif

/******************************************************************************/
/*	private variables	*/

static flag_t exitSystemReq;

#if SYSCFG_TASKMODEL_POLLING
static uint16_t count1sec_millisec;
#endif

static uint16_t sysTimePeriod;

static union {
	uint16_t addr_w;
	struct {
		uint8_t addrL_b;
		uint8_t addrH_b;
	} addr_bst;
} sysBadCallerAddr_u __attribute__ ((used));

/******************************************************************************/
/*	private function declarations	*/

/*!	pre-init check */
static void systemResetCheck (void);
/*!	init low level system components */
static void systemConfigure (void);

/******************************************************************************/
/*	public functions	*/

/*!	init */
void systemInit (void) {
	systemResetCheck();
	systemConfigure();

	clockInit();
	watchdogInit();

	digitalIOInit();
#if (SYSCFG_TASKMODEL_POLLING)
	timer0Init(); // now called by prvSetupTimerInterrupt / OS
#endif
	timer1Init();

	uartInit();

	//exitSystemReq = CLEAR;
	//count1sec_millisec = 0;

	testInit();
}

/*!	periodic task */
void systemExecution (void) {

	while(CLEAR == exitSystemReq) {
#if (SYSCFG_TASKMODEL_POLLING)
		if (timer0PollReset()) {
			sysTimePeriod = timer1MeasureTimeElapsed(T1ID_SYS);
			// rapid 1ms tasks
			watchdogPeriodicTask();

			uartPeriodicTask();
			//terminalPeriodicTask();

			testPeriodicTask();

			count1sec_millisec++;
			// slower 1 second tasks
			if (LOOPS_PER_SECOND <= count1sec_millisec) {
				count1sec_millisec = RESET;
			}
		}
#endif
	}
}

#if 1 //SYSCFG_LL_CONTAINED_ISR
/* vector handler */
//void BADISR_vect (void) __attribute__ ((signal,__INTR_ATTRS))  __attribute__ ((noreturn));
//void BADISR_vect (void)
ISR(BADISR_vect, ISR_NAKED) __attribute__ ((noreturn));
ISR(BADISR_vect) {
    // record info about error, like which ISR
	// load contents of stack pointer current location into X
	__asm__ __volatile__ ("in r28, __SP_L__" ::);
	__asm__ __volatile__ ("in r29, __SP_H__" ::);
	__asm__ __volatile__ ("ld r26, Y+" ::);
	__asm__ __volatile__ ("ld r27, Y" ::);
	
	__asm__ __volatile__ ("add r26, r26" ::);
	__asm__ __volatile__ ("adc r27, r27" ::);
	// store that at sysBadCallerAddr_u
	__asm__ __volatile__ ("sts sysBadCallerAddr_u, r26" ::);
	__asm__ __volatile__ ("sts sysBadCallerAddr_u + 1, r27" ::);

	// PC is 14 bits wide, instructions are 4 bytes each, so this returns exact address
//	sysBadCallerAddr_u.addr_w <<= 1;

	// then restart
	__asm__ __volatile__ ("jmp __vectors" ::);

	while (1);
}
#endif

/******************************************************************************/
/*	private functions	*/

/*!	check cause of last reset, log for feedback/reporting */
static void systemResetCheck (void) {
	registerByte_t mcuResetStatus = MCUSR;
	// check for cause of reset
	if (mcuResetStatus & (SET << WDRF)) {
		// watchdog reset happened, but why? do something to send some fault info
	}
	if (mcuResetStatus & (SET << BORF)) {
		// brown out event
	}
	if (mcuResetStatus & (SET << EXTRF)) {
		// external reset
	}
	if (mcuResetStatus & (SET << PORF)) {
		// normal
	}
	MCUSR = 0;
}

/*!	configure low level system wide settings */
static void systemConfigure (void) {
	// todo: make modules for sleep/power control
	// SMCR, MCUCR (BODS, BODSE), PRR

	// MCUCR mcu control reg
	// no bootloader vector relocation (see pg 77 megaAVR datasheet)
	MCUCR = (SET << IVCE);
	MCUCR &= ~((SET << IVSEL) | (SET << IVCE));

	// interrupt vector start address is affected by IVSEL in MCUCR

}

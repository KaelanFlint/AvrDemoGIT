/*! \file
	\ingroup WATCHDOG
	Watchdog implementation. Set for 1 second timeout time. if watchdog timer
	is not reset the Watchdog ISR is called
 */

/******************************************************************************/
/*	includes	*/

#include "sysconfig.h"
#include "debug.h"

#if (SYSCFG_TASKMODEL_THREADS)
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include "watchdog.h"

#include "types.h"

/******************************************************************************/
/*	private constants	*/

#define WD_CLOCK_FREQ_HZ (128000U)

#define WD_MODE_RESET	(0)
#define WD_MODE_ISR		(1)

/*!	timer prescaler field mask */
#define WD_WDP_MASK ((1<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0))

#define WD_CLEAR_IF		(SET << WDIF)

#define WD_DISABLED ( (CLEAR << WDIE) | (CLEAR << WDE) )
#define WD_ISR_MODE ( (SET << WDIE) | (CLEAR << WDE) )
#define WD_RESET_MODE ( (CLEAR << WDIE) | (SET << WDE) )
#define WD_ISR_RESET_MODE ( (SET << WDIE) | (SET << WDE) )

#if (WD_MODE_RESET)
#if (WD_MODE_ISR) // + isr
#define WD_MODE_SETTING (WD_ISR_RESET_MODE)
#else // no isr
#define WD_MODE_SETTING (WD_RESET_MODE)
#endif
#else // !WD_MODE_RESET no reset
#if (WD_MODE_ISR) // + isr
#define WD_MODE_SETTING (WD_ISR_MODE)
#else // no isr
#define WD_MODE_SETTING (WD_DISABLED)
#endif
#endif // WD_WANT_WDE_CLEAR

/*!	2K cycles @ 128KHz */
#define WD_TIMEOUT_16_mS ((0b0 << WDP3) | (0b000 << WDP0))
/*!	4K cycles @ 128KHz */
#define WD_TIMEOUT_32_mS ((0b0 << WDP3) | (0b001 << WDP0))
/*!	8K cycles @ 128KHz */
#define WD_TIMEOUT_64_mS ((0b0 << WDP3) | (0b010 << WDP0))
/*!	16K cycles @ 128KHz */
#define WD_TIMEOUT_125_mS ((0b0 << WDP3) | (0b011 << WDP0))
/*!	32K cycles @ 128KHz */
#define WD_TIMEOUT_250_mS ((0b0 << WDP3) | (0b100 << WDP0))
/*!	64K cycles @ 128KHz */
#define WD_TIMEOUT_500_mS ((0b0 << WDP3) | (0b101 << WDP0))
/*!	128K cycles @ 128KHz */
#define WD_TIMEOUT_1_S ((0b0 << WDP3) | (0b110 << WDP0))
/*!	256K cycles @ 128KHz */
#define WD_TIMEOUT_2_S ((0b0 << WDP3) | (0b111 << WDP0))
/*!	512K cycles @ 128KHz */
#define WD_TIMEOUT_4_S ((0b1 << WDP3) | (0b000 << WDP0))
/*!	1024K cycles @ 128KHz */
#define WD_TIMEOUT_8_S ((0b1 << WDP3) | (0b001 << WDP0))

#define WD_TIMEOUT_SETTING ( WD_TIMEOUT_1_S )


#if (SYSCFG_TASKMODEL_THREADS) // {

#ifdef WD_TIMEOUT_SETTING
#if (configTICK_RATE_HZ > 0)
#define CONFIG_TICK_mS	(1000/configTICK_RATE_HZ)
#else
#define CONFIG_TICK_mS	1000000000
#endif

#if (WD_TIMEOUT_SETTING == WD_TIMEOUT_16_mS)
#define WD_TIMEOUT_mS	16 // 15.625
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_32_mS)
#define WD_TIMEOUT_mS	32 // 31.25
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_64_mS)
#define WD_TIMEOUT_mS	64 // 62.5
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_125_mS)
#define WD_TIMEOUT_mS	125
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_250_mS)
#define WD_TIMEOUT_mS	250
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_500_mS)
#define WD_TIMEOUT_mS	500
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_1_S)
#define WD_TIMEOUT_mS	1000
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_2_S)
#define WD_TIMEOUT_mS	2000
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_4_S)
#define WD_TIMEOUT_mS	4000
#elif (WD_TIMEOUT_SETTING == WD_TIMEOUT_8_S)
#define WD_TIMEOUT_mS	8000
#else
#define WD_TIMEOUT_mS	0
#endif

#if (WD_TIMEOUT_mS > CONFIG_TICK_mS)
//pdMS_TO_TICKS
#define WD_KEEP_ALIVE_TKS	pdMS_TO_TICKS(WD_TIMEOUT_mS)
#else
#warning watch dog runs faster than OS ticks, might wanna rejigger the thing
#undef WD_KEEP_ALIVE_TKS
#define WD_KEEP_ALIVE_TKS 1
#endif

#else
#error you need to define the watchdog timeout setting
#endif
#endif // } SYSCFG_TASKMODEL_THREADS

#define WD_IS_WDE_SET(wdreg) (wdreg & (SET << WDE))


#define WD_RESET() __asm__ __volatile__ ("wdr")

#define WD_ENABLE(regval) __asm__ __volatile__ (	\
    "in __tmp_reg__,__SREG__	\n\t"	\
    "cli						\n\t"	\
    "wdr						\n\t"	\
    "sts %0,%1					\n\t"	\
    "out __SREG__,__tmp_reg__	\n\t"	\
    "sts %0,%2					\n\t"	\
    : /* no outputs */					\
    : "M" (_SFR_MEM_ADDR(WDTCSR)),	\
    "r" (_BV(WDCE) | _BV(WDE)),	\
    "r" ((uint8_t) (regval & ( _BV(WDIF) | _BV(WDIE) | WD_WDP_MASK ) ))	\
    : "r0"	\
)

#define WD_DISABLE() __asm__ __volatile__ (	\
	"in __tmp_reg__, __SREG__	\n\t"	\
	"cli						\n\t"	\
	"sts %0, %1					\n\t"	\
	"sts %0, __zero_reg__		\n\t"	\
	"out __SREG__,__tmp_reg__	\n\t"	\
	: /* no outputs */ \
	: "M" (_SFR_MEM_ADDR(WDTCSR)),		\
	"r" ((uint8_t)(_BV(WDCE) | _BV(WDE)))	\
	: "r0"	\
)

/******************************************************************************/
/*	private variables	*/

static watchdogStatus_t watchdogStatus;

/*	debug/test variables	*/
#ifdef DEBUG_H
static volatile int debugControl = 1;
#endif


/******************************************************************************/
/*	private OS variables	*/
#if (SYSCFG_TASKMODEL_THREADS) // {

#define WD_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

#define WD_OS_STACK_SIZE 54

static volatile UBaseType_t watchdogStackLeft;

static StackType_t ucwatchdogTaskStack[ WD_OS_STACK_SIZE * sizeof( StackType_t ) ];
/* The variable that will hold the TCB of tasks created by this function.  See
the comments above the declaration of the xCreatorTaskTCBBuffer variable for
more information.  NOTE:  This is not static so relies on the tasks that use it
being deleted before this function returns and deallocates its stack.  That will
only be the case if configUSE_PREEMPTION is set to 1. */
static StaticTask_t xwatchdogTCBBuffer;

//static volatile UBaseType_t watchdogStackWatermark;

/*	private OS functions	*/

static inline void watchdogPeriodicTask (void);

#endif // } SYSCFG_TASKMODEL_THREADS


/******************************************************************************/
/*	public functions	*/

/*!	init */
void watchdogInit (void) {
	/*	This bit is used in timed sequences for changing WDE and prescaler bits.
	 	To clear the WDE bit, and/or change the prescaler bits, WDCE must be
	 	set. Once written to one, hardware will clear WDCE after four clock
	 	cycles.*/
	DBG_COND(debugControl) {
	WD_ENABLE( (WD_CLEAR_IF | WD_MODE_SETTING | WD_TIMEOUT_SETTING) );
	} // DBG_COND


#if (SYSCFG_TASKMODEL_THREADS)
	xTaskCreateStatic( watchdogOsTask, "WTD", WD_OS_STACK_SIZE, NULL,
		WD_TASK_PRIORITY, ucwatchdogTaskStack, &xwatchdogTCBBuffer );
#endif
}


#if (SYSCFG_TASKMODEL_THREADS)
portTASK_FUNCTION( watchdogOsTask, pvParameters ) {
	/* The parameters are not used. */
	(void)pvParameters;

	for (;;) {
		watchdogPeriodicTask();
		watchdogStackLeft = uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay(WD_KEEP_ALIVE_TKS);
	}
}
#endif


/*!	periodic task to 'pet' the watchdog. System will reset if not called in
	time, which is the point. To indicate that there's something seriously wrong
	as a task is taking far too long to complete */
#if (SYSCFG_TASKMODEL_THREADS)
static inline void watchdogPeriodicTask (void) {
#elif (SYSCFG_TASKMODEL_POLLING)
void watchdogPeriodicTask (void) {
#endif

	registerByte_t wdStatusPrev = WDTCSR;
	// check if MCUSR is overriding (SET << WDE);

	DBG_COND(debugControl) {
	WD_RESET();
	watchdogStatus = WD_CLEAR;
	} // DBG_COND

#if (WD_MODE_RESET)

	if (WD_IS_WDE_SET(wdStatusPrev)) {
		// there may be a problem, were we able to clear it?
		if (WD_IS_WDE_SET(WDTCSR)) {
			// oh no do something
			// todo: something (maybe record info or blurt out a message before
			// the system resets)
			watchdogStatus = WD_PENDING_RESET;
		}
	}

#endif
}


watchdogStatus_t watchdogStatusGet (void) {
	return (watchdogStatus);
}

/*!	ISR watchdog timer timeout */
static inline void watchdogTimeoutIsr (void) __attribute__((always_inline));
static inline void watchdogTimeoutIsr (void) {
	//WDTCSR |= (SET << WDIE); //WDIE = 0 auto cleared in hardware
	//If WDE is set this goes into system reset mode

	//WDIE should be set outside of ISR so that if it's not re-enabled
	//system will reset (because the assumption is that something is locked up
	//and needs to reset

	watchdogStatus = WD_PENDING_CLEAR;
}
ISR(WDT_vect) {
	watchdogTimeoutIsr();
}

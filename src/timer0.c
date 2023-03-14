/*! \file timer0.c
	\ingroup TIM0
	low level timer that is basis for system execution timing
 */

/******************************************************************************/
/*	includes	*/

#include "sysconfig.h"
#include "debug.h"
#include <avr/io.h>
#if 1 // (SYSCFG_LL_CONTAINED_ISR)
#include <avr/interrupt.h>
#endif
#include "timer0.h"

#include "types.h"
#include "clock.h"

/******************************************************************************/
/*	private macros	*/

#define T0REG_COUNT_SIZE (256)

#define T0REG_CLK_OFF (0)
#define T0REG_CLK_SYS (0b001 << CS00)
#define T0REG_CLK_DIV_8 (0b010 << CS00)
#define T0REG_CLK_DIV_64 (0b011 << CS00)
#define T0REG_CLK_DIV_256 (0b100 << CS00)
#define T0REG_CLK_DIV_1024 (0b101 << CS00)

/*!	(system clock/desired freq) / t0 clock prescaler, so (16MHz/125Hz)/1024 */
#define T0REG_COUNT_8ms (125U)

/*!	CTC mode (clears counter on match */
#define T0REG_CLEAR_ON_COMPARE (2U << WGM00)

#define T0REG_ENABLE_COMPARE_A_ISR (SET << OCIE0A)

#define T0REG_CLEAR_ISR (0x03U)
#define T0REG_CLEAR_COMPARE_A_ISR (SET << OCF0A)

#define T0_INIT_COUNTS

enum {
	T0_CLK_OFF = 0,
	T0_CLK_SYS = T0REG_CLK_SYS,
	T0_CLK_DIV8 = T0REG_CLK_DIV_8,
	T0_CLK_DIV64 = T0REG_CLK_DIV_64,
	T0_CLK_DIV256 = T0REG_CLK_DIV_256,
	T0_CLK_DIV1024 = T0REG_CLK_DIV_1024,
	T0_CLK_SETTINGS
};

/******************************************************************************/
/*	private variables	*/

static unsigned char const T0REG_CLK_DIV[T0_CLK_SETTINGS] = {
		T0_CLK_OFF, T0_CLK_SYS, T0_CLK_DIV8, T0_CLK_DIV64, T0_CLK_DIV256, T0_CLK_DIV1024
};
static unsigned char const T0_CLK_SHIFT[T0_CLK_SETTINGS] = { 0, 0, 3, 3, 2, 2 };

#if SYSCFG_TASKMODEL_POLLING
volatile static flag_t timerSet = CLEAR;
#endif

/******************************************************************************/
/*	public functions	*/

/*!	init */
void timer0Init (void) {
	// no OC0A out, no OC0B out,

	TCCR0A = T0REG_CLEAR_ON_COMPARE;

	TCCR0B = T0REG_CLK_DIV_1024;

	OCR0A = T0REG_COUNT_8ms;

	OCR0B = 0U;

	TIMSK0 = T0REG_ENABLE_COMPARE_A_ISR; // SYSCFG_LL_CONTAINED_ISR en or SYSCFG_TASKMODEL_THREADS, do we need the ISR?

	TIFR0 = T0REG_CLEAR_ISR;

	TCNT0 = 0U;
}

/*! an unused overly fancy way of being able to change timer 0 frequency */
callStatus_t const timer0SetHz (unsigned int hertz) {
	callStatus_t return_result = CALL_BAD_ARG;
	unsigned char setting_idx;
	unsigned int clock_counts = 0;
	registerByte_t tccr0b_reg = 0;
	registerByte_t ocr0a_reg = 0;

	if (0 < hertz) {
		clock_counts = CLK_FREQ_HZ()/hertz;

		setting_idx = 1;
		do {
			if (clock_counts <= T0REG_COUNT_SIZE) {
				break;
			}
			clock_counts >>= T0_CLK_SHIFT[setting_idx];
			setting_idx++;
		} while (setting_idx < T0_CLK_SETTINGS);

		if (setting_idx != T0_CLK_SETTINGS) {
			tccr0b_reg = T0REG_CLK_DIV[setting_idx];
			ocr0a_reg = (clock_counts - 1);

			return_result = CALL_OK;
		}
	}

	TCCR0B = tccr0b_reg;
	OCR0A = ocr0a_reg;

	return (return_result);
}

#if SYSCFG_TASKMODEL_POLLING
flag_t const timer0PollReset (void) {
	flag_t return_flag = timerSet;
	timerSet = CLEAR;
	return return_flag;
}

flag_t const timer0IsSet (void) {
	flag_t return_flag = timerSet;
	return return_flag;
}
#endif


//#if SYSCFG_LL_CONTAINED_ISR // OS uses this now, nothing for it to do
#if (SYSCFG_TASKMODEL_POLLING)
/*! ISR timer interrupt for bare metal base polling task loop */
static inline void timer0systimeIsr (void) __attribute__((always_inline));
static inline void timer0systimeIsr (void) {
	timerSet = SET;
	//TIFR0 = T0_CLEAR_COMPARE_A_INTERRUPT;
	//TCNT0 = 0U;
}
ISR(TIMER0_COMPA_vect) {
	timer0systimeIsr();
}
#endif

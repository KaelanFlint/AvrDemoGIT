/*! \file timer1.c
	\ingroup TIM1
	timer used to measure execution length for testing and self monitoring
 */

/*	includes	*/

#include "sysconfig.h"
#include "debug.h"
#include <avr/io.h>

#include "timer1.h"

#include "types.h"

/*	private macros	*/

/* TCCR1B setting options */
#define CLK_SRC_NONE (0x00 << CS10)
#define CLK_SRC_SYSCLK (0x01 << CS10)
#define CLK_SRC_SYSCLK_DIV8 (0x02 << CS10)
#define CLK_SRC_SYSCLK_DIV64 (0x03 << CS10)
#define CLK_SRC_SYSCLK_DIV256 (0x04 << CS10)
#define CLK_SRC_SYSCLK_DIV1024 (0x05 << CS10)
#define CLK_SRC_EXTCLK_FALLEDGE (0x06 << CS10)
#define CLK_SRC_EXTCLK_RISEEDGE (0x07 << CS10)

#define CLK_SRC_SETTING (CLK_SRC_SYSCLK_DIV64)

#if (CLK_SRC_SETTING == CLK_SRC_SYSCLK_DIV8)
#define CLK_DIV (8u)
#elif (CLK_SRC_SETTING == CLK_SRC_SYSCLK_DIV64)
#define CLK_DIV (64u)
#elif (CLK_SRC_SETTING == CLK_SRC_SYSCLK_DIV256)
#define CLK_DIV (256u)
#elif (CLK_SRC_SETTING == CLK_SRC_SYSCLK_DIV1024)
#define CLK_DIV (1024u)
#elif (CLK_SRC_NONE)
#define CLK_DIV (0u)
#else
#define CLK_DIV (1u)
#endif

#if (CLK_DIV != 0u)
#define T0CLK_FREQ_HZ (SYSCFG_CLOCK_FREQ_HZ/CLK_DIV)
/*! microseconds per timer 1 tick */
#define COUNT_uS (1000000u / T0CLK_FREQ_HZ)
/*	private variables	*/
#else
#define T0CLK_FREQ_HZ (0)
#define COUNT_uS (0)
#endif

static registerWord_t captureTime[T1ID_MAX];
static uint16_t captureTime2;

/*	public functions	*/

/*!	init */
void timer1Init (void) {
	// normal timer/counter operation with 0xFFFF as max
	TCCR1A = 0; // COM1A#/COM1B# normal port operation

	TCCR1B = (0 << ICES1) // trigger on down edge
		| (0x01 << CS10); // main clock, no prescaler, so 16MHz
	
	TCCR1C = 0;

	TCNT1H = 0;
	TCNT1L = 0;

	// OCR1AH OCR1AL OCR1BH OCR1BL

	// no timer1 interrupt
	//TIMSK1 = (1 << ICIE1); // timer 1 input capture interrupt enable
}

/*	getters/setters	*/

uint16_t timer1MeasureTimeElapsed (t1MeasureleId_t id) {
	registerWord_t prev_capture_time;
	uint16_t time_passed = 0xFFFFu;

	if (T1ID_MAX > id) {
		prev_capture_time = captureTime[id];

		captureTime[id] = TCNT1;

		time_passed = captureTime[id] - prev_capture_time;
	}

	return (time_passed);
}

void timer1MeasureTime2Start (void) {
	captureTime2 = TCNT1;
}
uint16_t timer1MeasureTime2Elapsed (void) {
	//static uint16_t captureTime2;
	registerWord_t prev_capture_time;
	uint16_t time_passed;

	prev_capture_time = captureTime2;

	captureTime2 = TCNT1;

	time_passed = captureTime2 - prev_capture_time;

	return (time_passed);
}

uint32_t timer1MeasureTime2Elapsed_uS (void) {
	uint32_t counts = (uint32_t)timer1MeasureTime2Elapsed();

	counts = counts * (uint32_t)COUNT_uS;

	return (counts);
}


/*	For measuring frequency only, the clearing of the ICF1 Flag is not required
	(if an interrupt handler is used)*/
#if 0
/*!	ISR triggers when falling edge captured on T1 input port */
static inline void timer1freqcapIsr (void)  __attribute__((always_inline));
static inline void timer1freqcapIsr (void) {
	uint16_t prev_capture_time = captureTime;
	
	// TIFR1 |= (1 << ICF1); // clear timer capture interrupt, auto cleared by ISR

	captureTime = ICR1;

	captureFreq = captureTime - prev_capture_time;
}
ISR(TIMER1_CAPT_vect) {
	timer1freqcapIsr();
}
#endif

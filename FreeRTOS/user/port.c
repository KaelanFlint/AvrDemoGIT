/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */
/*! \file
	required by FreeRTOS for port specific implementation of common required
	low level functions such as timer, registers, and stack
 */
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include "portmacro.h" //I don't know why but this can't be included here??
#ifdef USE_WATCHDOG
#include <avr/wdt.h>
#endif
#include "FreeRTOS.h"
#include "task.h"
// my ll drivers
#include "timer0.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the AVR port.
 *----------------------------------------------------------*/

/* Start tasks with interrupts enabled. */
#define portFLAGS_INT_ENABLED			( (StackType_t) 0x80 )

#if defined (portUSE_WDTO)
	#warning "Watchdog Timer used for scheduler."
	#define portSCHEDULER_ISR			WDT_vect

#elif defined (portUSE_TIMER0 )
/* Hardware constants for Timer0. */
//	  #warning "Timer0 used for scheduler."
#define portSCHEDULER_ISR			TIMER0_COMPA_vect
#define portCLEAR_COUNTER_ON_MATCH	( (uint8_t) _BV(WGM01) )
#define portPRESCALE_1024			( (uint8_t) (_BV(CS02)|_BV(CS00)) )
#define portCLOCK_PRESCALER			( (uint32_t) 1024 )
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE	( (uint8_t) _BV(OCIE0A) )
#define portOCRL					OCR0A
#define portTCCRa					TCCR0A
#define portTCCRb					TCCR0B
#define portTIMSK					TIMSK0
#define portTIFR					TIFR0

#endif

/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
 any details of its type. */
typedef void TCB_t;
extern volatile TCB_t *volatile pxCurrentTCB;

/*-----------------------------------------------------------*/

/**
 Enable the watchdog timer, configuring it for expire after
 (value) timeout (which is a combination of the WDP0
 through WDP3 bits).

 This function is derived from <avr/wdt.h> but enables only
 the interrupt bit (WDIE), rather than the reset bit (WDE).

 Can't find it documented but the WDT, once enabled,
 rolls over and fires a new interrupt each time.

 See also the symbolic constants WDTO_15MS et al.

 Updated to match avr-libc 2.0.0
 */

#if defined (portUSE_WDTO)

static __inline__
__attribute__ ((__always_inline__))
void wdt_interrupt_enable (const uint8_t value) {
	if (_SFR_IO_REG_P (_WD_CONTROL_REG)) {
		__asm__ __volatile__ (
				"in __tmp_reg__,__SREG__"	"\n\t"
				"cli"						"\n\t"
				"wdr"						"\n\t"
				"out %0, %1"				"\n\t"
				"out __SREG__,__tmp_reg__"	"\n\t"
				"out %0, %2"				"\n\t"
				: /* no outputs */
				: "I" (_SFR_IO_ADDR(WDTCSR)),
				"r" ((uint8_t)(_BV(WDCE) | _BV(WDE))),
				"r" (value | _BV(WDIF) | _BV(WDIE) | (0x27))
				: "r0"
		);
	}
	else {
		__asm__ __volatile__ (
				"in __tmp_reg__,__SREG__"	"\n\t"
				"cli"						"\n\t"
				"wdr"						"\n\t"
				"sts %0, %1"				"\n\t"
				"out __SREG__,__tmp_reg__"	"\n\t"
				"sts %0, %2"				"\n\t"
				: /* no outputs */
				: "n" (_SFR_MEM_ADDR(WDTCSR)),
				"r" ((uint8_t)(_BV(WDCE) | _BV(WDE))),
				"r" ((uint8_t) ("r" ((uint8_t) (value & 0x6F))
				: "r0"
		);
	}
}
#endif

/*-----------------------------------------------------------*/
/**
 Enable the watchdog timer, configuring it for expire after
 (value) timeout (which is a combination of the WDP0
 through WDP3 bits).

 This function is derived from <avr/wdt.h> but enables both
 the reset bit (WDE), and the interrupt bit (WDIE).

 This will ensure that if the interrupt is not serviced
 before the second timeout, the AVR will reset.

 Servicing the interrupt automatically clears it,
 and ensures the AVR does not reset.

 Can't find it documented but the WDT, once enabled,
 rolls over and fires a new interrupt each time.

 See also the symbolic constants WDTO_15MS et al.

 Updated to match avr-libc 2.0.0
 */

#if defined( portUSE_WDTO)

static __inline__
__attribute__ ((__always_inline__))
void wdt_interrupt_reset_enable (const uint8_t value) {
	if (_SFR_IO_REG_P (_WD_CONTROL_REG)) {
		__asm__ __volatile__ (
				"in __tmp_reg__,__SREG__"	"\n\t"
				"cli"						"\n\t"
				"wdr"						"\n\t"
				"out %0, %1"				"\n\t"
				"out __SREG__,__tmp_reg__"	"\n\t"
				"out %0, %2"				"\n\t"
				: /* no outputs */
				: "I" (_SFR_IO_ADDR(WDTCSR)),
				"r" ((uint8_t)(_BV(WDCE) | _BV(WDE))),
				"r" ((uint8_t) ("r" ((uint8_t) (value & 0x6F))
				: "r0"
		);
	}
	else {
		__asm__ __volatile__ (
				"in __tmp_reg__,__SREG__"	"\n\t"
				"cli"						"\n\t"
				"wdr"						"\n\t"
				"sts %0, %1"				"\n\t"
				"out __SREG__,__tmp_reg__"	"\n\t"
				"sts %0, %2"				"\n\t"
				: /* no outputs */
				: "n" (_SFR_MEM_ADDR(WDTCSR)),
				"r" ((uint8_t)(_BV(WDCE) | _BV(WDE))),
				"r" ((uint8_t) ("r" ((uint8_t) (value & 0x6F))
				: "r0"
		);
	}
}
#endif

/*-----------------------------------------------------------*/

/*
 * Macro to save all the general purpose registers, the save the stack pointer
 * into the TCB.
 *
 * The first thing we do is save the flags then disable interrupts. This is to
 * guard our stack against having a context switch interrupt after we have already
 * pushed the registers onto the stack - causing the 32 registers to be on the
 * stack twice.
 *
 * r1 is set to zero (__zero_reg__) as the compiler expects it to be thus, however
 * some of the math routines make use of R1.
 *
 * r0 is set to __tmp_reg__ as the compiler expects it to be thus.
 *
 *
 * The interrupts will have been disabled during the call to portSAVE_CONTEXT()
 * so we need not worry about reading/writing to the stack pointer.
 */

/* 2-Byte PC Save */
#define portSAVE_CONTEXT() __asm__ __volatile__ (	\
	"push	__tmp_reg__				\n\t"	\
	"in		__tmp_reg__, __SREG__	\n\t"	\
	"cli							\n\t"	\
	"push	__tmp_reg__				\n\t"	\
	"push	__zero_reg__			\n\t"	\
	"clr	__zero_reg__			\n\t"	\
	"push	r2						\n\t"	\
	"push	r3						\n\t"	\
	"push	r4						\n\t"	\
	"push	r5						\n\t"	\
	"push	r6						\n\t"	\
	"push	r7						\n\t"	\
	"push	r8						\n\t"	\
	"push	r9						\n\t"	\
	"push	r10						\n\t"	\
	"push	r11						\n\t"	\
	"push	r12						\n\t"	\
	"push	r13						\n\t"	\
	"push	r14						\n\t"	\
	"push	r15						\n\t"	\
	"push	r16						\n\t"	\
	"push	r17						\n\t"	\
	"push	r18						\n\t"	\
	"push	r19						\n\t"	\
	"push	r20						\n\t"	\
	"push	r21						\n\t"	\
	"push	r22						\n\t"	\
	"push	r23						\n\t"	\
	"push	r24						\n\t"	\
	"push	r25						\n\t"	\
	"push	r26						\n\t"	\
	"push	r27						\n\t"	\
	"push	r28						\n\t"	\
	"push	r29						\n\t"	\
	"push	r30						\n\t"	\
	"push	r31						\n\t"	\
	"lds	r26, pxCurrentTCB		\n\t"	\
	"lds	r27, pxCurrentTCB + 1	\n\t"	\
	"in		__tmp_reg__, __SP_L__	\n\t"	\
	"st		x+, __tmp_reg__			\n\t"	\
	"in		__tmp_reg__, __SP_H__	\n\t"	\
	"st		x+, __tmp_reg__			\n\t"	\
);

/*
 * Opposite to portSAVE_CONTEXT().	Interrupts will have been disabled during
 * the context save so we can write to the stack pointer.
 */

/* 2-Byte PC Restore */
#define portRESTORE_CONTEXT() __asm__ __volatile__ (	\
	"lds	r26, pxCurrentTCB		\n\t"	\
	"lds	r27, pxCurrentTCB + 1	\n\t"	\
	"ld		r28, x+					\n\t"	\
	"out	__SP_L__, r28			\n\t"	\
	"ld		r29, x+					\n\t"	\
	"out	__SP_H__, r29			\n\t"	\
	"pop	r31						\n\t"	\
	"pop	r30						\n\t"	\
	"pop	r29						\n\t"	\
	"pop	r28						\n\t"	\
	"pop	r27						\n\t"	\
	"pop	r26						\n\t"	\
	"pop	r25						\n\t"	\
	"pop	r24						\n\t"	\
	"pop	r23						\n\t"	\
	"pop	r22						\n\t"	\
	"pop	r21						\n\t"	\
	"pop	r20						\n\t"	\
	"pop	r19						\n\t"	\
	"pop	r18						\n\t"	\
	"pop	r17						\n\t"	\
	"pop	r16						\n\t"	\
	"pop	r15						\n\t"	\
	"pop	r14						\n\t"	\
	"pop	r13						\n\t"	\
	"pop	r12						\n\t"	\
	"pop	r11						\n\t"	\
	"pop	r10						\n\t"	\
	"pop	r9						\n\t"	\
	"pop	r8						\n\t"	\
	"pop	r7						\n\t"	\
	"pop	r6						\n\t"	\
	"pop	r5						\n\t"	\
	"pop	r4						\n\t"	\
	"pop	r3						\n\t"	\
	"pop	r2						\n\t"	\
	"pop	__zero_reg__			\n\t"	\
	"pop	__tmp_reg__				\n\t"	\
	"out	__SREG__, __tmp_reg__	\n\t"	\
	"pop	__tmp_reg__				\n\t"	\
);

/*-----------------------------------------------------------*/

/*
 * Perform hardware setup to enable ticks from relevant Timer.
 */
static void prvSetupTimerInterrupt (void);
/*-----------------------------------------------------------*/

StackType_t* pxPortInitialiseStack (StackType_t * pxTopOfStack,
	TaskFunction_t pxCode, void * pvParameters) {
	uint16_t usAddress;
	/* Simulate how the stack would look after a call to vPortYield() generated by
	 the compiler. */

	/* The start of the task code will be popped off the stack last, so place
	 it on first. */
	usAddress = (uint16_t) pxCode;
	*pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
	pxTopOfStack--;

	usAddress >>= 8;
	*pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
	pxTopOfStack--;


	/* Next simulate the stack as if after a call to portSAVE_CONTEXT().
	 portSAVE_CONTEXT places the flags on the stack immediately after r0
	 to ensure the interrupts get disabled as soon as possible, and so ensuring
	 the stack use is minimal should a context switch interrupt occur. */
	*pxTopOfStack = (StackType_t) 0x00; /* R0 */
	pxTopOfStack--;
	*pxTopOfStack = portFLAGS_INT_ENABLED;
	pxTopOfStack--;

	/* Now the remaining registers. The compiler expects R1 to be 0. */
	*pxTopOfStack = (StackType_t) 0x00; /* R1 */

	/* Leave R2 - R23 untouched */
	pxTopOfStack -= 23;

	/* Place the parameter on the stack in the expected location. (r24:r25) */
	usAddress = (uint16_t) pvParameters;
	*pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);
	pxTopOfStack--;

	usAddress >>= 8;
	*pxTopOfStack = (StackType_t) (usAddress & (uint16_t) 0x00ff);

	/* Leave register R26 - R31 untouched */
	pxTopOfStack -= 7;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler (void) {
	/* Setup the relevant timer hardware to generate the tick. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task that is going to run. */
	portRESTORE_CONTEXT();

	/* Simulate a function call end as generated by the compiler. We will now
	 jump to the start of the task the context of which we have just restored. */
	__asm__ __volatile__ ( "ret" );

	/* Should not get here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler (void) {
	/* It is unlikely that the ATmega port will get stopped. */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch. The first thing we do is save the registers so we
 * can use a naked attribute.
 */
void vPortYield (void) __attribute__ ( ( hot, flatten, naked ) );
void vPortYield (void) {
	portSAVE_CONTEXT();
	vTaskSwitchContext();
	portRESTORE_CONTEXT();

	__asm__ __volatile__ ( "ret" );
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch callable from ISRs. The first thing we do is save
 * the registers so we can use a naked attribute.
 */
void vPortYieldFromISR (void) __attribute__ ( ( hot, flatten, naked ) );
void vPortYieldFromISR (void) {
	portSAVE_CONTEXT();
	vTaskSwitchContext();
	portRESTORE_CONTEXT();

	__asm__ __volatile__ ( "reti" );
}
/*-----------------------------------------------------------*/

/*
 * Context switch function used by the tick. This must be identical to
 * vPortYield() from the call to vTaskSwitchContext() onwards. The only
 * difference from vPortYield() is the tick count is incremented as the
 * call comes from the tick ISR.
 */
void vPortYieldFromTick (void) __attribute__ ( ( hot, flatten, naked ) );
void vPortYieldFromTick (void) {
	portSAVE_CONTEXT();
	if (xTaskIncrementTick() != pdFALSE) {
		vTaskSwitchContext();
	}
	portRESTORE_CONTEXT();

	__asm__ __volatile__ ( "ret" );
}
/*-----------------------------------------------------------*/

#if defined(portUSE_WDTO)
/*
 * Setup WDT to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt( void )
{
	/* reset watchdog */
	wdt_reset();

	/* set up WDT Interrupt (rather than the WDT Reset). */
	wdt_interrupt_enable( portUSE_WDTO );
}

#elif defined (portUSE_TIMER0)
/*
 * Setup Timer0 compare match A to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt (void) {
	timer0Init();
}

#endif

/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 1

/*
 * Tick ISR for preemptive scheduler. We can use a naked attribute as
 * the context is saved at the start of vPortYieldFromTick(). The tick
 * count is incremented after the context is saved.
 *
 * use ISR_NOBLOCK where there is an important timer running, that should preempt the scheduler.
 *
 */
ISR(portSCHEDULER_ISR, ISR_NAKED) __attribute__ ((hot, flatten));
/*	ISR(portSCHEDULER_ISR, ISR_NAKED ISR_NOBLOCK) __attribute__ ((hot, flatten));
 */
ISR(portSCHEDULER_ISR) {
	vPortYieldFromTick();
	__asm__ __volatile__ ( "reti" );
}
#else

	/*
	 * Tick ISR for the cooperative scheduler. All this does is increment the
	 * tick count. We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 *
	 * use ISR_NOBLOCK where there is an important timer running, that should preempt the scheduler.
	 */
	ISR(portSCHEDULER_ISR) __attribute__ ((hot, flatten));
/*	ISR(portSCHEDULER_ISR, ISR_NOBLOCK) __attribute__ ((hot, flatten));
 */
	ISR(portSCHEDULER_ISR) {
		xTaskIncrementTick();
	}
#endif


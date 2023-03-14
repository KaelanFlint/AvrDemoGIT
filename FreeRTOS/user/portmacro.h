#ifndef PORTMACRO_H
#define PORTMACRO_H
/*! \file
	required by FreeRTOS to interface to specific hardware implementation
 */

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

#include "sysconfig.h"

#ifdef USE_WATCHDOG
#include <avr/wdt.h>
#endif

/* Type definitions. */
#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE signed char
#define portBASE_TYPE char

#define portPOINTER_SIZE_TYPE unsigned short

typedef portSTACK_TYPE StackType_t;
typedef signed char BaseType_t;
typedef unsigned char UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned short TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffU
#else
	typedef unsigned long TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

#define portENTER_CRITICAL() MCU_ENTER_CRITICAL()

#define portEXIT_CRITICAL() MCU_EXIT_CRITICAL()

#define portDISABLE_INTERRUPTS() MCU_DISABLE_INTERRUPTS()
#define portENABLE_INTERRUPTS() MCU_ENABLE_INTERRUPTS()
	/*-----------------------------------------------------------*/

	/* Architecture specifics. */

	/* System Tick  - Scheduler timer
	 * Prefer to use the enhanced Watchdog Timer, but also Timer0 is ok.
	 */

	#if (USE_WATCHDOG && defined(WDIE) && defined(WDIF))              /* If Enhanced WDT with interrupt capability is available */

	#define portUSE_WDTO                WDTO_15MS   /* use the Watchdog Timer for xTaskIncrementTick */

	/* Watchdog period options:         WDTO_15MS
	                                    WDTO_30MS
	                                    WDTO_60MS
	                                    WDTO_120MS
	                                    WDTO_250MS
	                                    WDTO_500MS
	                                    WDTO_1S
	                                    WDTO_2S
	*/

	#else

	#define portUSE_TIMER0                          /* use the 8-bit Timer0 for xTaskIncrementTick */

	#endif

	#define portSTACK_GROWTH            ( -1 )

	/* Timing for the scheduler.
	 * Watchdog Timer is 128kHz nominal,
	 * but 120 kHz at 5V DC and 25 degrees is actually more accurate,
	 * from data sheet.
	 */
	#if defined( portUSE_WDTO )
	#define portTICK_PERIOD_MS          ( (TickType_t) _BV( portUSE_WDTO + 4 ) )
	#else
	#define portTICK_PERIOD_MS          ( (TickType_t) 1000 / configTICK_RATE_HZ )
	#endif

	#define portBYTE_ALIGNMENT          1
	#define portNOP()                   __asm__ __volatile__ ( "nop" );
	/*-----------------------------------------------------------*/

	/* Kernel utilities. */
	extern void vPortYield( void )      __attribute__ ( ( naked ) );
	#define portYIELD()                 vPortYield()

	extern void vPortYieldFromISR( void )   __attribute__ ( ( naked ) );
	#define portYIELD_FROM_ISR()            vPortYieldFromISR() // not used in other
	/*-----------------------------------------------------------*/

	#if defined(__AVR_3_BYTE_PC__)
	/* Task function macros as described on the FreeRTOS.org WEB site. */

	/* Add .lowtext tag from the avr linker script avr6.x for ATmega2560 and ATmega2561
	 * to make sure functions are loaded in low memory.
	 */
	#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) __attribute__ ((section (".lowtext")))
	#else
//#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) __attribute__ ((noreturn))
	#endif

	#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
	//#include "projdefs.h"
	//#define portTASK_FUNCTION( vFunction, pvParameters ) TaskFunction_t vFunction(pvParameters)

	#ifdef __cplusplus
	}
	#endif


#if 0 // notes on possible portX defines
portSET_INTERRUPT_MASK_FROM_ISR
portCLEAR_INTERRUPT_MASK_FROM_ISR
portFORCE_INLINE
portCRITICAL_NESTING_IN_TCB
portCLEAN_UP_TCB
portSETUP_TCB
portMEMORY_BARRIER
portSOFTWARE_BARRIER
portPRE_TASK_DELETE_HOOK

portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
portGET_RUN_TIME_COUNTER_VALUE
portALT_GET_RUN_TIME_COUNTER_VALUE
portPRIVILEGE_BIT
portYIELD_WITHIN_API
portSUPPRESS_TICKS_AND_SLEEP

portTASK_USES_FLOATING_POINT
portALLOCATE_SECURE_CONTEXT
portDONT_DISCARD
portASSERT_IF_INTERRUPT_PRIORITY_INVALID
portASSERT_IF_IN_ISR
portTICK_TYPE_IS_ATOMIC
portTICK_TYPE_ENTER_CRITICAL
portTICK_TYPE_EXIT_CRITICAL
portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR
portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR

portUSING_MPU_WRAPPERS

portNUM_CONFIGURABLE_REGIONS
portHAS_STACK_OVERFLOW_CHECKING

portARCH_NAME

portINPUT_BYTE

portSUPPORESS_TICKS_AND_SLEEP
portYIELD

portCLEAR_INTERRUPT_MASK_FROM_ISR( x )

portMPU_REGION_READ_WRITE
portMPU_REGION_READ_ONLY
portMPU_REGION_PRIVILEGED_READ_WRITE

portEND_SWITCHING_ISR

portREMOVE_STATIC_QUALIFIER




// maybe don't use?
portRESET_READY_PRIORITY
portRECORD_READY_PRIORITY
portGET_HIGHEST_PRIORITY


//don't implement
portBYTE_ALIGNMENT_MASK
portLU_PRINTF_SPECIFIER_REQUIRED


#endif

#endif // PORTMACRO_H

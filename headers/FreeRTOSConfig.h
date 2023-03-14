/*
 * FreeRTOS V202104.00
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "sysconfig.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION                    1
//#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
//#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      (SYSCFG_CLOCK_FREQ_HZ)
//#define configSYSTICK_CLOCK_HZ                  16000000
// would have been nice if the demo had made clear they wanted 16ms intervals instead of 1ms
// lets go with 8ms, clean number results in 125hz
//#define configTICK_RATE_HZ                      ( ( TickType_t ) 125 )
#define configTICK_RATE_HZ                      (125)
#define configMAX_PRIORITIES                    4
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 50 )
#define configMAX_TASK_NAME_LEN                 8 // 16
#define configUSE_16_BIT_TICKS                  1
#define configIDLE_SHOULD_YIELD                 1
//#define configUSE_TASK_NOTIFICATIONS            1
//#define configTASK_NOTIFICATION_ARRAY_ENTRIES   3
#define configUSE_MUTEXES                       1 // they didnt include it but I'm going to
#define configUSE_RECURSIVE_MUTEXES             0 // want this too, but later
#define configUSE_COUNTING_SEMAPHORES           0 // want this too, but later
//#define configUSE_ALTERNATIVE_API               0 /* Deprecated! */
#define configQUEUE_REGISTRY_SIZE               0
//#define configUSE_QUEUE_SETS                    0
////#define configUSE_TIME_SLICING                  0
//#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
//#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
#define configSTACK_DEPTH_TYPE                  unsigned short
//#define configMESSAGE_BUFFER_LENGTH_TYPE        unsigned short // maybe try unsigned long int?

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION             1
#define configSUPPORT_DYNAMIC_ALLOCATION            0
//#define configTOTAL_HEAP_SIZE                       1000
//#define configAPPLICATION_ALLOCATED_HEAP            1
//#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP   1

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          1
//#define configUSE_MALLOC_FAILED_HOOK            0
//#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/* Run time and task stats gathering related definitions. */
#define configUSE_TRACE_FACILITY	0

/* Co-routine definitions. */
////#define configUSE_CO_ROUTINES 		1
////#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer related definitions. */
//#define configUSE_TIMERS				1
//#define configTIMER_TASK_PRIORITY		2
//#define configTIMER_QUEUE_LENGTH		5
#define configTIMER_TASK_STACK_DEPTH	configMINIMAL_STACK_SIZE

/* Interrupt nesting behaviour configuration. */

/* Define to trap errors during development. */

/* FreeRTOS MPU specific definitions. */


// some found shit
#if 0
configUSE_TIMER_INSTANCE
configUSE_APPLICATION_TASK_TAG
configASSERT(x)
configPRECONDITION
configRECORD_STACK_HIGH_ADDRESS
configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H
configGENERATE_RUN_TIME_STATS
configEXPECTED_IDLE_TIME_BEFORE_SLEEP
configPRE_SUPPRESS_TICKS_AND_SLEEP_PROCESSING
configPRE_SLEEP_PROCESSING
configPOST_SLEEP_PROCESSING
configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS
configUSE_STATS_FORMATTING_FUNCTIONS
configUSE_POSIX_ERRNO
configINITIAL_TICK_COUNT
configPRINTF(x)
configENABLE_FPU 0
configENABLE_TRUSTZONE 0
configRUN_FREERTOS_SECURE_ONLY 0
configLIST_VOLATILE
configMAX_SYSCALL_INTERRUPT_PRIORITY
configMINIMAL_SECURE_STACK_SIZE

// likely dont need
#define configMAX( a, b )    ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#define configMIN( a, b )    ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
configUSE_ALTERNATIVE_API 0
configUSE_TASK_FPU_SUPPORT
configENABLE_MPU
configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES
configSUPPORT_CROSS_MODULE_OPTIMISATION
configIDLE_TASK_NAME
configTIMER_SERVICE_TASK_NAME


// no need
configASSERT_DEFINED
configPRECONDITION_DEFINED
configADJUSTED_HEAP_SIZE

#endif

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			1
//#define INCLUDE_xTaskDelayUntil		0
#define INCLUDE_vTaskDelay				1
//#define INCLUDE_xTaskGetIdleTaskHandle    0
//#define INCLUDE_xTaskAbortDelay    0
//#define INCLUDE_xQueueGetMutexHolder    0
//#define INCLUDE_xSemaphoreGetMutexHolder    INCLUDE_xQueueGetMutexHolder
//#define INCLUDE_xTaskGetHandle    0
#define INCLUDE_uxTaskGetStackHighWaterMark		1
//#define INCLUDE_uxTaskGetStackHighWaterMark2    0
//#define INCLUDE_eTaskGetState    0
//#define INCLUDE_xTaskResumeFromISR    1
//#define INCLUDE_xTimerPendFunctionCall    0
//#define INCLUDE_xTaskGetSchedulerState    0
//#define INCLUDE_xTaskGetCurrentTaskHandle    0

#endif /* FREERTOS_CONFIG_H */


/******************************************************************************/
/*	includes	*/

#include "sysconfig.h"
#include "debug.h"
#include "types.h"
#include "version.h"

#if (SYSCFG_TASKMODEL_THREADS)
//? {
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"
//? }
#endif

#include "test.h"

#include "uart.h"
//#include "terminal.h"
#include "digitalIO.h"
#include "timer1.h"

// debug todo: delete
#include "portmacro.h"

#include "ostaskthreads.h"

/******************************************************************************/
/*	private constants	*/

#if defined(__DOXYGEN__)
/*! testing out doxygen thing */
#define TEST_DOXY_THINGIE 1
#endif

#define TEST_STRING_SIZE_MAX (32U)

typedef enum {
	TEST_NONE = 0U,
	TEST_UART_RECEIVE,
	TEST_UART_SEND,
	TEST_COMMAND,
	TEST_MAX
} testCase_t;

/******************************************************************************/
/*	private variables	*/

static uint32_t testTime;
static uint16_t testTimePeriod;

static unsigned char testUartString[TEST_STRING_SIZE_MAX] = "abcdefghijklmnopqrstuvwxyz012\n\r";
static unsigned char testStringSize;

static volatile int testControl;
static volatile int testCallStatus;

static uartDataFrame_t testInCommand = 0;
static uartDataFrame_t testOutDataBuffer[TEST_STRING_SIZE_MAX];
static unsigned char testOutDataSize = 0;

/******************************************************************************/
/* private functions */

static void testCommandPeriodic(void);
static void testThread (void);
static unsigned char testDataGet
	(uartDataFrame_t const test_command, uartDataFrame_t * const buffer_ptr);


/******************************************************************************/
/*	private OS variables	*/

#if (SYSCFG_TASKMODEL_THREADS)

#define TEST_PERIOD_mS (8)
//#define TEST_PERIOD_TKS	(pdMS_TO_TICKS(TEST_PERIOD_mS)) // why doesn't this work?
#define TEST_PERIOD_TKS	((TEST_PERIOD_mS*125)/1000)

#if (TEST_PERIOD_TKS <= 0)
#undef TEST_PERIOD_TKS
#define TEST_PERIOD_TKS 1
#endif

#define TEST_TASK_PRIORITY	( tskIDLE_PRIORITY + 3 )

#define TEST_OS_STACK_SIZE 91


static StackType_t uctestTaskStack[ TEST_OS_STACK_SIZE * sizeof( StackType_t ) ];
/* The variable that will hold the TCB of tasks created by this function.  See
the comments above the declaration of the xCreatorTaskTCBBuffer variable for
more information.  NOTE:  This is not static so relies on the tasks that use it
being deleted before this function returns and deallocates its stack.  That will
only be the case if configUSE_PREEMPTION is set to 1. */
static StaticTask_t xtestTCBBuffer;
//static volatile osStackThing_t * taskStack_ptr;

static volatile UBaseType_t testStackLeft;

static inline void testPeriodicTask (void);

#endif // SYSCFG_TASKMODEL_THREADS


/******************************************************************************/
/*	public functions	*/

/*!	init */
void testInit (void) {

#if (SYSCFG_TASKMODEL_THREADS)
	//taskStack_ptr = (osStackThing_t *)
	//		&uctestTaskStack[((TEST_OS_STACK_SIZE) - sizeof(osStackThing_t))];

	xTaskCreateStatic( testOsTask, "TEST", TEST_OS_STACK_SIZE, NULL,
		TEST_TASK_PRIORITY, uctestTaskStack, &xtestTCBBuffer );
#endif

}


#if (SYSCFG_TASKMODEL_THREADS)
/*! OS task */
portTASK_FUNCTION( testOsTask, pvParameters ) {

	for (;;) {
		testTimePeriod = timer1MeasureTimeElapsed(T1ID_SYS);
		//testPeriodicTask();
		testThread();
		testStackLeft = uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay(TEST_PERIOD_TKS);
	}
}

#endif

/*!	periodic task */
#if (SYSCFG_TASKMODEL_THREADS)
static inline void testPeriodicTask (void) {
#elif (SYSCFG_TASKMODEL_POLLING)
void testPeriodicTask (void) {
#endif

	//(void)timer1MeasureTime2Elapsed();
	timer1MeasureTime2Start(); // measure time a task takes {

	switch (testControl) {
	case TEST_NONE :
		// do nothing
		break;
	case TEST_UART_RECEIVE:
		#if SYSCFG_TASKMODEL_THREADS
		dioSetLED0(); // test running
		#endif
		testCallStatus = uartRxRead(testUartString, &testStringSize);
		break;
	case TEST_UART_SEND:
		#if SYSCFG_TASKMODEL_THREADS
		dioSetLED0(); // test running
		#endif
		testCallStatus = uartTxWrite(testUartString, &testStringSize);
		break;
	case TEST_COMMAND:
		testCommandPeriodic();
		break;
	}

	testTime = timer1MeasureTime2Elapsed_uS(); // }
	
	if (TEST_NONE != testControl) {
		if (CALL_OK == testCallStatus) {
			#if SYSCFG_TASKMODEL_THREADS
			dioClearLED0();
			//testControl++;
			#else // SYSCFG_TASKMODEL_POLLING
			dioSetLED0(); // test running
			#endif
		}
		else if (CALL_PROCCESS_COMPLETE == testCallStatus) {
			#if SYSCFG_TASKMODEL_THREADS
			//testControl = 0; // error ?
			#else // SYSCFG_TASKMODEL_POLLING
			//testControl++;
			#endif
			dioClearLED0();
		}
		else if (CALL_BUSY == testCallStatus) {
			#if SYSCFG_TASKMODEL_THREADS
			//testControl = 0; // error ?
			dioClearLED0();
			#else // SYSCFG_TASKMODEL_POLLING
			dioSetLED0(); // still polling
			#endif
		}
		else {
			testControl = 0; // error ?
			dioClearLED0();
		}
	}

	if (testControl >=  TEST_MAX) {
		dioClearLED0(); // no test running
		testControl = 0;
	}
}


/*!	run a test according to character received over UART. Shows how to use rx
	and tx in practice (thread model) */
static void testThread (void) {
	unsigned char command_rx_size = 1;
	callStatus_t call_result;

	// waiting for new command
	call_result = uartRxRead(&testInCommand, &command_rx_size);

	if (CALL_OK != call_result) {
		//give error feedback over serial
		strncpy(testOutDataBuffer, "error: ", 7);
		testOutDataBuffer[7] = (call_result + 0x30);
	}
	else {
		testOutDataSize = testDataGet(testInCommand, testOutDataBuffer);
	}

	call_result = uartTxWrite(testOutDataBuffer, &testOutDataSize);

	if (CALL_OK != call_result) {
		//give error feedback, can't even communicate
		dioSetLED0();
	}

	return;
}

/*!	run a test according to character received over UART. Shows how to use rx
	and tx in practice (polling model) */
static void testCommandPeriodic(void) {
	unsigned char command_rx_size = 1;
	callStatus_t call_result = 0;

	if (0 == testInCommand)
	{
		// waiting for new command
		call_result = uartRxRead(&testInCommand, &command_rx_size);
	}
	else {
		call_result = uartTxWrite(testOutDataBuffer, &testOutDataSize);
		testInCommand = 0;
	}

	switch (call_result) {
	case CALL_BUSY:
		// wait till CALL_PROCCESS_COMPLETE
		break;
	case CALL_OK:
#if SYSCFG_TASKMODEL_POLLING
		//wait till CALL_PROCCESS_COMPLETE
		break;
	case CALL_PROCCESS_COMPLETE:
#endif
		if (0 == testOutDataSize) {
			// command received, prepare data to transmit
			//now we can read back the command received
			testOutDataSize = testDataGet(testInCommand, testOutDataBuffer);
		}
		else {
			// data transmit done
			testOutDataSize = 0;
		}
		break;
	default:
		//error
		break;
	}

	return;
}

static unsigned char testDataGet
(uartDataFrame_t const test_command, uartDataFrame_t * const buffer_ptr) {
	unsigned char buffer_size_ret = 0;
	char idx;

	switch (test_command) {
	case 'a':
		// just to test this setup works
		memcpy((void*) buffer_ptr, (void*)testUartString, TEST_STRING_SIZE_MAX);
		buffer_size_ret = TEST_STRING_SIZE_MAX;
		break;
	case 'b':
		// rule out memcpy? use debug to set buffer_size_ret to 32
		for (idx = 0; idx < TEST_STRING_SIZE_MAX; idx++)
			buffer_ptr[idx] = testUartString[idx];
		buffer_size_ret = idx;
		break;
	case 'c':
		//TODO doesnt work
		strncpy(buffer_ptr, versionAPP,  VERSION_STRING_SIZE);
		buffer_size_ret = VERSION_STRING_SIZE;
		break;
	case 'd':
		//TODO doesnt work
		strncpy(buffer_ptr, versionRTOS,  VERSION_STRING_SIZE);
		buffer_size_ret = VERSION_STRING_SIZE;
		break;
	case 'e':
		//TODO doesnt work
		itoa(testTimePeriod, buffer_ptr);
		buffer_size_ret = TEST_STRING_SIZE_MAX;
		break;
	default:
		//do nothing for now
		break;
	}

	return buffer_size_ret;
}

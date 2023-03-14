/*! \file
	\ingroup UART
	uart driver implementation. baud 9600, frame size 8 bits, 1 stop bit,
	no parity bit. Designed to handle transmitting or receiving a stream of
	frames successively for one caller at a time. Can be used in a round robin
	deterministic system (polling) or a multi thread system. Includes minimal
	error handeling and feedback
 */

/******************************************************************************/
/*	includes	*/

#include "sysconfig.h"
#include "debug.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "types.h"
#include "mcuCommon.h"

#define UART_MAIN
#include "uartReg.h"
#undef UART_MAIN

/******************************************************************************/
/*	private macros	*/

/* config macros */
/*! setting for UBRRn baud rate register */
#define UART_BAUD_REGS	(uint16_t)(UART_BAUD_UBRR_9600)
#define UART_BAUD_UBRRH (uint8_t)(UART_BAUD_REGS >> 8u)
#define UART_BAUD_UBRRL (uint8_t)(UART_BAUD_REGS & 0x00FF)

//#define UART_TASK_PAUSE_mS ((UART_BAUD_UBRR + 1)/100)

#if (UART_BAUD_UBRR > 4096)
#error "UART_BAUD_UBRR too large, wont fit, pick a faster baud"
#endif

#define UARTCFG_USE_RX_ISR 1
#define UARTCFG_USE_TX_ISR 0

#define RX_DATA_BUFFER_SIZE (64u)
#define TX_DATA_BUFFER_SIZE (64u)

/******************************************************************************/
/*	private types	*/

typedef union {
	uint8_t errByte;
	struct {
		uint8_t parity : 1;
		uint8_t dataOverrun : 1;
		uint8_t framing : 1;
		uint8_t rxBadState : 1;
		uint8_t txBadState : 1;
		uint8_t unused : 3;
	} errb;
} uartStatus_t;

typedef struct {
	uartDataFrame_t * buffer_ptr;
	unsigned char index;
	unsigned char length;
} uartControl_st;

/******************************************************************************/
/*	private variables	*/

/* these are accessed in and outside rx ISR */
static registerByte_t rxStatusRegCopy_Async;
static volatile uartControl_st rxData_Async;
static volatile uartControl_st txData_Async;

static uartStatus_t uartErrorNo;

/******************************************************************************/
/*	private function interface	*/

static inline void uartRxFrameCommon (void) __attribute__((always_inline));
static inline void uartTxFrameCommon (void) __attribute__((always_inline));

static void uartStatusCheck (void);

#if (SYSCFG_TASKMODEL_POLLING)
#if (!UARTCFG_USE_RX_ISR)
static void uartRxPollingTask (void);
#endif
#if (!UARTCFG_USE_TX_ISR)
static void uartTxPollingTask (void);
#endif
#endif

/******************************************************************************/
/***	OS interface	***/
#if (SYSCFG_TASKMODEL_THREADS) // {

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"

#define UART_OS_STACK_SIZE 54

#define UART_TASK_PRIORITY ( tskIDLE_PRIORITY + 2 )

#define UART_TASK_PAUSE_TKS (10) // just pick an arbitrary value, even 300Hz isn't slow enough to count the number of OS ticks (OS ticks currently at 8ms)
#ifdef UART_TASK_PAUSE_TKS
#if (UART_TASK_PAUSE_TKS <= 0u)
#undef UART_TASK_PAUSE_TKS
#warning need to wait more ticks before unyielding checking UART status task (try to get it at around how ever many milliseconds it takes to send/receive a single frame)
#define UART_TASK_PAUSE_TKS 1
#endif // if UART_TASK_PAUSE_TKS
#endif // ifdef UART_TASK_PAUSE_TKS


static volatile UBaseType_t uartStackLeft;

static StackType_t ucuartTaskStack[ UART_OS_STACK_SIZE * sizeof( StackType_t ) ];

/* The variable that will hold the TCB of tasks created by this function.  See
the comments above the declaration of the xCreatorTaskTCBBuffer variable for
more information.  NOTE:  This is not static so relies on the tasks that use it
being deleted before this function returns and deallocates its stack.  That will
only be the case if configUSE_PREEMPTION is set to 1. */
static StaticTask_t xuartTCBBuffer;

/*	private OS functions	*/

static inline void uartPeriodicTask (void);

#if (!UARTCFG_USE_RX_ISR)
static void uartRxThread (void);
#endif
#if (!UARTCFG_USE_TX_ISR)
static void uartTxThread (void);
#endif

#endif // } SYSCFG_TASKMODEL_THREADS
/***	end private OS interface	***/

/******************************************************************************/
/*	public functions	*/

void uartInit (void) {
	/* set baud rate */
	UBRR0H = UART_BAUD_UBRRH;
	UBRR0L = UART_BAUD_UBRRL;

	UCSR0A = 0;

	UCSR0B =
			//EN_RX_ISR_F |
			//EN_TX_ISR_F |
			//EN_DREGEMPTY_ISR_F |
			EN_TX_F | EN_RX_F;

	UCSR0C = FRAME_STOP_BITS_1 | FRAME_SIZE_8;

#if (SYSCFG_TASKMODEL_THREADS)
	xTaskCreateStatic( uartOsTask, "UART", UART_OS_STACK_SIZE, NULL,
		UART_TASK_PRIORITY,  ucuartTaskStack, &xuartTCBBuffer  );
#endif
}

#if (SYSCFG_TASKMODEL_THREADS)
portTASK_FUNCTION( uartOsTask, pvParameters ) {
	(void)pvParameters;

	for (;;) {
		uartPeriodicTask();
		uartStackLeft = uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay(UART_TASK_PAUSE_TKS);
	}
}
#endif

/*!	uart periodic task	*/
#if (SYSCFG_TASKMODEL_THREADS)
static inline void uartPeriodicTask (void) {
#elif (SYSCFG_TASKMODEL_POLLING)
void uartPeriodicTask (void) {

#if (!UARTCFG_USE_RX_ISR)
	uartRxPollingTask();
#endif
#if (!UARTCFG_USE_TX_ISR)
	uartTxPollingTask();
#endif
#endif
	uartStatusCheck();
}

/*!	uart internal error checking and logging	*/
static void uartStatusCheck (void) {
	MCU_ENTER_CRITICAL();
	registerByte_t const stat_reg_copy = rxStatusRegCopy_Async;
	uartControl_st const rx_async_copy = rxData_Async;
	uartControl_st const tx_async_copy = txData_Async;
	registerByte_t const isr_reg_copy = UCSR0B;
	MCU_EXIT_CRITICAL();

	if (stat_reg_copy & (1 << FE0)) {
		uartErrorNo.errb.framing = 1;
	}
	if (stat_reg_copy & (1 << DOR0)) {
		uartErrorNo.errb.dataOverrun = 1;
	}
	if (stat_reg_copy & (1 << UPE0)) {
		uartErrorNo.errb.parity = 1;
	}

#if (UARTCFG_USE_RX_ISR)
	if ( (isr_reg_copy & EN_RX_ISR_F) && (RESET == rx_async_copy.length) ) {
		// interrupt enabled when there's no room left in buffer
		// this shouldn't happen if call status behaves as it should
		uartErrorNo.errb.rxBadState = 1;
		/*** ISR DISABLED ***/
		DISABLE_RX_ISR();
		/********************/
	}
#endif
#if (UARTCFG_USE_TX_ISR)
	if ( (isr_reg_copy & EN_DREGEMPTY_ISR_F) && (RESET == tx_async_copy.length) ) {
		// this shouldn't happen if call status behaves as it should
		uartErrorNo.errb.txBadState = 1;
		/*** ISR DISABLED ***/
		TX_DREGEMPTY_DISABLE_ISR();
		/********************/
	}
#endif
}

/******************************************************************************/
/*	RX	*/

/*!	common code used to copy data from the rx register to the buffer. it knows
	it self disables when the buffer is full. \note MUST CHECK
	(NULL != rx_Async.buffer_ptr) && (RESET != rx_Async.length) WHEN USED	*/
static inline void uartRxFrameCommon (void) {
	rxStatusRegCopy_Async = UCSR0A;
#if (9 == UART_DATA_SIZE)
	rxData_Async.buffer_ptr[rxData_Async.index] = ((UCSR0B << RX_FRAME_DATA_HIGH_SHIFT) & DATA_HIGH_MASK);
	rxData_Async.buffer_ptr[rxData_Async.index] |= UDR0;
#else
	rxData_Async.buffer_ptr[rxData_Async.index]  = UDR0;
#endif
	rxData_Async.index++;
	if (rxData_Async.length <= rxData_Async.index) {
		rxData_Async.length = RESET;
#if UARTCFG_USE_RX_ISR
		/*** ISR DISABLED ***/
		DISABLE_RX_ISR();
		/********************/
#endif
	}
}

#if UARTCFG_USE_RX_ISR
/*!	ISR rx triggers when frame is received and puts frame data into rx buffer,
	disables itself when its reached the max frames for the buffer	*/
static inline void uartRxFrameIsr (void) __attribute__((always_inline));
static inline void uartRxFrameIsr (void) {
	if ( (NULL != rxData_Async.buffer_ptr) && (RESET != rxData_Async.length) ) {
		uartRxFrameCommon();
	}
}
ISR(USART_RX_vect) {
	uartRxFrameIsr();
}
#endif

/*!	The caller provides the buffer to fill with received data over uart.
	it waits until the buffer is full or there is an error, then returns with
	the result. Works a little different when using polling, need to call again
	with same buffer pointer to collect data (otherwise it assumes the data
	will go to the wrong buffer if there are multiple callers. only one can
	user rx at a time, indicated by .buffer_ptr != NULL) */
callStatus_t uartRxRead
(uartDataFrame_t const * const data_ptr, unsigned char * const num_frames_ptr) {

	registerByte_t stat_reg_copy = 0;
	callStatus_t call_result = CALL_OK;

	MCU_ENTER_CRITICAL();
	uartControl_st rx_async_copy = rxData_Async;
	MCU_EXIT_CRITICAL();

	// first make sure another string isn't currently still receiving
	if ((NULL == data_ptr) || (NULL == num_frames_ptr)
			|| (0 == *num_frames_ptr)) {
		call_result = CALL_BAD_ARG;
	}
	else if (RESET != rx_async_copy.length) {
		call_result = CALL_BUSY;
	}
#if SYSCFG_TASKMODEL_POLLING // {
	else if (data_ptr == rx_async_copy.buffer_ptr) {
		// args good
		// rx_async_copy.length == RESET

		//for polling to retrieve data without starting another rx session
		call_result = CALL_PROCCESS_COMPLETE;
		stat_reg_copy = rxStatusRegCopy_Async;
		*num_frames_ptr = rx_async_copy.index;
		rxData_Async.buffer_ptr = NULL;

		// rx state: busy -> idle
	}
	else if (NULL != rx_async_copy.buffer_ptr) {
		// length is 0, indicating it's done, but buffer hasn't been collected yet
		call_result = CALL_BUSY;
	}
#endif // }
	else { // idle
		/* 	TODO check that you don't need to add an error here for
			data_out_ptr != rx_async_copy.buffer_ptr (SYSCFG_TASKMODEL_POLLING)
		 	 could be that they're non null non matching and we're still midprocess */

		// RESET == rx_async_copy.length, NULL == rx_async_copy.buffer_ptr
		// new rx stream
		// rx state: idle -> busy
		if (RX_DATA_BUFFER_SIZE < *num_frames_ptr) {
			// limit how long anyone can hog the rx
			*num_frames_ptr = RX_DATA_BUFFER_SIZE;
		}
		// TODO: test protection {
		MCU_ENTER_CRITICAL();
		rxData_Async.buffer_ptr = (uartDataFrame_t *)data_ptr;
		rxData_Async.index = RESET;
		rxData_Async.length = *num_frames_ptr;
		MCU_EXIT_CRITICAL();
		// }

#if SYSCFG_TASKMODEL_THREADS // {
#if UARTCFG_USE_RX_ISR
		/*** ISR ENABLED ****/
		ENABLE_RX_ISR();
		/********************/
		do {
			MCU_ENTER_CRITICAL();
			rx_async_copy.length = rxData_Async.length;
			MCU_EXIT_CRITICAL();
		} while (RESET != rx_async_copy.length);
#else
		/*** rx wait ***/
		uartRxThread();
		/***************/
#endif
		stat_reg_copy = rxStatusRegCopy_Async;
		rx_async_copy = rxData_Async;
		*num_frames_ptr = rx_async_copy.index;
		rxData_Async.buffer_ptr = NULL;

		// rx state: busy -> idle
#endif // } otherwise the uart polling task completes the send of repeat frames
#if SYSCFG_TASKMODEL_POLLING
#if UARTCFG_USE_RX_ISR
		/*** ISR ENABLED ****/
		ENABLE_RX_ISR();
		/********************/
		/*	NOTE: what you are trying to do here is ultimately silly and
			pointless. Ditch the polling model */
#else
		// uartRxPollingTask called by uartPeriodicTask which is called by system loop
#endif
#endif
	}

	if (stat_reg_copy & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
		call_result = CALL_ERROR;
	}

	return (call_result);
}

#if SYSCFG_TASKMODEL_THREADS
#if (!UARTCFG_USE_RX_ISR)
static void uartRxThread (void) {
	if (NULL != rxData_Async.buffer_ptr) { // unlikely buffer_ptr is 0 here, it just got set
		while (RESET != rxData_Async.length) {
			while ( !(RX_FRAME_READY()) ) {
				//waiting for data to be received
			}
			portENTER_CRITICAL(); // {
			uartRxFrameCommon();
			portEXIT_CRITICAL(); // }
		}
	}
}
#endif
#endif

#if SYSCFG_TASKMODEL_POLLING
#if (!UARTCFG_USE_RX_ISR)
/*!	uart polling for rx data when no threads and no rx isr	*/
static void uartRxPollingTask (void) {
	if ( (NULL != rxData_Async.buffer_ptr) && (RESET != rxData_Async.length) ) {
		if (RX_FRAME_READY()) {
			uartRxFrameCommon();
		}
		else {
			// wait for next frame to be received
		}
	}
}
#endif
#endif

/******************************************************************************/
/*	TX	*/

/*!	common code used to copy data from the buffer to the tx register.
	it self disables when the buffer is full. MUST CHECK
	(NULL != tx_Async.buffer_ptr) && (RESET != tx_Async.length) WHEN USED	*/
static inline void uartTxFrameCommon (void) {
	// this clears UDRE0
#if (9 == UART_DATA_SIZE)
	UCSR0B = (txData_Async.buffer_ptr[txData_Async.index] & DATA_HIGH_MASK) >> TX_FRAME_DATA_HIGH_SHIFT;
	UDR0 = (txData_Async.buffer_ptr[txData_Async.index] & DATA_LOW_MASK);
#else
	UDR0 = txData_Async.buffer_ptr[txData_Async.index];
#endif
	txData_Async.index++;
	if (txData_Async.length <= txData_Async.index) {
		txData_Async.length = RESET;
#if UARTCFG_USE_TX_ISR
		/*** ISR DISABLED ***/
		DISABLE_DREGEMPTY_ISR();
		/********************/
#endif
	}
}

/*!	tx cstring */
callStatus_t uartTxWrite
(uartDataFrame_t const * const data_ptr,  unsigned char * const num_frames_ptr) {
	callStatus_t call_result = CALL_OK;
	
	MCU_ENTER_CRITICAL();
	uartControl_st tx_async_copy = txData_Async;
	MCU_EXIT_CRITICAL();
	
	if ((NULL == data_ptr) || (NULL == num_frames_ptr)
			|| (0 == *num_frames_ptr)) {
		call_result = CALL_BAD_ARG;
	}
	else if (RESET != tx_async_copy.length) {
		call_result = CALL_BUSY;
	}
#if SYSCFG_TASKMODEL_POLLING // {
	else if (data_ptr == tx_async_copy.buffer_ptr) {
		// args good
		// tx_async_copy.length == RESET

		//for polling to retrieve data without starting another rx session
		call_result = CALL_PROCCESS_COMPLETE;
		*num_frames_ptr = tx_async_copy.index;
		txData_Async.buffer_ptr = NULL;
		// tx state: busy -> idle
	}
	else if (NULL != tx_async_copy.buffer_ptr) {
		call_result = CALL_BUSY;
	}
#endif
	else {
		if (data_ptr != tx_async_copy.buffer_ptr) {
			// tx state: idle -> busy
			if (TX_DATA_BUFFER_SIZE < *num_frames_ptr) {
				//note: if @ 9600 it would take rougly 133ms to finish a stream that
				// large, even at the fastest (38400) it takes 33ms
				//STREAM TOO LONG
				*num_frames_ptr = TX_DATA_BUFFER_SIZE;
			}
			//TODO: test protection
			MCU_ENTER_CRITICAL();
			txData_Async.buffer_ptr = (uartDataFrame_t *)data_ptr;
			txData_Async.index = RESET;
			txData_Async.length = *num_frames_ptr;
			MCU_EXIT_CRITICAL();

#if SYSCFG_TASKMODEL_THREADS
#if UARTCFG_USE_TX_ISR
			/*** ISR ENABLED ****/
			TX_DREGEMPTY_ENABLE_ISR();
			do {
				MCU_ENTER_CRITICAL();
				tx_async_copy.length = txData_Async.length;
				MCU_EXIT_CRITICAL();
			} while (RESET != tx_async_copy.length)
#else
			/*** tx thread ***/
			uartTxThread();
			/*****************/
#endif
			tx_async_copy = txData_Async;
			*num_frames_ptr = tx_async_copy.index;
			txData_Async.buffer_ptr = NULL;
#endif // otherwise the uart polling task completes the send of repeat frames
		}
	}

	return (call_result);
}

#if SYSCFG_TASKMODEL_POLLING
#if (!UARTCFG_USE_TX_ISR)
/*!	TODO desc	*/
static void uartTxPollingTask (void) {

	if ( (NULL != txData_Async.buffer_ptr) && (RESET != txData_Async.length) ) {
		if (TX_FRAME_READY()) {
			uartTxFrameCommon();
		}
		else {
			// waiting for previous tx buffer to finish sending
		}
	}
}
#endif
#endif

#if SYSCFG_TASKMODEL_THREADS
#if (!UARTCFG_USE_TX_ISR)
/*!	TODO desc	*/
static void uartTxThread (void) {

	while ( (NULL != txData_Async.buffer_ptr) && (RESET != txData_Async.length) ) {
		while (!(TX_FRAME_READY())) {
			// wait for previous tx frame to finish sending
		}
		uartTxFrameCommon();
	}
}
#endif
#endif

#if (UARTCFG_USE_TX_ISR) // note: keeping just in case I want to have some high priority error spewing deathrows thing
/*!	ISR tx triggers when out buffer is empty, this ISR fills it again unless
	there is no more to send */
static inline void uartTxDregEmptyIsr (void) __attribute__((always_inline));
static inline void uartTxDregEmptyIsr (void) {

	if ( (NULL != txData_Async.buffer_ptr) && (RESET != txData_Async.length) ) {
		uartTxFrameCommon();
	}
}
ISR(USART_UDRE_vect) {
	uartTxDregEmptyIsr();
}
#endif

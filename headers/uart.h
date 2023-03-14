#ifndef UART_H
#define UART_H
/*! \file uart.h
	\ingroup UART
	interface to low level UART serial functions
 */

/* includes */

#include "sysconfig.h"

#if (SYSCFG_TASKMODEL_THREADS) // {
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#endif // } (SYSCFG_TASKMODEL_THREADS)

#include "types.h"

/* public macros */

/*! desired frame size (bits) */
#define UART_DATA_SIZE (8u)

/*! desired baudrate */
#define UART_BAUD (9600UL)


/* typedefs */

#if (9 == UART_DATA_SIZE)

typedef unsigned short uartDataFrame_t;
#else
typedef unsigned char uartDataFrame_t;
#endif


/* public functions */

/*! init uart registers */
void uartInit (void);

#if (SYSCFG_TASKMODEL_THREADS) // {
/*!	uart OS task thread */
portTASK_FUNCTION_PROTO( uartOsTask, pvParameters );
#elif (SYSCFG_TASKMODEL_POLLING)
/*! uart timed functions. Call at even intervals */
void uartPeriodicTask (void);
#endif // }

/*!	\param[in] data_ptr pointer to receiving data buffer
	\param[in,out] num_frames_ptr size of data stream in, returns actual data received
	\return status of uart RX call
	\retval CALL_OK call succeeded
	\retval CALL_BUSY caller should try again later
	\retval CALL_BAD_ARG caller sent bad arguments
	\retval CALL_ERROR uart is in a weird state
 */
callStatus_t uartRxRead (uartDataFrame_t const * const data_ptr,
		unsigned char * const num_frames_ptr) __attribute__ (( nonnull ));

/*!	\param[in] data_ptr pointer to transmitting data buffer
	\param[in,out] num_frames_ptr size of data stream out, returns actual data transmitted
	\return status of uart TX call
	\retval CALL_OK call succeeded
	\retval CALL_BUSY caller should try again later
	\retval CALL_BAD_ARG caller sent bad arguments
	\retval CALL_ERROR uart is in a weird state
 */
callStatus_t uartTxWrite (uartDataFrame_t const * const data_ptr,
	 unsigned char * const num_frames_ptr) __attribute__ (( nonnull ));

#endif // UART_H

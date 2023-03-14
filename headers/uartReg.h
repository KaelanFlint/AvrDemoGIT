
#ifndef UART_REG_H
#define UART_REG_H

/*! \file uartReg.h
	\ingroup UART
	macros to safely interface with uart registers. to be used by uart module only
 */

#include <avr/io.h>

#ifndef UART_MAIN
#warning "this file should only be included directly by the main uart implementation uart.c"
#endif

/* uart setting macros */

/* UCSR0A settings */

#define ASYNCH_DOUBLE_SPEED	(SET << U2X0)

/* UCSR0A status check */

#define RX_COMPLETE_F (SET << RXC0)
#define TX_COMPLETE_F (SET << TXC0)
#define TX_EMPTY_F (SET << UDRE0)

#define RX_FRAME_READY() (UCSR0A & (RX_COMPLETE_F))
#define TX_FRAME_READY() (UCSR0A & (TX_EMPTY_F))

/* UCSR0B settings */

#define EN_RX_ISR_F (SET << RXCIE0)
#define EN_TX_ISR_F (SET << TXCIE0)
#define EN_DREGEMPTY_ISR_F (SET << UDRIE0)
#define EN_TX_F (SET << TXEN0)
#define EN_RX_F (SET << RXEN0)

/* UCSR0B status check */

#define IS_RX_ISR_ENABLED() (UCSR0B & (EN_RX_ISR_F))
#define IS_DREGEMPTY_ISR_ENABLED() (UCSR0B & (EN_DREGEMPTY_ISR_F))
#define IS_RX_ISR_DISABLED() (UCSR0B ^ (EN_RX_ISR_F))
#define IS_DREGEMPTY_ISR_DISABLED() (UCSR0B ^ (EN_DREGEMPTY_ISR_F))

/* formatting for frame > 8 bits */

#define DATA_LOW_MASK (0x00FFU)
#define DATA_HIGH_MASK (0X0100U)
/*!	when receiving a 9 bit frame, the msb of the data is read at 0x02 of
	register UCSR0B */
#define RX_FRAME_DATA_HIGH_SHIFT (7U)
/*!	when sending a 9 bit frame, the msb of the data is written at 0x01 of
	register UCSR0B */
#define TX_FRAME_DATA_HIGH_SHIFT (8U)

/* these have side effects */

#define ENABLE_RX_ISR() (UCSR0B |= (EN_RX_ISR_F))
#define DISABLE_RX_ISR() (UCSR0B &= ~(EN_RX_ISR_F))
#define ENABLE_DREGEMPTY_ISR() (UCSR0B |= (EN_DREGEMPTY_ISR_F))
#define DISABLE_DREGEMPTY_ISR() (UCSR0B &= ~(EN_DREGEMPTY_ISR_F))

/* UCSR0C settings */

/* UCSZn Data Size Setting options */
#define FRAME_SIZE_5 (0x00U << UCSZ00)
#define FRAME_SIZE_6 (0x01U << UCSZ00)
#define FRAME_SIZE_7 (0x02U << UCSZ00)
#define FRAME_SIZE_8 (0x03U << UCSZ00)
#define FRAME_SIZE_9 (0x07U << UCSZ00)

#define FRAME_STOP_BITS_1 (0x00U << USBS0)
#define FRAME_STOP_BITS_2 (0x01U << USBS0)

/* UBRR0 settings */

/* valid baud rates for UBRRn, precalculated based on
	((FOSC/(16*UART_BAUD)) - 1) where FOSC is the system clock */
#define UART_BAUD_UBRR_300		(3332U)
#define UART_BAUD_UBRR_600		(1666U)
#define UART_BAUD_UBRR_1200		(832U)
#define UART_BAUD_UBRR_1800		(555U)
#define UART_BAUD_UBRR_2400		(416U)
#define UART_BAUD_UBRR_4800		(207U)
#define UART_BAUD_UBRR_7200		(138U)
#define UART_BAUD_UBRR_9600		(103U)
#define UART_BAUD_UBRR_14400	(68U)
#define UART_BAUD_UBRR_19200	(51U)
#define UART_BAUD_UBRR_31250	(31U)
#define UART_BAUD_UBRR_38400	(25U)

#endif // UART_REG_H

#ifndef MCU_COMMON_H
#define MCU_COMMON_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define MCU_DISABLE_INTERRUPTS() do { __asm__ __volatile__ ( "cli" );	\
	} while (0)

#define MCU_ENABLE_INTERRUPTS() do { __asm__ __volatile__ ( "sei" );	\
	} while (0)

/*!	save status register and global interrupt settings, then turn off global interrupts */
#define MCU_ENTER_CRITICAL() do { __asm__ __volatile__ ( \
		"in __tmp_reg__, __SREG__	\n\t" \
		"cli						\n\t" \
		"push __tmp_reg__			\n\t" \
); } while (0)

/*!	restore status register and global interrupt settings */
#define MCU_EXIT_CRITICAL() do { __asm__ __volatile__ ( \
		"pop __tmp_reg__			\n\t" \
		"out __SREG__, __tmp_reg__	\n\t" \
); } while (0)

#endif // MCU_COMMON_H

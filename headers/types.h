#ifndef TYPES_H
#define TYPES_H

/*!	\file types.h
	system wide generic types and constants
	*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*	public macros	*/

#ifndef RESET
#define RESET (0u)
#endif

#ifndef SET
#define SET (1U)
#endif

#ifndef CLEAR
#define CLEAR (0u)
#endif

#ifndef FALSE
#ifdef __bool_true_false_are_defined
#define FALSE (false)
#else
#define FALSE (0U)
#endif
#endif

#ifndef TRUE
#ifdef __bool_true_false_are_defined
#define TRUE (true)
#else
#define TRUE (1U)
#endif
#endif

/*	typedefs	*/

typedef unsigned char flag_t;

/*! standardized return values */
typedef enum {
	CALL_OK = 0,
	CALL_BAD_ARG,
	CALL_BUSY,
	CALL_PROCCESS_COMPLETE, /*!< used when a function has to be 'polled' (called more than once) to indicate successful completion */
	CALL_ERROR
} callStatus_t;

typedef volatile uint8_t registerByte_t;
typedef registerByte_t * registerByteAddrPtr_t;

typedef volatile uint16_t registerWord_t;
typedef registerWord_t * registerWordAddrPtr_t;

#endif // TYPES_H

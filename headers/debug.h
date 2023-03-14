#ifndef DEBUG_H
#define DEBUG_H

/*	includes	*/

#include "sysconfig.h"
#if (SYSCFG_DEBUG == 1)

/*	public constants	 */

#define DBG_COND(ex_block) if (ex_block)

#else

#define DBG_COND(ex_block)


#endif

#endif // DEBUG_H

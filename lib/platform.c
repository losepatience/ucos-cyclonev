/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stddef.h>
#include <delay.h>
#include <errno.h>

#include <asm/types.h>
#include <platform.h>

/* -------------------------------------------------------- */
/* -------------------- mutex ----------------------------- */
/* -------------------------------------------------------- */
inline void mutex_init(struct mutex *mutex)
{
	mutex->lock = OSSemCreate(1);
}

inline void mutex_destroy(struct mutex *mutex)
{
	INT8U err = 0;
	OSSemDel(mutex->lock, OS_DEL_ALWAYS, &err);
}

inline void mutex_lock(struct mutex *mutex)
{
	INT8U err = 0;
	OSSemPend(mutex->lock, 0, &err);
}

/*XXX 1 if sucessfully, others 0 */
inline int mutex_trylock(struct mutex *mutex)
{
	INT8U err = 0;
	OSSemPend(mutex->lock, 500, &err);
	return err == OS_ERR_NONE ? 1 : 0;
}

inline void mutex_unlock(struct mutex *mutex)
{
	OSSemPost(mutex->lock);
}

inline void init_completion(struct completion *x)
{
	x->com = OSSemCreate(0);
}

inline void complete(struct completion *x)
{
	OSSemPost(x->com);
}

long wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
	INT8U err = 0;
	u32 time;

	time = OSTimeGet();
	OSSemPend(x->com, timeout, &err);

	if (err == OS_ERR_TIMEOUT)
		return 0L;
	else if (err == OS_ERR_NONE)
		/*XXX plus 1 to avoid returning 0 */
		return (long)(OSTimeGet() - time) + 1;
	else
		return -1L;
}

bool wait_for_condition(volatile int *x, unsigned long timeout)
{
	u32 time = OSTimeGet();

	while ((OSTimeGet() - time) < timeout) {
		if (*x)
			return true;

		udelay(10);
	}

	return false;
}


/* -------------------------------------------------------- */
/* ---------------------- interrupt ----------------------- */
/* -------------------------------------------------------- */
static void __OS0_exc_handler(u8 exc)
{
	if (exc >= 0 && exc <= 4)
		for (; 1; )
			;
}

void OS_CSP_BSP_ExceptHandler(u8 exc)
{
	return __OS0_exc_handler(exc);
}

int request_irq(u32 irq, irq_handler_t handler, void *arg)
{
	int rval;
	rval = CSP_IntVectReg(0, (u16)irq, handler, arg);

	if (rval)
		CSP_IntEn(0, (u16)irq);

	return rval ? 0 : -EINVAL;
}

inline void free_irq(u32 irq)
{
	CSP_IntVectUnreg(0, (u16)irq);
}


/* -------------------------------------------------------- */
/* ---------------------- timer ----------------------- */
/* -------------------------------------------------------- */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)

void CPU_TS_TmrInit(void)
{
	CPU_PMU_En();
	CPU_PMU_CtrCycleEn();	/* Enable counter cycle counter. */
	CPU_PMU_CtrCycleRst();	/* Reset  counter cycle counter. */

	CPU_TS_TmrFreqSet((u32)25000000);
}

u32 CPU_TS_TmrRd(void)
{
	return (u32)CPU_PMU_CtrCycleGet();
}

u64 CPU_TS32_to_uSec(u32 ts_cnts)
{
	return 0u;
}

u64 CPU_TS64_to_uSec(u64 ts_cnts)
{
	return 0u;
}

#endif


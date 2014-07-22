/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 21 12:07:59 CST 2014
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

#include <platform.h>
#include <delay.h>
#include <stddef.h>

/* --------------------------------------------------------
 * completion:
 * -------------------------------------------------------- */
long wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
	u8 err = 0;
	u32 start;

	start = OSTimeGet();
	OSSemPend(x->com, timeout, &err);

	if (err == OS_ERR_TIMEOUT)
		return 0L;
	else if (err == OS_ERR_NONE)
		/*XXX plus 1 to avoid returning 0 */
		return (long)(OSTimeGet() - start) + 1;
	else
		return -1L;
}

bool wait_for_condition(volatile bool *x, unsigned long timeout)
{
	u32 start;

	if (timeout == 0) {

		while (!*x)
			udelay(10);

		return true;
	}

	start = OSTimeGet();
	do {
		if (*x)
			return true;

		msleep(1);

	} while ((OSTimeGet() - start) < timeout);

	return false;
}


void OS_CSP_BSP_ExceptHandler(u8 exc)
{
	if (exc >= 0 && exc <= 4)
		while (1)
			;
}

int request_irq(u32 irq, irq_handler_t handler, void *arg)
{
	if (!CSP_IntVectReg(0, (u16)irq, handler, arg))
		return -EINVAL;

	CSP_IntEn(0, (u16)irq);
	return 0;
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

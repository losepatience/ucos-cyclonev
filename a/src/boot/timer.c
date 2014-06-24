/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 16:52:28 CST 2014
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
#include <asm/arch/regs.h>
#include <asm/io.h>

#define TMR_BASE		DM_TIMER2_BASE
#define TIMER_OVERFLOW_VAL	0xffffffff

#ifndef V_OSCK
# define V_OSCK 24000000	/* Clock output from T2 */
#endif

#define TIMER_CLOCK		(V_OSCK / (2 << 2))

int timer_init(void)
{
	void *timer_base = (void *)TMR_BASE;

	/* start the counter ticking up, reload value on overflow */
	writel(0, timer_base + TMREG_TLDR);

	/* enable timer */
	writel(2 << 2 | 1 << 5 | 1 << 1 | 1, timer_base + TMREG_TCLR);

	return 0;
}


/* delay x useconds */
void udelay(unsigned long usec)
{
	long tmo = usec * (TIMER_CLOCK / 1000) / 1000;
	void *timer_base = (void *)TMR_BASE;
	unsigned long now, last = readl(timer_base + TMREG_TCRR);

	while (tmo > 0) {
		now = readl(timer_base + TMREG_TCRR);
		if (last > now)	/* count up timer overflow */
			tmo -= TIMER_OVERFLOW_VAL - last + now + 1;
		else
			tmo -= now - last;
		last = now;
	}
}

void sdelay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}

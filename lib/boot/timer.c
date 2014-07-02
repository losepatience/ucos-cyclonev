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

#include <asm/io.h>
#include <asm/regs.h>
#include <config.h>
#include <platform.h>

/* WARN: timer 0 is used as systick timer, here we use timer1 */
#define TMR_LOAD_COUNT		0x0
#define TMR_CURR_VAL		0x4
#define TMR_CTRL_REG		0x8

static void *timer1_regs = (void *)(SOCFPGA_OSC1TIMER0_ADDRESS + 0x1000);

static u32 read_timer(void)
{
	return readl(timer1_regs + TMR_CURR_VAL);
}

/* Delay x useconds */
static void __udelay(unsigned long usec)
{
	unsigned long now, last;

	/*
	 * get the tmo value based on timer clock speed
	 * tmo = delay required / period of timer clock
	 */
	long tmo = usec * CONFIG_TIMER_CLOCK_KHZ / 1000;

	last = read_timer();
	while (tmo > 0) {
		now = read_timer();
		if (last >= now)
			/* normal mode (non roll) */
			tmo -= last - now;
		else
			/* we have overflow of the count down timer */
			tmo -= TIMER_LOAD_VAL - last + now;
		last = now;
	}
}

/* Timer initialization */
int timer_init(void)
{
	u32 val;

	writel(TIMER_LOAD_VAL, timer1_regs + TMR_LOAD_COUNT);
	writel(TIMER_LOAD_VAL, timer1_regs + TMR_CURR_VAL);

	val = readl(timer1_regs + TMR_CTRL_REG);
	writel(val | 0x3, timer1_regs + TMR_CTRL_REG);

	return 0;
}

inline void udelay(unsigned long usec)
{
	__udelay(usec);
}

void mdelay(unsigned long msec)
{
	while (msec--)
		udelay(1000);
}

void msleep(unsigned int msecs)
{
	__msleep(msecs);
}

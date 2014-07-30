/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Fri Jun 27 16:03:53 CST 2014
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

#include <asm/regs.h>
#include <asm/io.h>
#include <malloc.h>
#include <config.h>
#include <timer.h>
#include <clock.h>
#include <cache.h>
#include <mm.h>

static volatile unsigned int __OS1_awake;
extern unsigned int __OS1_exceptions;
extern unsigned int __OS1_exceptions_end;
extern int main(void);
extern ulong bss_end;

static void wakeup_OS1(void)
{
	unsigned int *v = (unsigned int *)0;
	unsigned int *p = &__OS1_exceptions;
	unsigned int b[&__OS1_exceptions_end - &__OS1_exceptions];
	int i;

	/* copy interrupt vectors of core1 to core1 reset address(0) */
	for (i = 0; &p[i] < &__OS1_exceptions_end; i++) {
		b[i] = v[i];
		v[i] = p[i];
	}

	/* Reset core1, it will execute the command in address 0 */
	writel(0, (void *)(SOCFPGA_RSTMGR_ADDRESS + 0x10));

	/* Wait for core1 to wake up */
	while (__OS1_awake == 0) {
		/* XXX flush __OS1_awake for cache coherence between 2 cores */
		__asm__ __volatile__ ("" : : : "memory");
	}

	/* XXX warning: you would better not put ether image in address 0x0 */
	for (i = 0; &p[i] < &__OS1_exceptions_end; i++)
		v[i] = b[i];
}

/* this is called in start.S when OS1 starts to run */
void __OS1_reset(void)
{
	__OS1_awake = 0xa5a5a5a5;

	/* TODO */
	/* jump to OS1 */
	/* int (*jump_2_OS1)(void) = 0xabcddcba */
	/* (*jump_2_OS1)(); */
	while (1)
		;
}

static void disable_watchdog(void)
{
	volatile void *reg = (void *)(SOCFPGA_RSTMGR_ADDRESS + 0x14);
	writel(readl(reg) + 0xc0, reg);
}

void lowlevel_init(void)
{
	ulong start, size;

	//disable_watchdog();
#if 1
	/* remap the SDRAM at lower memory instead on-chip RAM */
	writel(0x1, (void *)SOCFPGA_MPUL2_ADDRESS + 0xC00);

	/* do this before pl310_init */
	start = bss_end;
	size = __OS_BASE + (__OS_SIZE / 2) - start;
	mem_malloc_init(start, size);

	__enable_cache();		/* enable L1 cache*/
	pl310_init(0U, ~0UL);		/* enable L2 cache*/

	timer_init();			/* initialize timer */
	clock_init();			/* get clock info */
#endif
	/* wakeup_OS1(); */		/* not for now */
	main();
}


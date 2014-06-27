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
#include <stdio.h>
#include <malloc.h>
#include <config.h>
#include <delay.h>
#include <dwgpio.h>
#include <at24.h>
#include <flash.h>
#include "cache.h"
#include "mm.h"

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
	*((volatile unsigned int *)0xFFD05010) = 0;

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


extern int dwi2c_add_numbered_adapter(int num);

void lowlevel_init(void)
{
	ulong start, size;

	/* remap the SDRAM at lower memory instead on-chip RAM */
	writel(0x1, (void *)0xfffefC00);

	/* do this before pl310_init */
	start = bss_end;
	size = __os_base + (__os_size / 2) - start;
	mem_malloc_init(start, size);

	__enable_cache();		/* enable L1 cache*/
	pl310_init(0U, ~0UL);		/* enable L2 cache*/

	timer_init();			/* initialize timer */

	/* XXX only for OS0 */
	PIO_InitializeInterrupts(0);

	/* do it before at24_init */
	if (dwi2c_add_numbered_adapter(0)) {
		pr_err("%s: failed to probe i2c0\n", __func__);
		goto out;
	}

	if (at24_init()) {
		pr_err("%s: failed to init eeprom\n", __func__);
		goto out;
	}

	if (flash_init()) {
		pr_err("%s: failed to init flash\n", __func__);
		goto out;
	}

out:
	/* wakeup_OS1(); */	/* not for now */
	main();
}


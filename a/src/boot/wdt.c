/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 17:13:05 CST 2014
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

void watchdog_disable(void)
{
	void *wdt_base = (void *)WDT_BASE;

	writel(0xaaaa, wdt_base + WDTREG_WSPR);
	while (readl(wdt_base + WDTREG_WWPS) != 0x0)
		;

	writel(0x5555, wdt_base + WDTREG_WSPR);
	while (readl(wdt_base + WDTREG_WWPS) != 0x0)
		;
}

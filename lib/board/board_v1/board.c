/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 28 14:44:55 CST 2014
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

#include <board.h>

void os_preinit(void)
{
	CPU_IntDis();
	CSP_IntInit();
	CPU_Init();
	CPU_IntDis();

	OSInit();
}

void os_postinit(void)
{
	OS_CSP_TickInit();	/* Initialize the Tick interrupt */
	OSStatInit();		/* Determine CPU capacity */
}

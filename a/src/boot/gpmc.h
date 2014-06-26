/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jun 26 19:09:25 CST 2014
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

#ifndef __GPMC_H__
#define __GPMC_H__

#include <asm/arch/regs.h>

static inline void *gpmc_reg(unsigned int reg)
{
	return (void *)GPMC_BASE + reg;
}

static inline void *gpmc_cs(int cs)
{
	return gpmc_reg(GPMC_CS0) + (0x30 * cs);
}

void gpmc_init(int cs);

#endif

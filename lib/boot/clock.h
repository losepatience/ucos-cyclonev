/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Fri Mar 21 18:26:10 CST 2014
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

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <asm/types.h>

struct clk_info {
	/* main group */
	u32 main_vco_base;
	u32 mpuclk;
	u32 mainclk;
	u32 dbgatclk;
	u32 mainqspiclk;
	u32 mainnandsdmmcclk;
	u32 cfg2fuser0clk;
	u32 maindiv;
	u32 dbgdiv;
	u32 tracediv;
	u32 l4src;

	/* peripheral group */
	u32 peri_vco_base;
	u32 emac0clk;
	u32 emac1clk;
	u32 perqspiclk;
	u32 pernandsdmmcclk;
	u32 perbaseclk;
	u32 s2fuser1clk;
	u32 perdiv;
	u32 gpiodiv;
	u32 persrc;

	/* sdram pll group */
	u32 sdram_vco_base;
	u32 ddrdqsclk;
	u32 ddr2xdqsclk;
	u32 ddrdqclk;
	u32 s2fuser2clk;
};

enum vco {
	main_vco,
	peri_vco,
	sdram_vco,
};

int clock_init(void);

#endif

/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Sat Mar 22 14:21:39 CST 2014
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
#include <config.h>
#include <clock.h>

#define CLKMGR_CTRL_ADDRESS				0x0
#define CLKMGR_BYPASS_ADDRESS				0x4
#define CLKMGR_INTER_ADDRESS				0x8
#define CLKMGR_INTREN_ADDRESS				0xc
#define CLKMGR_DBCTRL_ADDRESS				0x10
#define CLKMGR_STAT_ADDRESS				0x14
#define CLKMGR_MAINPLLGRP_ADDRESS			0x40
#define CLKMGR_MAINPLLGRP_VCO_ADDRESS			0x40
#define CLKMGR_MAINPLLGRP_MISC_ADDRESS			0x44
#define CLKMGR_MAINPLLGRP_MPUCLK_ADDRESS		0x48
#define CLKMGR_MAINPLLGRP_MAINCLK_ADDRESS		0x4c
#define CLKMGR_MAINPLLGRP_DBGATCLK_ADDRESS		0x50
#define CLKMGR_MAINPLLGRP_MAINQSPICLK_ADDRESS		0x54
#define CLKMGR_MAINPLLGRP_MAINNANDSDMMCCLK_ADDRESS	0x58
#define CLKMGR_MAINPLLGRP_CFGS2FUSER0CLK_ADDRESS	0x5c
#define CLKMGR_MAINPLLGRP_EN_ADDRESS			0x60
#define CLKMGR_MAINPLLGRP_MAINDIV_ADDRESS		0x64
#define CLKMGR_MAINPLLGRP_DBGDIV_ADDRESS		0x68
#define CLKMGR_MAINPLLGRP_TRACEDIV_ADDRESS		0x6c
#define CLKMGR_MAINPLLGRP_L4SRC_ADDRESS			0x70
#define CLKMGR_PERPLLGRP_ADDRESS			0x80
#define CLKMGR_PERPLLGRP_VCO_ADDRESS			0x80
#define CLKMGR_PERPLLGRP_MISC_ADDRESS			0x84
#define CLKMGR_PERPLLGRP_EMAC0CLK_ADDRESS		0x88
#define CLKMGR_PERPLLGRP_EMAC1CLK_ADDRESS		0x8c
#define CLKMGR_PERPLLGRP_PERQSPICLK_ADDRESS		0x90
#define CLKMGR_PERPLLGRP_PERNANDSDMMCCLK_ADDRESS	0x94
#define CLKMGR_PERPLLGRP_PERBASECLK_ADDRESS		0x98
#define CLKMGR_PERPLLGRP_S2FUSER1CLK_ADDRESS		0x9c
#define CLKMGR_PERPLLGRP_EN_ADDRESS			0xa0
#define CLKMGR_PERPLLGRP_DIV_ADDRESS			0xa4
#define CLKMGR_PERPLLGRP_GPIODIV_ADDRESS		0xa8
#define CLKMGR_PERPLLGRP_SRC_ADDRESS			0xac
#define CLKMGR_SDRPLLGRP_ADDRESS			0xc0
#define CLKMGR_SDRPLLGRP_VCO_ADDRESS			0xc0
#define CLKMGR_SDRPLLGRP_CTRL_ADDRESS			0xc4
#define CLKMGR_SDRPLLGRP_DDRDQSCLK_ADDRESS		0xc8
#define CLKMGR_SDRPLLGRP_DDR2XDQSCLK_ADDRESS		0xcc
#define CLKMGR_SDRPLLGRP_DDRDQCLK_ADDRESS		0xd0
#define CLKMGR_SDRPLLGRP_S2FUSER2CLK_ADDRESS		0xd4
#define CLKMGR_SDRPLLGRP_EN_ADDRESS			0xd8

static struct clk_info cv_clk;
u32 BSP_OSC_FREQ;

static u32 cacl_vco_base(enum vco vco)
{
	u32 offs, val, cnt, denom;

	switch (vco) {
	case main_vco:
		offs = CLKMGR_MAINPLLGRP_VCO_ADDRESS;
		break;
	case peri_vco:
		offs = CLKMGR_PERPLLGRP_VCO_ADDRESS;
		break;
	case sdram_vco:
		offs = CLKMGR_SDRPLLGRP_VCO_ADDRESS;
		break;
	default:
		break;
	}

	val = readl((void *)SOCFPGA_CLKMGR_ADDRESS + offs);

	cnt = 1 + ((val & 0x0000fff8) >> 3);
	denom = 1 + ((val & 0x003f0000) >> 16);

	return CONFIG_HPS_CLK_OSC1_HZ * cnt / denom;
}

static u32 cacl_divided_clk(u32 scr_clk, u32 offs)
{
	u32 val, div;

	val = readl((void *)SOCFPGA_CLKMGR_ADDRESS + offs);
	div = 1 + ((val & 0x000001ff) >> 0);

	return scr_clk / div;
}

int clock_init(void)
{
	u32 clk;
	void *addr;

	cv_clk.main_vco_base = cacl_vco_base(main_vco);
	cv_clk.peri_vco_base = cacl_vco_base(peri_vco);
	cv_clk.sdram_vco_base = cacl_vco_base(sdram_vco);
	/* clocks derived from main pll */
	clk = cv_clk.main_vco_base;

	cv_clk.mpuclk = cacl_divided_clk(clk / 2, 0x48);
	cv_clk.mainclk = cacl_divided_clk(clk / 4, 0x4c);
	cv_clk.dbgatclk = cacl_divided_clk(clk / 4, 0x50);
	cv_clk.mainqspiclk = cacl_divided_clk(clk, 0x54);
	cv_clk.mainnandsdmmcclk = cacl_divided_clk(clk, 0x58);
	cv_clk.cfg2fuser0clk = cacl_divided_clk(clk, 0x5c);

	/* clocks derived from peripheral pll */
	clk = cv_clk.peri_vco_base;

	cv_clk.emac0clk = cacl_divided_clk(clk, 0x88);
	cv_clk.emac1clk = cacl_divided_clk(clk, 0x8c);
	cv_clk.perqspiclk = cacl_divided_clk(clk, 0x90);
	cv_clk.pernandsdmmcclk = cacl_divided_clk(clk, 0x94);
	cv_clk.perbaseclk = cacl_divided_clk(clk, 0x98);
	cv_clk.s2fuser1clk = cacl_divided_clk(clk, 0x9c);

	/* clocks derived from sdram pll */
	clk = cv_clk.sdram_vco_base;

	cv_clk.ddrdqsclk = cacl_divided_clk(clk, 0xc8);
	cv_clk.ddr2xdqsclk = cacl_divided_clk(clk, 0xcc);
	cv_clk.ddrdqclk = cacl_divided_clk(clk, 0xd0);
	cv_clk.s2fuser2clk = cacl_divided_clk(clk, 0xd4);

	addr = (void *)SOCFPGA_CLKMGR_ADDRESS;

	/* l3 mp and l3 sp */
	cv_clk.maindiv = readl(addr + 0x64);
	cv_clk.dbgdiv = readl(addr + 0x68);
	cv_clk.tracediv = readl(addr + 0x6c);
	cv_clk.l4src = readl(addr + 0x70);

	/* l4 mp, l4 sp, can0, and can1 */
	cv_clk.perdiv = readl(addr + 0xa4);
	cv_clk.gpiodiv = readl(addr + 0xa8);
	cv_clk.persrc = readl(addr + 0xac);

	BSP_OSC_FREQ = cv_clk.mpuclk;
	return 0;
}

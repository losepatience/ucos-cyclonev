/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 16:48:49 CST 2014
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
#include "clock.h"

#define V_OSCK			24000000  /* Clock output from T2 */
#define OSC			(V_OSCK / 1000000)

static const struct dpll_regs dpll_mpu_regs = {
	.cm_clkmode_dpll	= (void *)CM_WKUP + 0x88,
	.cm_idlest_dpll		= (void *)CM_WKUP + 0x20,
	.cm_clksel_dpll		= (void *)CM_WKUP + 0x2C,
	.cm_div_m2_dpll[0]	= (void *)CM_WKUP + 0xA8,
};

static const struct dpll_regs dpll_core_regs = {
	.cm_clkmode_dpll	= (void *)CM_WKUP + 0x90,
	.cm_idlest_dpll		= (void *)CM_WKUP + 0x5C,
	.cm_clksel_dpll		= (void *)CM_WKUP + 0x68,
	.cm_div_m2_dpll[2]	= (void *)CM_WKUP + 0x80,
	.cm_div_m2_dpll[3]	= (void *)CM_WKUP + 0x84,
	.cm_div_m2_dpll[4]	= (void *)CM_WKUP + 0xD8,
};

static const struct dpll_regs dpll_per_regs = {
	.cm_clkmode_dpll	= (void *)CM_WKUP + 0x8C,
	.cm_idlest_dpll		= (void *)CM_WKUP + 0x70,
	.cm_clksel_dpll		= (void *)CM_WKUP + 0x9C,
	.cm_div_m2_dpll[0]	= (void *)CM_WKUP + 0xAC,
};

static const struct dpll_regs dpll_ddr_regs = {
	.cm_clkmode_dpll	= (void *)CM_WKUP + 0x94,
	.cm_idlest_dpll		= (void *)CM_WKUP + 0x34,
	.cm_clksel_dpll		= (void *)CM_WKUP + 0x40,
	.cm_div_m2_dpll[2]	= (void *)CM_WKUP + 0xA0,
};

const struct dpll_params dpll_mpu_opp100 = {
	550, OSC - 1, {1, -1, -1, -1, -1 }
};
const struct dpll_params dpll_core_opp100 = {
	1000, OSC - 1, {-1, -1, 10, 8, 4 }
};
const struct dpll_params dpll_mpu = {
	300, OSC - 1, { 1, -1, -1, -1, -1 }
};
const struct dpll_params dpll_core = {
	50, OSC - 1, { -1, -1, 1, 1, 1 }
};
const struct dpll_params dpll_per = {
	960, OSC - 1, { 5, -1, -1, -1, -1 }
};
const struct dpll_params dpll_ddr_evm_sk = {
	303, OSC - 1, { 1, -1, -1, -1, -1 }
};


static void lock_dpll(const struct dpll_regs *regs)
{
	u32 idlest;

	clrsetbits(regs->cm_clkmode_dpll,
			CM_CLKMODE_DPLL_EN_MASK, DPLL_EN_LOCK);

	do {
		idlest = readl(regs->cm_idlest_dpll);
	} while (!(idlest & ST_DPLL_CLK_MASK));
}

static void setup_post_dividers(const struct dpll_regs *regs,
				const struct dpll_params *params)
{
	int i;

	for (i = 0; i < 5; i++)
		if (params->m2[i] >= 0)
			writel(params->m2[i], regs->cm_div_m2_dpll[i]);
}

static void bypass_dpll(const struct dpll_regs *regs)
{
	u32 idlest;

	clrsetbits(regs->cm_clkmode_dpll,
			CM_CLKMODE_DPLL_EN_MASK, DPLL_EN_MN_BYPASS);

	do {
		idlest = readl(regs->cm_idlest_dpll);
	} while (idlest & ST_DPLL_CLK_MASK);
}

static void do_setup_dpll(const struct dpll_regs *regs,
		const struct dpll_params *params)
{
	u32 tmp = readl(regs->cm_clksel_dpll);

	bypass_dpll(regs);

	/* Set M & N */
	tmp &= ~CM_CLKSEL_DPLL_M_MASK;
	tmp |= (params->m << CM_CLKSEL_DPLL_M_SHIFT) & CM_CLKSEL_DPLL_M_MASK;

	tmp &= ~CM_CLKSEL_DPLL_N_MASK;
	tmp |= (params->n << CM_CLKSEL_DPLL_N_SHIFT) & CM_CLKSEL_DPLL_N_MASK;

	writel(tmp, regs->cm_clksel_dpll);

	/* Setup post-dividers */
	setup_post_dividers(regs, params);

	lock_dpll(regs);
}

void setup_dplls(void)
{
	do_setup_dpll(&dpll_core_regs, &dpll_core);
	do_setup_dpll(&dpll_mpu_regs, &dpll_mpu);
	do_setup_dpll(&dpll_per_regs, &dpll_per);

	writel(0x300, CMWKUP_CLKDCOLDODPLLPER);

	do_setup_dpll(&dpll_ddr_regs, &dpll_ddr_evm_sk);
}


static inline void enable_clk_domain(u32 *reg, u32 mode)
{
	clrsetbits(reg, CD_CLKCTRL_CLKTRCTRL_MASK, mode);
}

static void enable_clk_mod(u32 *addr, u32 mode)
{
	u32 idlest;

	clrsetbits(addr, MODULE_CLKCTRL_MODULEMODE_MASK, mode);

	/* wait for module enabled */
	do {
		idlest = (readl(addr) & MODULE_CLKCTRL_IDLEST_MASK)
			>> MODULE_CLKCTRL_IDLEST_SHIFT;
	} while (idlest & 0x1);
}

static void enable_basic_clocks(void)
{
	int i;

	u32 *const domains[] = {
		CMPER_L3CLKSTCTRL,	CMPER_L4FWCLKSTCTRL,
		CMPER_L3SCLKSTCTRL,	CMPER_L4LSCLKSTCTRL,
		CMWKUP_WKCLKSTCTRL,	CMPER_EMIFFWCLKCTRL,
		CMRTC_CLKSTCTRL,	0
	};

	u32 *const mods[] = {
		CMPER_L3CLKCTRL,	CMPER_L4LSCLKCTRL,
		CMPER_L4FWCLKCTRL,	CMWKUP_WKL4WKCLKCTRL,
		CMPER_L3INSTRCLKCTRL,	CMPER_L4HSCLKCTRL,
		CMWKUP_WKGPIO0CLKCTRL,	CMWKUP_WKCTRLCLKCTRL,
		CMPER_TIMER2CLKCTRL,	CMPER_GPMCCLKCTRL,
		CMPER_ELMCLKCTRL,	CMPER_MMC0CLKCTRL,
		CMPER_MMC1CLKCTRL,	CMWKUP_WKUP_I2C0CTRL,
		CMPER_GPIO1CLKCTRL,	CMPER_GPIO2CLKCTRL,
		CMPER_GPIO3CLKCTRL,	CMPER_I2C1CLKCTRL,
		CMPER_CPGMAC0CLKCTRL,	CMPER_SPI0CLKCTRL,
		CMRTC_RTCCLKCTRL,	CMPER_USB0CLKCTRL,
		CMPER_EMIFFWCLKCTRL,	CMPER_EMIFCLKCTRL,
		0
	};

	for (i = 0; domains[i]; i++)
		enable_clk_domain(domains[i],
			CD_CLKCTRL_CLKTRCTRL_SW_WKUP);

	for (i = 0; mods[i]; i++)
		enable_clk_mod(mods[i],
			MODULE_CLKCTRL_MODULEMODE_SW_EXPLICIT_EN);

	/* Select the Master osc 24 MHZ as Timer2 clock source */
	writel(0x1, CMDPLL_CLKTIMER2CLK);
}

void prcm_init(void)
{
	enable_basic_clocks();
	setup_dplls();
}

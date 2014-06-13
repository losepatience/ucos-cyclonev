/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 11 17:58:47 CST 2014
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
#include <asm/arch/cpu.h>
#include <asm/arch/clock.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>

/* ------------------------------------------------------------------------------
 * wdt
 * ------------------------------------------------------------------------------
 */
#define WDT_BASE		0x44E35000
#define WDTREG_WSPR_OFFS	0x48
#define WDTREG_WWPS_OFFS	0x34

static void watchdog_disable(void)
{
	void *wdt_base = (void *)WDT_BASE;

	writel(0xaaaa, wdt_base + WDTREG_WSPR_OFFS);
	while (readl(wdt_base + WDTREG_WWPS_OFFS) != 0x0)
		;

	writel(0x5555, wdt_base + WDTREG_WSPR_OFFS);
	while (readl(wdt_base + WDTREG_WWPS_OFFS) != 0x0)
		;
}


/* ------------------------------------------------------------------------------
 * on-board rtc
 * ------------------------------------------------------------------------------
 */
#define RTC_BASE		0x44E3E000

#define	RTC_OSC			0x54
#define	RTC_KICK0R		0x6c
#define	RTC_KICK1R		0x70

#define RTC_KICK0R_WE		0x83e70b13
#define RTC_KICK1R_WE		0x95a4f1e0

static void rtc32k_enable(void)
{
	void *rtc_base = (void *)RTC_BASE;

	/*
	 * Unlock the RTC's registers.  For more details please see the
	 * RTC_SS section of the TRM.  In order to unlock we need to
	 * write these specific values (keys) in this order.
	 */
	writel(RTC_KICK0R_WE, rtc_base + RTC_KICK0R);
	writel(RTC_KICK1R_WE, rtc_base + RTC_KICK0R);

	/* Enable the RTC 32K OSC by setting bits 3 and 6. */
	writel((1 << 3) | (1 << 6), rtc_base + RTC_OSC);
}

/* ------------------------------------------------------------------------------
 * pin mux
 * ------------------------------------------------------------------------------
 */

/* PAD Control Fields */
#define SLEWCTRL	(0x1 << 6)
#define RXACTIVE	(0x1 << 5)
#define PULLDOWN_EN	(0x0 << 4)	/* Pull Down Selection */
#define PULLUP_EN	(0x1 << 4)	/* Pull Up Selection */
#define PULLUDEN	(0x0 << 3)	/* Pull up enabled */
#define PULLUDDIS	(0x1 << 3)	/* Pull up disabled */
#define MODE(val)	val		/* used for Readability */

struct module_pin_mux {
	short		reg_offs;
	unsigned int	val;
};

static struct module_pin_mux uart0_pin_mux[] = {
	{ MUX_UART0_RXD(0),	PULLUP_EN | RXACTIVE },	/* UART0_RXD */
	{ MUX_UART0_TXD(0),	PULLUDEN },		/* UART0_TXD */
	{ -1 },
};

static struct module_pin_mux nand_pin_mux[] = {
	{ MUX_GPMC_AD(0),	PULLUP_EN | RXACTIVE },	/* NAND AD0 */
	{ MUX_GPMC_AD(1),	PULLUP_EN | RXACTIVE },	/* NAND AD1 */
	{ MUX_GPMC_AD(2),	PULLUP_EN | RXACTIVE },	/* NAND AD2 */
	{ MUX_GPMC_AD(3),	PULLUP_EN | RXACTIVE },	/* NAND AD3 */
	{ MUX_GPMC_AD(4),	PULLUP_EN | RXACTIVE },	/* NAND AD4 */
	{ MUX_GPMC_AD(5),	PULLUP_EN | RXACTIVE },	/* NAND AD5 */
	{ MUX_GPMC_AD(6),	PULLUP_EN | RXACTIVE },	/* NAND AD6 */
	{ MUX_GPMC_AD(7),	PULLUP_EN | RXACTIVE },	/* NAND AD7 */
	{ MUX_GPMC_WAIT0,	PULLUP_EN | RXACTIVE },	/* NAND WAIT */
	{ MUX_GPMC_WPN,		PULLUP_EN | RXACTIVE },	/* NAND_WPN */
	{ MUX_GPMC_CSN0,	PULLUDEN },		/* NAND_CS0 */
	{ MUX_GPMC_ADVN_ALE,	PULLUDEN },		/* NAND_ADV_ALE */
	{ MUX_GPMC_OEN_REN,	PULLUDEN },		/* NAND_OE */
	{ MUX_GPMC_WEN,		PULLUDEN },		/* NAND_WEN */
	{ MUX_GPMC_BE0N_CLE,	PULLUDEN },		/* NAND_BE_CLE */
	{ -1 },
};

/* Configure the pin mux for the module */
void configure_module_pin_mux(struct module_pin_mux *mux)
{
	void *ctrl_base = (void *)(CTRL_BASE);
	int i;

	for (i = 0; mux[i].reg_offset != -1; i++)
		__raw_writel(mux[i].val, (ctrl_base + mux[i].reg_offs));
}


/* ------------------------------------------------------------------------------
 * pll
 * ------------------------------------------------------------------------------
 */
#include <asm/arch/clock.h>

#define V_OSCK			24000000  /* Clock output from T2 */
#define OSC			(V_OSCK / 1000000)

struct cm_perpll *const cmper = (struct cm_perpll *)CM_PER;
struct cm_wkuppll *const cmwkup = (struct cm_wkuppll *)CM_WKUP;
struct cm_dpll *const cmdpll = (struct cm_dpll *)CM_DPLL;
struct cm_rtc *const cmrtc = (struct cm_rtc *)CM_RTC;

const struct dpll_regs dpll_mpu_regs = {
	.cm_clkmode_dpll	= CM_WKUP + 0x88,
	.cm_idlest_dpll		= CM_WKUP + 0x20,
	.cm_clksel_dpll		= CM_WKUP + 0x2C,
	.cm_div_m2_dpll		= CM_WKUP + 0xA8,
};

const struct dpll_regs dpll_core_regs = {
	.cm_clkmode_dpll	= CM_WKUP + 0x90,
	.cm_idlest_dpll		= CM_WKUP + 0x5C,
	.cm_clksel_dpll		= CM_WKUP + 0x68,
	.cm_div_m4_dpll		= CM_WKUP + 0x80,
	.cm_div_m5_dpll		= CM_WKUP + 0x84,
	.cm_div_m6_dpll		= CM_WKUP + 0xD8,
};

const struct dpll_regs dpll_per_regs = {
	.cm_clkmode_dpll	= CM_WKUP + 0x8C,
	.cm_idlest_dpll		= CM_WKUP + 0x70,
	.cm_clksel_dpll		= CM_WKUP + 0x9C,
	.cm_div_m2_dpll		= CM_WKUP + 0xAC,
};

const struct dpll_regs dpll_ddr_regs = {
	.cm_clkmode_dpll	= CM_WKUP + 0x94,
	.cm_idlest_dpll		= CM_WKUP + 0x34,
	.cm_clksel_dpll		= CM_WKUP + 0x40,
	.cm_div_m2_dpll		= CM_WKUP + 0xA0,
};

struct dpll_params dpll_mpu_opp100 = {
	550, OSC - 1, 1, -1, -1, -1, -1
};

const struct dpll_params dpll_core_opp100 = {
	1000, OSC - 1, -1, -1, 10, 8, 4
};

const struct dpll_params dpll_mpu = {
	300, OSC - 1, 1, -1, -1, -1, -1
};

const struct dpll_params dpll_core = {
	50, OSC - 1, -1, -1, 1, 1, 1
};

const struct dpll_params dpll_per = {
	960, OSC - 1, 5, -1, -1, -1, -1
};

const struct dpll_params *get_dpll_mpu_params(void)
{
	return &dpll_mpu;
}

const struct dpll_params *get_dpll_core_params(void)
{
	return &dpll_core;
}

const struct dpll_params *get_dpll_per_params(void)
{
	return &dpll_per;
}

void setup_clocks_for_console(void)
{
	clrsetbits(&cmwkup->wkclkstctrl, CD_CLKCTRL_CLKTRCTRL_MASK,
			CD_CLKCTRL_CLKTRCTRL_SW_WKUP <<
			CD_CLKCTRL_CLKTRCTRL_SHIFT);

	clrsetbits(&cmper->l4hsclkstctrl, CD_CLKCTRL_CLKTRCTRL_MASK,
			CD_CLKCTRL_CLKTRCTRL_SW_WKUP <<
			CD_CLKCTRL_CLKTRCTRL_SHIFT);

	clrsetbits(&cmwkup->wkup_uart0ctrl,
			MODULE_CLKCTRL_MODULEMODE_MASK,
			MODULE_CLKCTRL_MODULEMODE_SW_EXPLICIT_EN <<
			MODULE_CLKCTRL_MODULEMODE_SHIFT);
}

void enable_basic_clocks(void)
{
	u32 *const clk_domains[] = {
		&cmper->l3clkstctrl,
		&cmper->l4fwclkstctrl,
		&cmper->l3sclkstctrl,
		&cmper->l4lsclkstctrl,
		&cmwkup->wkclkstctrl,
		&cmper->emiffwclkctrl,
		&cmrtc->clkstctrl,
		0
	};

	u32 *const clk_modules_explicit_en[] = {
		&cmper->l3clkctrl,
		&cmper->l4lsclkctrl,
		&cmper->l4fwclkctrl,
		&cmwkup->wkl4wkclkctrl,
		&cmper->l3instrclkctrl,
		&cmper->l4hsclkctrl,
		&cmwkup->wkgpio0clkctrl,
		&cmwkup->wkctrlclkctrl,
		&cmper->timer2clkctrl,
		&cmper->gpmcclkctrl,
		&cmper->elmclkctrl,
		&cmper->mmc0clkctrl,
		&cmper->mmc1clkctrl,
		&cmwkup->wkup_i2c0ctrl,
		&cmper->gpio1clkctrl,
		&cmper->gpio2clkctrl,
		&cmper->gpio3clkctrl,
		&cmper->i2c1clkctrl,
		&cmper->cpgmac0clkctrl,
		&cmper->spi0clkctrl,
		&cmrtc->rtcclkctrl,
		&cmper->usb0clkctrl,
		&cmper->emiffwclkctrl,
		&cmper->emifclkctrl,
		0
	};

	do_enable_clocks(clk_domains, clk_modules_explicit_en, 1);

	/* Select the Master osc 24 MHZ as Timer2 clock source */
	writel(0x1, &cmdpll->clktimer2clk);
}

void prcm_init(void)
{
	enable_basic_clocks();
	setup_dplls();
}

void init()
{
	watchdog_disable();

	configure_module_pin_mux(uart0_pin_mux);
	prcm_init();

	configure_module_pin_mux(nand_pin_mux);
}



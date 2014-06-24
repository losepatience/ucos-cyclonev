/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 17:13:50 CST 2014
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
#include "mux.h"

/* PAD Control Fields */
#define SLEWCTRL	(0x1 << 6)
#define RXACTIVE	(0x1 << 5)
#define PULLDOWN_EN	(0x0 << 4)	/* Pull Down Selection */
#define PULLUP_EN	(0x1 << 4)	/* Pull Up Selection */
#define PULLUDEN	(0x0 << 3)	/* Pull up enabled */
#define PULLUDDIS	(0x1 << 3)	/* Pull up disabled */
#define MODE(val)	val		/* used for Readability */

struct module_pin_mux uart0_pin_mux[] = {
	{ __pin(uart0_rxd),	PULLUP_EN | RXACTIVE },
	{ __pin(uart0_txd),	PULLUDEN },
	{ -1 },
};

struct module_pin_mux uart1_pin_mux[] = {
	{ __pin(uart1_rxd),	PULLUP_EN | RXACTIVE },
	{ __pin(uart1_txd),	PULLUDEN },
	{ -1 },
};

struct module_pin_mux uart2_pin_mux[] = {
	{ __pin(spi0_sclk),	0x1 | PULLUP_EN | RXACTIVE },
	{ __pin(spi0_d0),	0x1 | PULLUDEN },
	{ -1 },
};

struct module_pin_mux uart3_pin_mux[] = {
	{ __pin(spi0_cs1),	0x1 | PULLUP_EN | RXACTIVE },
	{ __pin(ecap0_in_pwm0_out),	0x1 | PULLUDEN },
	{ -1 },
};

struct module_pin_mux uart4_pin_mux[] = {
	{ __pin(gpmc_wait0),	0x6 | PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_wpn),	0x6 | PULLUDEN },
	{ -1 },
};

struct module_pin_mux uart5_pin_mux[] = {
	{ __pin(lcd_data9),	0x4 | PULLUP_EN | RXACTIVE },
	{ __pin(lcd_data8),	0x4 | PULLUDEN },
	{ -1 },
};

struct module_pin_mux mmc0_pin_mux[] = {
	{ __pin(mmc0_dat3),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat2),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat1),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat0),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_clk),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_cmd),	RXACTIVE | PULLUP_EN },
	{ __pin(mcasp0_aclkr),	0x4 | RXACTIVE },
	{ __pin(spi0_cs1),	0x5 | RXACTIVE | PULLUP_EN },
	{ -1 },
};

struct module_pin_mux mmc0_no_cd_pin_mux[] = {
	{ __pin(mmc0_dat3),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat2),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat1),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat0),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_clk),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_cmd),	RXACTIVE | PULLUP_EN },
	{ __pin(mcasp0_aclkr),	0x4 | RXACTIVE },
	{ -1 },
};

struct module_pin_mux mmc0_pin_mux_sk_evm[] = {
	{ __pin(mmc0_dat3),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat2),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat1),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_dat0),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_clk),	RXACTIVE | PULLUP_EN },
	{ __pin(mmc0_cmd),	RXACTIVE | PULLUP_EN },
	{ __pin(spi0_cs1),	0x5 | RXACTIVE | PULLUP_EN },
	{ -1 },
};

struct module_pin_mux mmc1_pin_mux[] = {
	{ __pin(gpmc_ad3),	0x1 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_ad2),	0x1 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_ad1),	0x1 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_ad0),	0x1 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_csn1),	0x2 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_csn2),	0x2 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_csn0),	0x7 | RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_advn_ale),	0x7 | RXACTIVE | PULLUP_EN },
	{ -1 },
};

struct module_pin_mux i2c0_pin_mux[] = {
	{ __pin(i2c0_sda),	RXACTIVE | PULLUDEN | SLEWCTRL },
	{ __pin(i2c0_scl),	RXACTIVE | PULLUDEN | SLEWCTRL },
	{ -1 },
};

struct module_pin_mux i2c1_pin_mux[] = {
	{ __pin(spi0_d1),	0x2 | RXACTIVE | PULLUDEN | SLEWCTRL },
	{ __pin(spi0_cs0),	0x2 | RXACTIVE | PULLUDEN | SLEWCTRL },
	{ -1 },
};

struct module_pin_mux spi0_pin_mux[] = {
	{ __pin(spi0_sclk),	RXACTIVE | PULLUDEN },
	{ __pin(spi0_d0),	RXACTIVE | PULLUDEN | PULLUP_EN },
	{ __pin(spi0_d1),	RXACTIVE | PULLUDEN },
	{ __pin(spi0_cs0),	RXACTIVE | PULLUDEN | PULLUP_EN },
	{ -1 },
};

struct module_pin_mux gpio0_7_pin_mux[] = {
	{ __pin(ecap0_in_pwm0_out),	0x7 | PULLUDEN },
	{ -1 },
};

struct module_pin_mux rgmii1_pin_mux[] = {
	{ __pin(mii1_txen),	0x2 },
	{ __pin(mii1_rxdv),	0x2 | RXACTIVE },
	{ __pin(mii1_txd3),	0x2 },
	{ __pin(mii1_txd2),	0x2 },
	{ __pin(mii1_txd1),	0x2 },
	{ __pin(mii1_txd0),	0x2 },
	{ __pin(mii1_txclk),	0x2 },
	{ __pin(mii1_rxclk),	0x2 | RXACTIVE },
	{ __pin(mii1_rxd3),	0x2 | RXACTIVE },
	{ __pin(mii1_rxd2),	0x2 | RXACTIVE },
	{ __pin(mii1_rxd1),	0x2 | RXACTIVE },
	{ __pin(mii1_rxd0),	0x2 | RXACTIVE },
	{ __pin(mdio_data),	RXACTIVE | PULLUP_EN },
	{ __pin(mdio_clk),	PULLUP_EN },
	{ -1 },
};

struct module_pin_mux mii1_pin_mux[] = {
	{ __pin(mii1_rxerr),	RXACTIVE },
	{ __pin(mii1_txen),	0x0 },
	{ __pin(mii1_rxdv),	RXACTIVE },
	{ __pin(mii1_txd3),	0x0 },
	{ __pin(mii1_txd2),	0x0 },
	{ __pin(mii1_txd1),	0x0 },
	{ __pin(mii1_txd0),	0x0 },
	{ __pin(mii1_txclk),	RXACTIVE },
	{ __pin(mii1_rxclk),	RXACTIVE },
	{ __pin(mii1_rxd3),	RXACTIVE },
	{ __pin(mii1_rxd2),	RXACTIVE },
	{ __pin(mii1_rxd1),	RXACTIVE },
	{ __pin(mii1_rxd0),	RXACTIVE },
	{ __pin(mdio_data),	RXACTIVE | PULLUP_EN},
	{ __pin(mdio_clk),	PULLUP_EN },
	{ -1 },
};

struct module_pin_mux nand_pin_mux[] = {
	{ __pin(gpmc_ad0),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad1),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad2),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad3),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad4),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad5),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad6),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_ad7),	PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_wait0),	RXACTIVE | PULLUP_EN },
	{ __pin(gpmc_wpn),	0x7 | PULLUP_EN | RXACTIVE },
	{ __pin(gpmc_csn0),	PULLUDEN },
	{ __pin(gpmc_advn_ale),	PULLUDEN },
	{ __pin(gpmc_oen_ren),	PULLUDEN },
	{ __pin(gpmc_wen),	PULLUDEN },
	{ __pin(gpmc_be0n_cle),	PULLUDEN },
	{ -1 },
};


/* Configure the pin mux for the module */
void configure_module_pin_mux(struct module_pin_mux *mux)
{
	void *ctrl_base = (void *)(PRCM_BASE);
	int i;

	for (i = 0; mux[i].offs != -1; i++)
		__raw_writel(mux[i].val, (ctrl_base + mux[i].offs));
}

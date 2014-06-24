/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 18:52:30 CST 2014
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

#ifndef __MUX_H__
#define __MUX_H__

struct module_pin_mux {
	int		offs;
	unsigned int	val;
};

extern struct module_pin_mux uart0_pin_mux[];
extern struct module_pin_mux uart1_pin_mux[];
extern struct module_pin_mux uart2_pin_mux[];
extern struct module_pin_mux uart3_pin_mux[];
extern struct module_pin_mux uart4_pin_mux[];
extern struct module_pin_mux uart5_pin_mux[];
extern struct module_pin_mux mmc0_pin_mux[];
extern struct module_pin_mux mmc0_no_cd_pin_mux[];
extern struct module_pin_mux mmc0_pin_mux_sk_evm[];
extern struct module_pin_mux mmc1_pin_mux[];
extern struct module_pin_mux i2c0_pin_mux[];
extern struct module_pin_mux i2c1_pin_mux[];
extern struct module_pin_mux spi0_pin_mux[];
extern struct module_pin_mux gpio0_7_pin_mux[];
extern struct module_pin_mux rgmii1_pin_mux[];
extern struct module_pin_mux mii1_pin_mux[];
extern struct module_pin_mux nand_pin_mux[];

void configure_module_pin_mux(struct module_pin_mux *mux);

#endif

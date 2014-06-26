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
#include <asm/arch/regs.h>
#include <asm/io.h>
#include "boot/delay.h"
#include "boot/mux.h"



void lowlevel_init(void)
{

	/*s_init*/
	watchdog_disable();
	timer_init();
	prcm_init();

	configure_module_pin_mux(nand_pin_mux);
	configure_module_pin_mux(i2c0_pin_mux);



#if 0
	config_ddr(303, MT41J512M8RH125_IOCTRL_VALUE, &ddr3_evm_data,
			&ddr3_evm_cmd_ctrl_data, &ddr3_evm_emif_reg_data, 0);
#endif
	/*s_init end*/

}


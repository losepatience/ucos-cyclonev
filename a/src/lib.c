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

#define GPMC_SIZE_256M		0x0
#define CONFIG_SYS_NAND_BASE	0x8000000

#define M_NAND_GPMC_CONFIG1	0x00000800
#define M_NAND_GPMC_CONFIG2	0x001e1e00
#define M_NAND_GPMC_CONFIG3	0x001e1e00
#define M_NAND_GPMC_CONFIG4	0x16051807
#define M_NAND_GPMC_CONFIG5	0x00151e1e
#define M_NAND_GPMC_CONFIG6	0x16000f80
#define M_NAND_GPMC_CONFIG7	0x00000008

void enable_gpmc_cs_config(u32 *cfg, void *cs_base, u32 base, u32 size)
{
	int i;

	writel(0, cs_base + GPMC_CS_CONFIG7);
	sdelay(1000);

	/* delay for settling */
	for (i = 0; cfg[i] != 0; i++)
		writel(cfg[i], cs_base + (0x4 * i));

	/* enable the config */
	writel(((size & 0xF) << 8)
		| ((base >> 24) & 0x3F)
		| (1 << 6),
		cs_base + GPMC_CS_CONFIG7);

	sdelay(2000);
}

void gpmc_init(int cs)
{
	/* putting a blanket check on GPMC based on ZeBu for now */
	void *gpmc_base = (void *)GPMC_BASE;
	void *gpmc_cs_base = gpmc_base + GPMC_CS0 + (0x30 * cs);

	/* configure GPMC for NAND */
	const u32 gpmc_regs[7] = {
		M_NAND_GPMC_CONFIG1,
		M_NAND_GPMC_CONFIG2,
		M_NAND_GPMC_CONFIG3,
		M_NAND_GPMC_CONFIG4,
		M_NAND_GPMC_CONFIG5,
		M_NAND_GPMC_CONFIG6,
		0
	};

	u32 size = GPMC_SIZE_256M;
	u32 base = CONFIG_SYS_NAND_BASE;

	/* global settings */
	writel(0x00000008, gpmc_base + GPMCREG_SYSCONFIG);
	writel(0x00000000, gpmc_base + GPMCREG_IRQSTATUS);
	writel(0x00000000, gpmc_base + GPMCREG_IRQENABLE);

	writel(0x00000012, gpmc_base + GPMCREG_CONFIG);

	/* enable chip-select specific configurations */
	enable_gpmc_cs_config(gpmc_regs, gpmc_cs_base, base, size);
}


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


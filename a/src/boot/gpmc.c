/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jun 26 19:08:24 CST 2014
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
#include "gpmc.h"
#include "mem.h"
#include "delay.h"

#ifndef SYS_NAND_BASE
# define SYS_NAND_BASE	0x8000000
#endif
static void gpmc_cs_config(u32 *cfg, int cs, u32 base, u32 size)
{
	void *reg = gpmc_cs(cs);

	writel(0, reg + GPMC_CS_CONFIG7);
	sdelay(1000);

	/* delay for settling */
	while (*cfg) {
		writel(*cfg++, reg);
		reg += 0x00000004;
	}

	/* enable the config */
	writel(((size & 0xF) << 8) | ((base >> 24) & 0x3F) | (1 << 6), reg);

	sdelay(2000);
}

void gpmc_init(int cs)
{
	/* configure GPMC for NAND */
	u32 gpmc_regs[7] = {
		M_NAND_GPMC_CONFIG1,
		M_NAND_GPMC_CONFIG2,
		M_NAND_GPMC_CONFIG3,
		M_NAND_GPMC_CONFIG4,
		M_NAND_GPMC_CONFIG5,
		M_NAND_GPMC_CONFIG6,
		0
	};

	u32 size = GPMC_SIZE_256M;
	u32 base = SYS_NAND_BASE;

	/* global settings */
	writel(0x00000008, gpmc_reg(GPMC_SYSCONFIG));
	writel(0x00000000, gpmc_reg(GPMC_IRQSTATUS));
	writel(0x00000000, gpmc_reg(GPMC_IRQENABLE));

	writel(0x00000012, gpmc_reg(GPMC_CONFIG));

	/* enable chip-select specific configurations */
	gpmc_cs_config(gpmc_regs, cs, base, size);
}

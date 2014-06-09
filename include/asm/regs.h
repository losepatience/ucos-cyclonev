/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
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

#ifndef __REGS_H__
#define __REGS_H__

/* io register base */
#define SOCFPGA_EMAC0_ADDRESS 0xff700000
#define SOCFPGA_EMAC1_ADDRESS 0xff702000
#define SOCFPGA_SDMMC_ADDRESS 0xff704000
#define SOCFPGA_QSPIREGS_ADDRESS 0xff705000
#define SOCFPGA_L3REGS_ADDRESS 0xff800000
#define SOCFPGA_QSPIREGS_ADDRESS 0xff705000
#define SOCFPGA_FPGAMGRREGS_ADDRESS 0xff706000
#define SOCFPGA_QSPIDATA_ADDRESS 0xffa00000
#define SOCFPGA_FPGAMGRDATA_ADDRESS 0xffb90000
#define SOCFPGA_UART0_ADDRESS 0xffc02000
#define SOCFPGA_UART1_ADDRESS 0xffc03000
#define SOCFPGA_I2C0_ADDRESS 0xffc04000
#define SOCFPGA_GPIO0_ADDRESS 0xFF708000
#define SOCFPGA_SDR_ADDRESS 0xffc20000
#define SOCFPGA_OSC1TIMER0_ADDRESS 0xffd00000
#define SOCFPGA_L4WD0_ADDRESS 0xffd02000
#define SOCFPGA_CLKMGR_ADDRESS 0xffd04000
#define SOCFPGA_RSTMGR_ADDRESS 0xffd05000
#define SOCFPGA_SYSMGR_ADDRESS 0xffd08000
#define SOCFPGA_SCANMGR_ADDRESS 0xfff02000
#define SOCFPGA_MPUSCU_ADDRESS 0xfffec000
#define SOCFPGA_MPUL2_ADDRESS 0xfffef000
#define SOCFPGA_LWH2F_ADDRESS 0xff200000

/* security register group */
#define SOCFPGA_L3REGS_SECGRP 0x8

/* sdram control register group */
#define SDR_CTRLGRP_ADDRESS 0x5000

/* FPGA interface group */
#define CONFIG_SYSMGR_FPGAINTF_MODULE	(SOCFPGA_SYSMGR_ADDRESS + 0x28)

/* Misc */
#define CONFIG_SYSMGR_EMAC_CTRL		(SOCFPGA_SYSMGR_ADDRESS + 0x60)
#define CONFIG_CPU1_START_ADDR		(SOCFPGA_SYSMGR_ADDRESS + 0xc4)
#define CONFIG_SYSMGR_ROMCODEGRP_CTRL	(SOCFPGA_SYSMGR_ADDRESS + 0xc0)

/* ECC */
#define CONFIG_SYSMGR_ECCGRP		(SOCFPGA_SYSMGR_ADDRESS + 0x140)
#define CONFIG_SYSMGR_ECC_OCRAM		(CONFIG_SYSMGR_ECCGRP + 0x4)

/* pin mux */
#define CONFIG_SYSMGR_PINMUXGRP		(SOCFPGA_SYSMGR_ADDRESS + 0x400)
#define CONFIG_SYSMGR_NANDUSEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x2F0)
#define CONFIG_SYSMGR_EMAC1USEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x2F8)
#define CONFIG_SYSMGR_SDMMCUSEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x308)
#define CONFIG_SYSMGR_EMAC0USEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x314)
#define CONFIG_SYSMGR_SPIM1USEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x330)
#define CONFIG_SYSMGR_SPIM0USEFPGA	(CONFIG_SYSMGR_PINMUXGRP + 0x338)

#endif

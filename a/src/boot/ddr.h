/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 25 09:43:52 CST 2014
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

#ifndef __DDR_H__
#define __DDR_H__


/* Micron MT41J512M8RH-125 on EVM v1.5 */
#define MT41J512M8RH125_EMIF_READ_LATENCY	0x100006
#define MT41J512M8RH125_EMIF_TIM1		0x0888A39B
#define MT41J512M8RH125_EMIF_TIM2		0x26517FDA
#define MT41J512M8RH125_EMIF_TIM3		0x501F84EF
#define MT41J512M8RH125_EMIF_SDCFG		0x61C04BB2
#define MT41J512M8RH125_EMIF_SDREF		0x0000093B
#define MT41J512M8RH125_ZQ_CFG			0x50074BE4
#define MT41J512M8RH125_RATIO			0x80
#define MT41J512M8RH125_INVERT_CLKOUT		0x0
#define MT41J512M8RH125_RD_DQS			0x3B
#define MT41J512M8RH125_WR_DQS			0x3C
#define MT41J512M8RH125_PHY_FIFO_WE		0xA5
#define MT41J512M8RH125_PHY_WR_DATA		0x74
#define MT41J512M8RH125_IOCTRL_VALUE		0x18B

struct ddr_data {
	unsigned long datardsratio0;
	unsigned long datawdsratio0;
	unsigned long datawiratio0;
	unsigned long datagiratio0;
	unsigned long datafwsratio0;
	unsigned long datawrsratio0;
};


struct cmd_control {
	unsigned long cmd0csratio;
	unsigned long cmd0csforce;
	unsigned long cmd0csdelay;
	unsigned long cmd0iclkout;
	unsigned long cmd1csratio;
	unsigned long cmd1csforce;
	unsigned long cmd1csdelay;
	unsigned long cmd1iclkout;
	unsigned long cmd2csratio;
	unsigned long cmd2csforce;
	unsigned long cmd2csdelay;
	unsigned long cmd2iclkout;
};

struct emif_regs {
	u32 freq;
	u32 sdram_config_init;
	u32 sdram_config;
	u32 sdram_config2;
	u32 ref_ctrl;
	u32 sdram_tim1;
	u32 sdram_tim2;
	u32 sdram_tim3;
	u32 read_idle_ctrl;
	u32 zq_config;
	u32 temp_alert_config;
	u32 emif_ddr_phy_ctlr_1_init;
	u32 emif_ddr_phy_ctlr_1;
	u32 emif_ddr_ext_phy_ctrl_1;
	u32 emif_ddr_ext_phy_ctrl_2;
	u32 emif_ddr_ext_phy_ctrl_3;
	u32 emif_ddr_ext_phy_ctrl_4;
	u32 emif_ddr_ext_phy_ctrl_5;
	u32 emif_rd_wr_lvl_rmp_win;
	u32 emif_rd_wr_lvl_rmp_ctl;
	u32 emif_rd_wr_lvl_ctl;
	u32 emif_rd_wr_exec_thresh;
	u32 int_config;
};

#endif

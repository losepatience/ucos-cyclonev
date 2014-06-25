/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 25 09:45:59 CST 2014
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
#include "ddr.h"

/* AM335X EMIF Register values */
#define VTP_CTRL_READY			(0x1 << 5)
#define VTP_CTRL_ENABLE			(0x1 << 6)
#define VTP_CTRL_START_EN		(0x1)
#define DDR_CKE_CTRL_NORMAL		(0x1)
#define PHY_EN_DYN_PWRDN		(0x1 << 20)

static const struct cmd_control ddr3_evm_cmd_ctrl_data = {
	.cmd0csratio = MT41J512M8RH125_RATIO,
	.cmd0iclkout = MT41J512M8RH125_INVERT_CLKOUT,

	.cmd1csratio = MT41J512M8RH125_RATIO,
	.cmd1iclkout = MT41J512M8RH125_INVERT_CLKOUT,

	.cmd2csratio = MT41J512M8RH125_RATIO,
	.cmd2iclkout = MT41J512M8RH125_INVERT_CLKOUT,
};

struct addr_info {
	void	*vtp_ctrl_addr;
	void	*ddr_phycmd_addr;
	void	*ddr_phydat_addr[DDR_DATA_REGS_NR];
	void	*emif4_cfg_addr;
};

static struct addr_info addr_info = {
	.vtp_ctrl_addr		= (void *)VTP0_CTRL_ADDR,
	.ddr_phycmd_addr	= (void *)DDR_PHY_CMD_ADDR,
	.ddr_phydat_addr[0]	= (void *)DDR_PHY_DATA_ADDR,
	.ddr_phydat_addr[1]	= (void *)DDR_PHY_DATA_ADDR2,
	.emif4_cfg_addr		= (void *)EMIF4_0_CFG_BASE,
};

static void config_vtp(void)
{
	void *addr = addr_info.vtp_ctrl_addr;

	writel(readl(addr) | VTP_CTRL_ENABLE, addr);
	writel(readl(addr) & (~VTP_CTRL_START_EN), addr);
	writel(readl(addr) | VTP_CTRL_START_EN, addr);

	/* Poll for READY */
	while ((readl(addr) & VTP_CTRL_READY) != VTP_CTRL_READY)
		;
}

/* Configure DDR CMD control registers */
static void config_cmd_ctrl(const struct cmd_control *cmd)
{
	void *base = addr_info.ddr_phycmd_addr;

	writel(cmd->cmd0csratio, base + DDRCMD_CM0CSRATIO);
	writel(cmd->cmd0iclkout, base + DDRCMD_CM0ICLKOUT);

	writel(cmd->cmd1csratio, base + DDRCMD_CM1CSRATIO);
	writel(cmd->cmd1iclkout, base + DDRCMD_CM1ICLKOUT);

	writel(cmd->cmd2csratio, base + DDRCMD_CM2CSRATIO);
	writel(cmd->cmd2iclkout, base + DDRCMD_CM2ICLKOUT);
}

/* Configure DDR DATA registers */
static void config_ddr_data(const struct ddr_data *data)
{
	int i;

	for (i = 0; i < DDR_DATA_REGS_NR; i++) {
		void *base = addr_info.ddr_phydat_addr[i];

		writel(data->datardsratio0, base + DDRDAT_DT0RDSRATIO0);
		writel(data->datawdsratio0, base + DDRDAT_DT0WDSRATIO0);
		writel(data->datawiratio0, base + DDRDAT_DT0WIRATIO0);
		writel(data->datagiratio0, base + DDRDAT_DT0GIRATIO0);
		writel(data->datafwsratio0, base + DDRDAT_DT0FWSRATIO0);
		writel(data->datawrsratio0, base + DDRDAT_DT0WRSRATIO0);
	}
}

static void config_io_ctrl(unsigned long val)
{
	void *base = (void *)DDR_CONTROL_BASE_ADDR;

	writel(val, base + DDRCON_CM0IOCTL);
	writel(val, base + DDRCON_CM1IOCTL);
	writel(val, base + DDRCON_CM2IOCTL);
	writel(val, base + DDRCON_DT0IOCTL);
	writel(val, base + DDRCON_DT1IOCTL);
}

static void config_ddr_phy(const struct emif_regs *regs)
{
	void *base = addr_info.emif4_cfg_addr;

	writel(regs->emif_ddr_phy_ctlr_1, base + EMIF_DDR_PHY_CTRL_1);
	writel(regs->emif_ddr_phy_ctlr_1, base + EMIF_DDR_PHY_CTRL_1_SHDW);
}


static void set_sdram_timings(const struct emif_regs *regs)
{
	void *base = addr_info.emif4_cfg_addr;

	writel(regs->sdram_tim1, base + EMIF_SDRAM_TIM_1);
	writel(regs->sdram_tim1, base + EMIF_SDRAM_TIM_1_SHDW);
	writel(regs->sdram_tim2, base + EMIF_SDRAM_TIM_2);
	writel(regs->sdram_tim2, base + EMIF_SDRAM_TIM_2_SHDW);
	writel(regs->sdram_tim3, base + EMIF_SDRAM_TIM_3);
	writel(regs->sdram_tim3, base + EMIF_SDRAM_TIM_3_SHDW);
}


/* Configure SDRAM */
static void config_sdram(const struct emif_regs *regs)
{
	u32 int_config;
	void *base = addr_info.emif4_cfg_addr;

	if (regs->int_config)
		int_config = regs->int_config;
	else
		int_config = 0x00FFFF10;

	if (regs->zq_config) {
		void *ctl_base = (void *)CTRL_BASE;
		/*
		 * A value of 0x2800 for the REF CTRL will give us
		 * about 570us for a delay, which will be long enough
		 * to configure things.
		 */
		writel(0x2800, base + EMIF_SDRAM_REF_CTRL);
		writel(regs->zq_config, base + EMIF_ZQ_CONFIG);
		writel(regs->sdram_config, ctl_base + SECURE_EMIF_SDRAM_CONFIG);
		writel(regs->sdram_config, base + EMIF_SDRAM_CONFIG);
		writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL);
		writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL_SHDW);
	}

	/*
	 * The INT_CONFIG register on AM335x or PBBPR on TI81XX serves
	 * similar purposes to the L3_CONFIG register on OMAP4 and lower
	 * (or OCP_CONFIG on OMAP5 and higher).
	 */

#ifndef BOARD_OTHERS
	writel(int_config, base + EMIF_L3_CONFIG);
#endif

	writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL);
	writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL_SHDW);
	writel(regs->sdram_config, base + EMIF_SDRAM_CONFIG);
}

void config_ddr(unsigned int pll,
		unsigned int ioctrl,
		const struct ddr_data *data,
		const struct cmd_control *ctrl,
		const struct emif_regs *regs)
{
	void *ddr_ctrl_base = (void *)DDR_CTRL_ADDR;

#ifndef BOARD_TI816X
	config_vtp();
#endif

	config_cmd_ctrl(ctrl);

	config_ddr_data(data);

#ifndef BOARD_OTHERS
	config_io_ctrl(ioctrl);

	/* Set CKE to be controlled by EMIF/DDR PHY */
	writel(DDR_CKE_CTRL_NORMAL, ddr_ctrl_base + DDR_CTRL_DDRCKECTRL);
#endif

	/* Program EMIF instance */
	config_ddr_phy(regs);
	set_sdram_timings(regs);
	config_sdram(regs);
}

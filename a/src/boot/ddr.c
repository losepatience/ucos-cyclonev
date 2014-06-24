/*
 * emif4.c
 *
 * AM33XX emif4 configuration file
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/regs.h>
#include <asm/io.h>
#include "ddr.h"


#define STATUSREG			0X040
#define SECURE_EMIF_SDRAM_CONFIG	0X0110

#define PHYCMD_CM0CSRATIO		0X01C
#define PHYCMD_CM0ICLKOUT		0X02C
#define PHYCMD_CM1CSRATIO		0X050
#define PHYCMD_CM1ICLKOUT		0X060
#define PHYCMD_CM2CSRATIO		0X084
#define PHYCMD_CM2ICLKOUT		0X094

#define PHYDAT_DT0RDSRATIO0		0X0C8
#define PHYDAT_DT0WDSRATIO0		0X0DC
#define PHYDAT_DT0WIRATIO0		0X0F0
#define PHYDAT_DT0WIMODE0		0X0F8
#define PHYDAT_DT0GIRATIO0		0X0FC
#define PHYDAT_DT0GIMODE0		0X104
#define PHYDAT_DT0FWSRATIO0		0X108
#define PHYDAT_DT0DQOFFSET		0X11C
#define PHYDAT_DT0WRSRATIO0		0X120
#define PHYDAT_DT0RDELAYS0		0X134
#define PHYDAT_DT0DLDIFF0		0X138

#define CON_CM0IOCTL			0X000
#define CON_CM1IOCTL			0X004
#define CON_CM2IOCTL			0X008
#define CON_DT0IOCTL			0X03C
#define CON_DT1IOCTL			0X040
#define CON_DT2IOCTRL			0X044
#define CON_DT3IOCTRL			0X048

#define EMIF_SDRAM_CONFIG		0X008
#define EMIF_SDRAM_REF_CTRL		0X010
#define EMIF_SDRAM_REF_CTRL_SHDW	0X014
#define EMIF_SDRAM_TIM_1		0X018
#define EMIF_SDRAM_TIM_1_SHDW		0X01C
#define EMIF_SDRAM_TIM_2		0X020
#define EMIF_SDRAM_TIM_2_SHDW		0X024
#define EMIF_SDRAM_TIM_3		0X028
#define EMIF_SDRAM_TIM_3_SHDW		0X02C
#define EMIF_L3_CONFIG			0X054
#define EMIF_ZQ_CONFIG			0X0CC
#define EMIF_DDR_PHY_CTRL_1		0X0E8
#define EMIF_DDR_PHY_CTRL_1_SHDW	0X0EC

/* AM335X EMIF Register values */
#define VTP_CTRL_READY		(0x1 << 5)
#define VTP_CTRL_ENABLE		(0x1 << 6)
#define VTP_CTRL_START_EN	(0x1)
#define DDR_CKE_CTRL_NORMAL	(0x1)
#define PHY_EN_DYN_PWRDN	(0x1 << 20)

static const struct cmd_control ddr3_evm_cmd_ctrl_data = {
	.cmd0csratio = MT41J512M8RH125_RATIO,
	.cmd0iclkout = MT41J512M8RH125_INVERT_CLKOUT,

	.cmd1csratio = MT41J512M8RH125_RATIO,
	.cmd1iclkout = MT41J512M8RH125_INVERT_CLKOUT,

	.cmd2csratio = MT41J512M8RH125_RATIO,
	.cmd2iclkout = MT41J512M8RH125_INVERT_CLKOUT,
};

static void config_vtp(void)
{
	void *addr = (void *)VTP0_CTRL_ADDR;
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
	void *base = (void *)DDR_PHY_CMD_ADDR;

	writel(cmd->cmd0csratio, base + PHYCMD_CM0CSRATIO);
	writel(cmd->cmd0iclkout, base + PHYCMD_CM0ICLKOUT);

	writel(cmd->cmd1csratio, base + PHYCMD_CM1CSRATIO);
	writel(cmd->cmd1iclkout, base + PHYCMD_CM1ICLKOUT);

	writel(cmd->cmd2csratio, base + PHYCMD_CM2CSRATIO);
	writel(cmd->cmd2iclkout, base + PHYCMD_CM2ICLKOUT);
}

/* Configure DDR DATA registers */
static void config_ddr_data(const struct ddr_data *data)
{
	int i;
	void *base[2] = {
		(void *)DDR_PHY_DATA_ADDR,
		(void *)DDR_PHY_DATA_ADDR2,
	};

	for (i = 0; i < 2; i++) {
		writel(data->datardsratio0, base[i] + PHYDAT_DT0RDSRATIO0);
		writel(data->datawdsratio0, base[i] + PHYDAT_DT0WDSRATIO0);
		writel(data->datawiratio0, base[i] + PHYDAT_DT0WIRATIO0);
		writel(data->datagiratio0, base[i] + PHYDAT_DT0GIRATIO0);
		writel(data->datafwsratio0, base[i] + PHYDAT_DT0FWSRATIO0);
		writel(data->datawrsratio0, base[i] + PHYDAT_DT0WRSRATIO0);
	}
}

static void config_io_ctrl(unsigned long val)
{
	void *base = (void *)DDR_CONTROL_BASE_ADDR;

	writel(val, base + CON_CM0IOCTL);
	writel(val, base + CON_CM1IOCTL);
	writel(val, base + CON_CM2IOCTL);
	writel(val, base + CON_DT0IOCTL);
	writel(val, base + CON_DT1IOCTL);
}

static void config_ddr_phy(const struct emif_regs *regs)
{
	u32 val = regs->emif_ddr_phy_ctlr_1;
	void *base = (void *)EMIF4_0_CFG_BASE;

	writel(val, base + EMIF_DDR_PHY_CTRL_1);
	writel(val, base + EMIF_DDR_PHY_CTRL_1_SHDW);
}


static void set_sdram_timings(const struct emif_regs *regs)
{
	void *base = (void *)EMIF4_0_CFG_BASE;

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
	void *base = (void *)EMIF4_0_CFG_BASE;

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
	writel(int_config, base + EMIF_L3_CONFIG);
	writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL);
	writel(regs->ref_ctrl, base + EMIF_SDRAM_REF_CTRL_SHDW);
	writel(regs->sdram_config, base + EMIF_SDRAM_CONFIG);
}

void config_ddr(unsigned int pll, unsigned int ioctrl,
		const struct ddr_data *data, const struct cmd_control *ctrl,
		const struct emif_regs *regs)
{
	config_vtp();
	config_cmd_ctrl(ctrl);

	config_ddr_data(data);
	config_io_ctrl(ioctrl);

	/* Set CKE to be controlled by EMIF/DDR PHY */
//	writel(DDR_CKE_CTRL_NORMAL, &ddrctrl->ddrckectrl);

	/* Program EMIF instance */
	config_ddr_phy(regs);
	set_sdram_timings(regs);
	config_sdram(regs);
}

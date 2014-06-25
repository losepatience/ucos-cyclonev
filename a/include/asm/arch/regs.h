/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 24 13:21:40 CST 2014
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

/* DM Timer base addresses */
#define DM_TIMER0_BASE			0x4802C000
#define DM_TIMER1_BASE			0x4802E000
#define DM_TIMER2_BASE			0x48040000
#define DM_TIMER3_BASE			0x48042000
#define DM_TIMER4_BASE			0x48044000
#define DM_TIMER5_BASE			0x48046000
#define DM_TIMER6_BASE			0x48048000
#define DM_TIMER7_BASE			0x4804A000

/* GPIO Base address */
#define GPIO0_BASE			0x48032000
#define GPIO1_BASE			0x4804C000
#define GPIO2_BASE			0x481AC000

/* BCH Error Location Module */
#define ELM_BASE			0x48080000

/* EMIF Base address */
#define EMIF4_0_CFG_BASE		0x4C000000
#define EMIF4_1_CFG_BASE		0x4D000000

/* DDR Base address */
#define DDR_CTRL_ADDR			0x44E10E04
#define DDR_CONTROL_BASE_ADDR		0x44E11404

/* UART */
#define DEFAULT_UART_BASE		UART0_BASE

/* GPMC Base address */
#define GPMC_BASE			0x50000000

/* CPSW Config space */
#define CPSW_BASE			0x4A100000

/* UART Base Address */
#define UART0_BASE			0x44E09000

/* Watchdog Timer */
#define WDT_BASE			0x44E35000

/* Control Module Base Address */
#define CTRL_BASE			0x44E10000
#define CTRL_DEVICE_BASE		0x44E10600

/* PRCM Base Address */
#define PRCM_BASE			0x44E00000
#define CM_PER				0x44E00000
#define CM_WKUP				0x44E00400
#define CM_DPLL				0x44E00500
#define CM_RTC				0x44E00800

#define PRM_RSTCTRL			(PRCM_BASE + 0x0F00)
#define PRM_RSTST			(PRM_RSTCTRL + 8)

/* VTP Base address */
#define VTP0_CTRL_ADDR			0x44E10E0C
#define VTP1_CTRL_ADDR			0x48140E10

/* DDR Base address */
#define DDR_PHY_CMD_ADDR		0x44E12000
#define DDR_PHY_DATA_ADDR		0x44E120C8
#define DDR_PHY_CMD_ADDR2		0x47C0C800
#define DDR_PHY_DATA_ADDR2		0x47C0C8C8
#define DDR_DATA_REGS_NR		2

#define DDRPHY_0_CONFIG_BASE		(CTRL_BASE + 0x1400)
#define DDRPHY_CONFIG_BASE		DDRPHY_0_CONFIG_BASE

/* CPSW Config space */
#define CPSW_MDIO_BASE			0x4A101000

/* RTC base address */
#define RTC_BASE			0x44E3E000

/* OTG */
#define USB0_OTG_BASE			0x47401000
#define USB1_OTG_BASE			0x47401800

/* LCD Controller */
#define LCD_CNTL_BASE			0x4830E000

/* PWMSS */
#define PWMSS0_BASE			0x48300000
#define AM33XX_ECAP0_BASE		0x48300100


/* I2C */
#define  I2C0_BASE			0x44E0B000
#define  I2C1_BASE			0x4802A000
#define  I2C2_BASE			0x4819C000
#define	 I2C_BUS_MAX			3

#define I2C_DEFAULT_BASE		I2C_BASE1
/* -------------------------------------------------------------
 * offsets
 * -------------------------------------------------------------
 */

/* timer registers */
#define TMREG_TCLR			0x38
#define TMREG_TCRR			0x3C
#define TMREG_TLDR			0x40

/* wdt registers */
#define WDTREG_WSPR			0x48
#define WDTREG_WWPS			0x34

/* rtc registers */
#define	RTCREG_OSC			0x54
#define	RTCREG_KICK0R			0x6c
#define	RTCREG_KICK1R			0x70

/* gpmc registers */

#define GPMC_SYSCONFIG			0x010
#define GPMC_IRQSTATUS			0x018
#define GPMC_IRQENABLE			0x01C
#define GPMC_TIMEOUT_CONTROL		0x040
#define GPMC_CONFIG			0x050
#define GPMC_STATUS			0x054
#define GPMC_CS0			0x060
#define GPMC_CS1			0x090
#define GPMC_CS2			0x0C0
#define GPMC_CS3			0x0F0
#define GPMC_CS4			0x120
#define GPMC_CS5			0x150
#define GPMC_CS6			0x180
#define GPMC_CS7			0x1B0
#define GPMC_ECC_CONFIG			0x1F4
#define GPMC_ECC_CONTROL		0x1F8
#define GPMC_ECC_SIZE_CONFIG		0x1FC
#define GPMC_ECC1_RESULT		0x200
#define GPMC_ECC2_RESULT		0x204
#define GPMC_ECC3_RESULT		0x208
#define GPMC_ECC4_RESULT		0x20C
#define GPMC_ECC5_RESULT		0x210
#define GPMC_ECC6_RESULT		0x214
#define GPMC_ECC7_RESULT		0x218
#define GPMC_ECC8_RESULT		0x21C
#define GPMC_ECC9_RESULT		0x220
#define GPMC_TESTMOMDE_CTRL		0x230
#define GPMC_BCH_RES1_0_3		0x240
#define GPMC_BCH_RES2_0_3		0x250

#define GPMC_CS_CONFIG1			0x000
#define GPMC_CS_CONFIG2			0x004
#define GPMC_CS_CONFIG3			0x008
#define GPMC_CS_CONFIG4			0x00C
#define GPMC_CS_CONFIG5			0x010
#define GPMC_CS_CONFIG6			0x014
#define GPMC_CS_CONFIG7			0x018
#define GPMC_CS_NAND_CMD		0x01C
#define GPMC_CS_NAND_ADR		0x020
#define GPMC_CS_NAND_DAT		0x024

/* ----------------------------------
 * clock manager registers
 * ----------------------------------
 */
/* Encapsulating core pll registers */
#define CMWKUP_WKCLKSTCTRL		((void *)(CM_WKUP + 0x00))
#define CMWKUP_WKCTRLCLKCTRL		((void *)(CM_WKUP + 0x04))
#define CMWKUP_WKGPIO0CLKCTRL		((void *)(CM_WKUP + 0x08))
#define CMWKUP_WKL4WKCLKCTRL		((void *)(CM_WKUP + 0x0c))
#define CMWKUP_IDLESTDPLLMPU		((void *)(CM_WKUP + 0x20))
#define CMWKUP_CLKSELDPLLMPU		((void *)(CM_WKUP + 0x2c))
#define CMWKUP_IDLESTDPLLDDR		((void *)(CM_WKUP + 0x34))
#define CMWKUP_CLKSELDPLLDDR		((void *)(CM_WKUP + 0x40))
#define CMWKUP_CLKSELDPLLDISP		((void *)(CM_WKUP + 0x54))
#define CMWKUP_IDLESTDPLLCORE		((void *)(CM_WKUP + 0x5c))
#define CMWKUP_CLKSELDPLLCORE		((void *)(CM_WKUP + 0x68))
#define CMWKUP_IDLESTDPLLPER		((void *)(CM_WKUP + 0x70))
#define CMWKUP_CLKDCOLDODPLLPER		((void *)(CM_WKUP + 0x7c))
#define CMWKUP_DIVM4DPLLCORE		((void *)(CM_WKUP + 0x80))
#define CMWKUP_DIVM5DPLLCORE		((void *)(CM_WKUP + 0x84))
#define CMWKUP_CLKMODDPLLMPU		((void *)(CM_WKUP + 0x88))
#define CMWKUP_CLKMODDPLLPER		((void *)(CM_WKUP + 0x8c))
#define CMWKUP_CLKMODDPLLCORE		((void *)(CM_WKUP + 0x90))
#define CMWKUP_CLKMODDPLLDDR		((void *)(CM_WKUP + 0x94))
#define CMWKUP_CLKMODDPLLDISP		((void *)(CM_WKUP + 0x98))
#define CMWKUP_CLKSELDPLLPER		((void *)(CM_WKUP + 0x9c))
#define CMWKUP_DIVM2DPLLDDR		((void *)(CM_WKUP + 0xA0))
#define CMWKUP_DIVM2DPLLDISP		((void *)(CM_WKUP + 0xA4))
#define CMWKUP_DIVM2DPLLMPU		((void *)(CM_WKUP + 0xA8))
#define CMWKUP_DIVM2DPLLPER		((void *)(CM_WKUP + 0xAC))
#define CMWKUP_WKUP_UART0CTRL		((void *)(CM_WKUP + 0xB4))
#define CMWKUP_WKUP_I2C0CTRL		((void *)(CM_WKUP + 0xB8))
#define CMWKUP_DIVM6DPLLCORE		((void *)(CM_WKUP + 0xD8))

/* peripheral functional clocks pll registers */
#define CMPER_L4LSCLKSTCTRL		((void *)(CM_PER + 0x00))
#define CMPER_L3SCLKSTCTRL		((void *)(CM_PER + 0x04))
#define CMPER_L4FWCLKSTCTRL		((void *)(CM_PER + 0x08))
#define CMPER_L3CLKSTCTRL		((void *)(CM_PER + 0x0c))
#define CMPER_CPGMAC0CLKCTRL		((void *)(CM_PER + 0x14))
#define CMPER_LCDCLKCTRL		((void *)(CM_PER + 0x18))
#define CMPER_USB0CLKCTRL		((void *)(CM_PER + 0x1C))
#define CMPER_TPTC0CLKCTRL		((void *)(CM_PER + 0x24))
#define CMPER_EMIFCLKCTRL		((void *)(CM_PER + 0x28))
#define CMPER_OCMCRAMCLKCTRL		((void *)(CM_PER + 0x2c))
#define CMPER_GPMCCLKCTRL		((void *)(CM_PER + 0x30))
#define CMPER_MCASP0CLKCTRL		((void *)(CM_PER + 0x34))
#define CMPER_UART5CLKCTRL		((void *)(CM_PER + 0x38))
#define CMPER_MMC0CLKCTRL		((void *)(CM_PER + 0x3C))
#define CMPER_ELMCLKCTRL		((void *)(CM_PER + 0x40))
#define CMPER_I2C2CLKCTRL		((void *)(CM_PER + 0x44))
#define CMPER_I2C1CLKCTRL		((void *)(CM_PER + 0x48))
#define CMPER_SPI0CLKCTRL		((void *)(CM_PER + 0x4C))
#define CMPER_SPI1CLKCTRL		((void *)(CM_PER + 0x50))
#define CMPER_L4LSCLKCTRL		((void *)(CM_PER + 0x60))
#define CMPER_L4FWCLKCTRL		((void *)(CM_PER + 0x64))
#define CMPER_MCASP1CLKCTRL		((void *)(CM_PER + 0x68))
#define CMPER_UART1CLKCTRL		((void *)(CM_PER + 0x6C))
#define CMPER_UART2CLKCTRL		((void *)(CM_PER + 0x70))
#define CMPER_UART3CLKCTRL		((void *)(CM_PER + 0x74))
#define CMPER_UART4CLKCTRL		((void *)(CM_PER + 0x78))
#define CMPER_TIMER7CLKCTRL		((void *)(CM_PER + 0x7C))
#define CMPER_TIMER2CLKCTRL		((void *)(CM_PER + 0x80))
#define CMPER_TIMER3CLKCTRL		((void *)(CM_PER + 0x84))
#define CMPER_TIMER4CLKCTRL		((void *)(CM_PER + 0x88))
#define CMPER_GPIO1CLKCTRL		((void *)(CM_PER + 0xAC))
#define CMPER_GPIO2CLKCTRL		((void *)(CM_PER + 0xB0))
#define CMPER_GPIO3CLKCTRL		((void *)(CM_PER + 0xB4))
#define CMPER_TPCCCLKCTRL		((void *)(CM_PER + 0xBC))
#define CMPER_DCAN0CLKCTRL		((void *)(CM_PER + 0xC0))
#define CMPER_DCAN1CLKCTRL		((void *)(CM_PER + 0xC4))
#define CMPER_EMIFFWCLKCTRL		((void *)(CM_PER + 0xD0))
#define CMPER_EPWMSS0CLKCTRL		((void *)(CM_PER + 0xD4))
#define CMPER_EPWMSS2CLKCTRL		((void *)(CM_PER + 0xD8))
#define CMPER_L3INSTRCLKCTRL		((void *)(CM_PER + 0xDC))
#define CMPER_L3CLKCTRL			((void *)(CM_PER + 0xE0))
#define CMPER_MMC1CLKCTRL		((void *)(CM_PER + 0xF4))
#define CMPER_MMC2CLKCTRL		((void *)(CM_PER + 0xF8))
#define CMPER_L4HSCLKSTCTRL		((void *)(CM_PER + 0x11C))
#define CMPER_L4HSCLKCTRL		((void *)(CM_PER + 0x120))
#define CMPER_CPSWCLKSTCTRL		((void *)(CM_PER + 0x144))
#define CMPER_LCDCCLKSTCTRL		((void *)(CM_PER + 0x148))

/* Display pll registers */
#define CMDPLL_CLKTIMER2CLK		((void *)(CM_DPLL + 0x08))
#define CMDPLL_CLKLCDCPIXELCLK		((void *)(CM_DPLL + 0x34))

/* RTC pll registers */
#define CMRTC_RTCCLKCTRL		((void *)(CM_RTC + 0x0))
#define CMRTC_CLKSTCTRL			((void *)(CM_RTC + 0x4))


/* ----------------------------------
 *  pin mux registers
 * ----------------------------------
 */
#define PAD_CR	0x800

struct pad_signals;

#define __pin(x)	\
	(int)(&((struct pad_signals *)(PAD_CR))->x)

struct pad_signals {
	int gpmc_ad0;
	int gpmc_ad1;
	int gpmc_ad2;
	int gpmc_ad3;
	int gpmc_ad4;
	int gpmc_ad5;
	int gpmc_ad6;
	int gpmc_ad7;
	int gpmc_ad8;
	int gpmc_ad9;
	int gpmc_ad10;
	int gpmc_ad11;
	int gpmc_ad12;
	int gpmc_ad13;
	int gpmc_ad14;
	int gpmc_ad15;
	int gpmc_a0;
	int gpmc_a1;
	int gpmc_a2;
	int gpmc_a3;
	int gpmc_a4;
	int gpmc_a5;
	int gpmc_a6;
	int gpmc_a7;
	int gpmc_a8;
	int gpmc_a9;
	int gpmc_a10;
	int gpmc_a11;
	int gpmc_wait0;
	int gpmc_wpn;
	int gpmc_be1n;
	int gpmc_csn0;
	int gpmc_csn1;
	int gpmc_csn2;
	int gpmc_csn3;
	int gpmc_clk;
	int gpmc_advn_ale;
	int gpmc_oen_ren;
	int gpmc_wen;
	int gpmc_be0n_cle;
	int lcd_data0;
	int lcd_data1;
	int lcd_data2;
	int lcd_data3;
	int lcd_data4;
	int lcd_data5;
	int lcd_data6;
	int lcd_data7;
	int lcd_data8;
	int lcd_data9;
	int lcd_data10;
	int lcd_data11;
	int lcd_data12;
	int lcd_data13;
	int lcd_data14;
	int lcd_data15;
	int lcd_vsync;
	int lcd_hsync;
	int lcd_pclk;
	int lcd_ac_bias_en;
	int mmc0_dat3;
	int mmc0_dat2;
	int mmc0_dat1;
	int mmc0_dat0;
	int mmc0_clk;
	int mmc0_cmd;
	int mii1_col;
	int mii1_crs;
	int mii1_rxerr;
	int mii1_txen;
	int mii1_rxdv;
	int mii1_txd3;
	int mii1_txd2;
	int mii1_txd1;
	int mii1_txd0;
	int mii1_txclk;
	int mii1_rxclk;
	int mii1_rxd3;
	int mii1_rxd2;
	int mii1_rxd1;
	int mii1_rxd0;
	int rmii1_refclk;
	int mdio_data;
	int mdio_clk;
	int spi0_sclk;
	int spi0_d0;
	int spi0_d1;
	int spi0_cs0;
	int spi0_cs1;
	int ecap0_in_pwm0_out;
	int uart0_ctsn;
	int uart0_rtsn;
	int uart0_rxd;
	int uart0_txd;
	int uart1_ctsn;
	int uart1_rtsn;
	int uart1_rxd;
	int uart1_txd;
	int i2c0_sda;
	int i2c0_scl;
	int mcasp0_aclkx;
	int mcasp0_fsx;
	int mcasp0_axr0;
	int mcasp0_ahclkr;
	int mcasp0_aclkr;
	int mcasp0_fsr;
	int mcasp0_axr1;
	int mcasp0_ahclkx;
	int xdma_event_intr0;
	int xdma_event_intr1;
	int nresetin_out;
	int porz;
	int nnmi;
	int osc0_in;
	int osc0_out;
	int rsvd1;
	int tms;
	int tdi;
	int tdo;
	int tck;
	int ntrst;
	int emu0;
	int emu1;
	int osc1_in;
	int osc1_out;
	int pmic_power_en;
	int rtc_porz;
	int rsvd2;
	int ext_wakeup;
	int enz_kaldo_1p8v;
	int usb0_dm;
	int usb0_dp;
	int usb0_ce;
	int usb0_id;
	int usb0_vbus;
	int usb0_drvvbus;
	int usb1_dm;
	int usb1_dp;
	int usb1_ce;
	int usb1_id;
	int usb1_vbus;
	int usb1_drvvbus;
	int ddr_resetn;
	int ddr_csn0;
	int ddr_cke;
	int ddr_ck;
	int ddr_nck;
	int ddr_casn;
	int ddr_rasn;
	int ddr_wen;
	int ddr_ba0;
	int ddr_ba1;
	int ddr_ba2;
	int ddr_a0;
	int ddr_a1;
	int ddr_a2;
	int ddr_a3;
	int ddr_a4;
	int ddr_a5;
	int ddr_a6;
	int ddr_a7;
	int ddr_a8;
	int ddr_a9;
	int ddr_a10;
	int ddr_a11;
	int ddr_a12;
	int ddr_a13;
	int ddr_a14;
	int ddr_a15;
	int ddr_odt;
	int ddr_d0;
	int ddr_d1;
	int ddr_d2;
	int ddr_d3;
	int ddr_d4;
	int ddr_d5;
	int ddr_d6;
	int ddr_d7;
	int ddr_d8;
	int ddr_d9;
	int ddr_d10;
	int ddr_d11;
	int ddr_d12;
	int ddr_d13;
	int ddr_d14;
	int ddr_d15;
	int ddr_dqm0;
	int ddr_dqm1;
	int ddr_dqs0;
	int ddr_dqsn0;
	int ddr_dqs1;
	int ddr_dqsn1;
	int ddr_vref;
	int ddr_vtp;
	int ddr_strben0;
	int ddr_strben1;
	int ain7;
	int ain6;
	int ain5;
	int ain4;
	int ain3;
	int ain2;
	int ain1;
	int ain0;
	int vrefp;
	int vrefn;
};


/* ----------------------------------
 * control device registers
 * ----------------------------------
 */
#define CDEV_DEVICEID			0x000
#define CDEV_USB_CTRL0			0x020
#define CDEV_USB_CTRL1			0x028
#define CDEV_MACID0L			0x030
#define CDEV_MACID0H			0x034
#define CDEV_MACID1L			0x038
#define CDEV_MACID1H			0x03c
#define CDEV_MIISEL			0x050
#define CDEV_EFUSE_SMA			0x1FC

/* ----------------------------------
 * ddr registers
 * ----------------------------------
 */
#define DDRCMD_CM0CSRATIO		0x01C
#define DDRCMD_CM0ICLKOUT		0x02C
#define DDRCMD_CM1CSRATIO		0x050
#define DDRCMD_CM1ICLKOUT		0x060
#define DDRCMD_CM2CSRATIO		0x084
#define DDRCMD_CM2ICLKOUT		0x094

#define DDRDAT_DT0RDSRATIO0		0x0C8
#define DDRDAT_DT0WDSRATIO0		0x0DC
#define DDRDAT_DT0WIRATIO0		0x0F0
#define DDRDAT_DT0WIMODE0		0x0F8
#define DDRDAT_DT0GIRATIO0		0x0FC
#define DDRDAT_DT0GIMODE0		0x104
#define DDRDAT_DT0FWSRATIO0		0x108
#define DDRDAT_DT0DQOFFSET		0x11C
#define DDRDAT_DT0WRSRATIO0		0x120
#define DDRDAT_DT0RDELAYS0		0x134
#define DDRDAT_DT0DLDIFF0		0x138

#define DDRCON_CM0IOCTL			0x000
#define DDRCON_CM1IOCTL			0x004
#define DDRCON_CM2IOCTL			0x008
#define DDRCON_DT0IOCTL			0x03C
#define DDRCON_DT1IOCTL			0x040
#define DDRCON_DT2IOCTRL		0x044
#define DDRCON_DT3IOCTRL		0x048

#define DDR_CTRL_DDRIOCTRL		0x000
#define DDR_CTRL_DDRCKECTRL		0x514

/* ----------------------------------
 * emif registers
 * ----------------------------------
 */
#define EMIF_SDRAM_CONFIG		0x008
#define EMIF_SDRAM_REF_CTRL		0x010
#define EMIF_SDRAM_REF_CTRL_SHDW	0x014
#define EMIF_SDRAM_TIM_1		0x018
#define EMIF_SDRAM_TIM_1_SHDW		0x01C
#define EMIF_SDRAM_TIM_2		0x020
#define EMIF_SDRAM_TIM_2_SHDW		0x024
#define EMIF_SDRAM_TIM_3		0x028
#define EMIF_SDRAM_TIM_3_SHDW		0x02C
#define EMIF_L3_CONFIG			0x054
#define EMIF_ZQ_CONFIG			0x0CC
#define EMIF_DDR_PHY_CTRL_1		0x0E8
#define EMIF_DDR_PHY_CTRL_1_SHDW	0x0EC

/* ctrl registers */
#define SECURE_EMIF_SDRAM_CONFIG	0x0110


/* ----------------------------------
 * i2c registers
 * ----------------------------------
 */
#define I2CREG_REVNB_LO			0x00
#define I2CREG_REVNB_HI			0x04
#define I2CREG_SYSC			0x10
#define I2CREG_IRQSTATUS_RAW		0x24
#define I2CREG_STAT			0x28
#define I2CREG_IE			0x2C
#define I2CREG_IRQENABLE_CLR		0x30
#define I2CREG_IV			0x34
#define I2CREG_SYSS			0x90
#define I2CREG_BUF			0x94
#define I2CREG_CNT			0x98
#define I2CREG_DATA			0x9C
#define I2CREG_RES14			0xA0
#define I2CREG_CON			0xA4
#define I2CREG_OA			0xA8
#define I2CREG_SA			0xAC
#define I2CREG_PSC			0xB0
#define I2CREG_SCLL			0xB4
#define I2CREG_SCLH			0xB8
#define I2CREG_SYSTEST			0xBC
#define I2CREG_BUFSTAT			0xC0

#endif /* __REGS_H__ */

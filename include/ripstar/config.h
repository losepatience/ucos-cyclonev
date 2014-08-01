#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <asm/regs.h>

/* macro for both os */
#define __OS_BASE		(0x00200000)
#define __OS_SIZE		(0x01400000)	/* size(os) + size(pt) */
#define PHYS_SDRAM_SIZE		(0x20000000)

/*
 * os0:	0x200000--0x700000(0x4F8000(os) + 0x4000(pt) + 0x4000(stack))
 * os1:	0x700000--0xC00000(0x4F8000(os) + 0x4000(pt) + 0x4000(stack))
 */
#ifndef OS1
# define __text_base		(__OS_BASE + 0x8000)
#else
# define __text_base		(__OS_BASE + 0x8000 + __OS_SIZE / 2)
#endif

#define __mmu_table		(__text_base - 0x4000)
#define __svc_stack		(__mmu_table - 1)
#define __irq_stack		(__mmu_table - 0x2000)

#define RESERVED_MEMSIZE	(__OS_BASE)

/* -------------------------------------------------------------- */
/* flash layout */
/* -------------------------------------------------------------- */
/*
 * .------------------------.------------.
 * | 0x300000-0x700000(4M)  | FPGA       |
 * .------------------------.------------.
 * | 0x200000-0x300000(1M)  | OS1        |
 * .------------------------.------------.
 * | 0x100000-0x200000(1M)  | OS0        |
 * .------------------------.------------.
 * | 0-0x40000(64K)         | preloader  |
 * ^------------------------^------------^
 */
#define FLASH_OS0_OFFSET	0x100000
#define FLASH_OS1_OFFSET	0x200000
#define FLASH_FPGA_OFFSET	0x300000

/* -------------------------------------------------------------- */
/* XXX: Plz verify it according to pll_config.h */
/* -------------------------------------------------------------- */
#define CONFIG_HPS_CLK_OSC1_HZ			(25000000)
#define CONFIG_HPS_CLK_MAINVCO_HZ		(1600000000)
#define CONFIG_HPS_CLK_PERVCO_HZ		(1000000000)
#define CONFIG_HPS_CLK_SDRVCO_HZ		(600000000)
#define CONFIG_HPS_CLK_EMAC0_HZ			(50000000)
#define CONFIG_HPS_CLK_EMAC1_HZ			(50000000)
#define CONFIG_HPS_CLK_USBCLK_HZ		(200000000)
#define CONFIG_HPS_CLK_NAND_HZ			(100000000)
#define CONFIG_HPS_CLK_SDMMC_HZ			(50000000)
#define CONFIG_HPS_CLK_QSPI_HZ			(400000000)
#define CONFIG_HPS_CLK_SPIM_HZ			(200000000)
#define CONFIG_HPS_CLK_CAN0_HZ			(100000000)
#define CONFIG_HPS_CLK_CAN1_HZ			(100000000)
#define CONFIG_HPS_CLK_GPIODB_HZ		(32000)
#define CONFIG_HPS_CLK_L4_MP_HZ			(100000000)
#define CONFIG_HPS_CLK_L4_SP_HZ			(100000000)

/* -------------------------------------------------------------- */
/* QSPI support */
/* -------------------------------------------------------------- */
#define CONFIG_CQSPI_BASE		(SOCFPGA_QSPIREGS_ADDRESS)
#define CONFIG_CQSPI_AHB_BASE		(SOCFPGA_QSPIDATA_ADDRESS)
#define CONFIG_SPI_FLASH_STMICRO	/* Micron/Numonyx flash */

/* Flash device info */
#define CONFIG_SF_DEFAULT_SPEED		(50000000)
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3

#define CONFIG_SPI_FLASH_QUAD		(1)

/* QSPI reference clock */
#define CONFIG_CQSPI_REF_CLK		CONFIG_HPS_CLK_QSPI_HZ

/* QSPI page size and block size */
#define CONFIG_CQSPI_PAGE_SIZE		(256)
#define CONFIG_CQSPI_BLOCK_SIZE		(16)

/* QSPI Delay timing */
#define CONFIG_CQSPI_TSHSL_NS		(200)
#define CONFIG_CQSPI_TSD2D_NS		(255)
#define CONFIG_CQSPI_TCHSH_NS		(20)
#define CONFIG_CQSPI_TSLCH_NS		(20)
#define CONFIG_CQSPI_DECODER		(0)
#define CONFIG_CQSPI_4BYTE_ADDR		(1)

/* -------------------------------------------------------------- */
/* L4 OSC1 Timer1 */
/* -------------------------------------------------------------- */
/* reload value when timer count to zero */
#define TIMER_LOAD_VAL			0xFFFFFFFF

/* Timer info */
#define CONFIG_SYS_HZ			1000

/* Clocks source frequency to timer */
#define CONFIG_TIMER_CLOCK_KHZ		(CONFIG_HPS_CLK_OSC1_HZ / 1000)

/* -------------------------------------------------------------- */
/* emac */
/* -------------------------------------------------------------- */
#define CONFIG_EMAC0_BASE		SOCFPGA_EMAC0_ADDRESS
#define CONFIG_EMAC1_BASE		SOCFPGA_EMAC1_ADDRESS

/* designware */
#define CONFIG_NET_MULTI
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_DW_SEARCH_PHY
#define CONFIG_MII
#define CONFIG_PHY_GIGE
#define CONFIG_DW_AUTONEG
#define CONFIG_AUTONEG_TIMEOUT		(15 * CONFIG_SYS_HZ)
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_PHY_MICREL_KSZ9021

/* phy */
#define CONFIG_EPHY0_PHY_ADDR		0
#define CONFIG_EPHY1_PHY_ADDR		4
#define CONFIG_KSZ9021_CLK_SKEW_VAL	0xf0f0
#define CONFIG_KSZ9021_DATA_SKEW_VAL	0x0

/* Type of PHY available */
#define SOCFPGA_PHYSEL_ENUM_GMII	0x0
#define SOCFPGA_PHYSEL_ENUM_MII		0x1
#define SOCFPGA_PHYSEL_ENUM_RGMII	0x2
#define SOCFPGA_PHYSEL_ENUM_RMII	0x3

/* -------------------------------------------------------------- */
/* eeprom */
/* -------------------------------------------------------------- */
#ifndef E2PROM_PAGESIZE
# define E2PROM_PAGESIZE	(0x40)
#endif
#ifndef IIC_EEPROM_ADDR
# define IIC_EEPROM_ADDR	(0x50)
#endif

#define AT24_FLAG_ADDR16	(0x1)

#endif

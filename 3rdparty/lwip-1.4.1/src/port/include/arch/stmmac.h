#ifndef _STMMAC_H_
#define _STMMAC_H_

#include <lwip/netif.h>
#include <netif/etharp.h>

#include <asm/io.h>
#include <asm/regs.h>

/* Receive Interrupt */
#define DMA_INTR_ENA_RIE	0x00000040
#define DMA_INTR_ENA_NIE	0x00010000
#define DMA_STATUS_RI		0x00000040

/* micrel phy related definitions */
#define KSZ9021_EXTREG_CTRL		11
#define KSZ9021_EXTREG_DATA_WRITE	12

#define MII_KSZ9021_EXT_COMMON_CTRL		0x100
#define MII_KSZ9021_EXT_STRAP_STATUS		0x101
#define MII_KSZ9021_EXT_OP_STRAP_OVERRIDE	0x102
#define MII_KSZ9021_EXT_OP_STRAP_STATUS		0x103
#define MII_KSZ9021_EXT_RGMII_CLOCK_SKEW	0x104
#define MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW	0x105
#define MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW	0x106
#define MII_KSZ9021_EXT_ANALOG_TEST		0x107

/* MAC configuration register definitions */
#define FRAMEBURSTENABLE	(1 << 21)
#define MII_PORTSELECT		(1 << 15)
#define FES_100			(1 << 14)
#define DISABLERXOWN		(1 << 13)
#define FULLDPLXMODE		(1 << 11)
#define RXENABLE		(1 << 2)
#define TXENABLE		(1 << 3)

/* MII address register definitions */
#define MII_BUSY		(1 << 0)
#define MII_WRITE		(1 << 1)
#define MII_CLKRANGE_60_100M	(0)
#define MII_CLKRANGE_100_150M	(0x4)
#define MII_CLKRANGE_20_35M	(0x8)
#define MII_CLKRANGE_35_60M	(0xC)
#define MII_CLKRANGE_150_250M	(0x10)
#define MII_CLKRANGE_250_300M	(0x14)

#define MIIADDRSHIFT		(11)
#define MIIREGSHIFT		(6)
#define MII_REGMSK		(0x1F << 6)
#define MII_ADDRMSK		(0x1F << 11)

/* Bus mode register definitions */
#define FIXEDBURST		(1 << 16)
#define PRIORXTX_41		(3 << 14)
#define PRIORXTX_31		(2 << 14)
#define PRIORXTX_21		(1 << 14)
#define PRIORXTX_11		(0 << 14)
#define BURST_1			(1 << 8)
#define BURST_2			(2 << 8)
#define BURST_4			(4 << 8)
#define BURST_8			(8 << 8)
#define BURST_16		(16 << 8)
#define BURST_32		(32 << 8)
#define RXHIGHPRIO		(1 << 1)
#define DMAMAC_SRST		(1 << 0)

/* Poll demand definitions */
#define POLL_DATA		(0xFFFFFFFF)

/* Operation mode definitions */
#define STOREFORWARD		(1 << 21)
#define FLUSHTXFIFO		(1 << 20)
#define TXSTART			(1 << 13)
#define TXSECONDFRAME		(1 << 2)
#define RXSTART			(1 << 1)

/* Descriptior related definitions */
#define MAC_MAX_FRAME_SZ	(1600)

/* tx status bits definitions */
#define DESC_TXSTS_OWNBYDMA		(1 << 31)
#define DESC_TXSTS_TXINT		(1 << 30)
#define DESC_TXSTS_TXLAST		(1 << 29)
#define DESC_TXSTS_TXFIRST		(1 << 28)
#define DESC_TXSTS_TXCRCDIS		(1 << 27)

#define DESC_TXSTS_TXPADDIS		(1 << 26)
#define DESC_TXSTS_TXCHECKINSCTRL	(3 << 22)
#define DESC_TXSTS_TXRINGEND		(1 << 21)
#define DESC_TXSTS_TXCHAIN		(1 << 20)
#define DESC_TXSTS_MSK			(0x1FFFF << 0)

/* rx status bits definitions */
#define DESC_RXSTS_OWNBYDMA		(1 << 31)
#define DESC_RXSTS_DAFILTERFAIL		(1 << 30)
#define DESC_RXSTS_FRMLENMSK		(0x3FFF << 16)
#define DESC_RXSTS_FRMLENSHFT		(16)

#define DESC_RXSTS_ERROR		(1 << 15)
#define DESC_RXSTS_RXTRUNCATED		(1 << 14)
#define DESC_RXSTS_SAFILTERFAIL		(1 << 13)
#define DESC_RXSTS_RXIPC_GIANTFRAME	(1 << 12)
#define DESC_RXSTS_RXDAMAGED		(1 << 11)
#define DESC_RXSTS_RXVLANTAG		(1 << 10)
#define DESC_RXSTS_RXFIRST		(1 << 9)
#define DESC_RXSTS_RXLAST		(1 << 8)
#define DESC_RXSTS_RXIPC_GIANT		(1 << 7)
#define DESC_RXSTS_RXCOLLISION		(1 << 6)
#define DESC_RXSTS_RXFRAMEETHER		(1 << 5)
#define DESC_RXSTS_RXWATCHDOG		(1 << 4)
#define DESC_RXSTS_RXMIIERROR		(1 << 3)
#define DESC_RXSTS_RXDRIBBLING		(1 << 2)
#define DESC_RXSTS_RXCRC		(1 << 1)

/* tx control bits definitions */
#define DESC_TXCTRL_SIZE1MASK		(0x1FFF << 0)
#define DESC_TXCTRL_SIZE1SHFT		(0)
#define DESC_TXCTRL_SIZE2MASK		(0x1FFF << 16)
#define DESC_TXCTRL_SIZE2SHFT		(16)

/* rx control bits definitions */
#define DESC_RXCTRL_RXINTDIS		(1 << 31)
#define DESC_RXCTRL_RXRINGEND		(1 << 15)
#define DESC_RXCTRL_RXCHAIN		(1 << 14)

#define DESC_RXCTRL_SIZE1MASK		(0x1FFF << 0)
#define DESC_RXCTRL_SIZE1SHFT		(0)
#define DESC_RXCTRL_SIZE2MASK		(0x1FFF << 16)
#define DESC_RXCTRL_SIZE2SHFT		(16)

/*
 * Sizing the descriptor rings will depend upon how many pbufs you
 * have available and how big they are. Also on how many frames you
 * might want to input before dropping frames. Generally it is a
 * good idea to buffer one tcp window. This means that you won't get
 * a tcp retransmit and your tcp transmissions will be reasonably
 * fast.
 */
#define STMMAC_RXDESC_NUM	64
#define STMMAC_TXDESC_NUM	32
#define STMMAC_BUF_SZ		2048 /* XXX mod 16 == 0 */
#define STMMAC_DMA_BASE		(1024 * 1024)
#define TX_TOTAL_BUFSIZE	(STMMAC_BUF_SZ * STMMAC_TXDESC_NUM)
#define RX_TOTAL_BUFSIZE	(STMMAC_BUF_SZ * STMMAC_RXDESC_NUM)

struct stmmac_mac_regs {
	u32 config;		/* 0x00 */
	u32 framefilt;		/* 0x04 */
	u32 hashtablehigh;	/* 0x08 */
	u32 hashtablelow;	/* 0x0c */
	u32 miiaddr;		/* 0x10 */
	u32 miidata;		/* 0x14 */
	u32 flowcontrol;	/* 0x18 */
	u32 vlantag;		/* 0x1c */
	u32 version;		/* 0x20 */
	u8 reserved_1[20];
	u32 intreg;		/* 0x38 */
	u32 intmask;		/* 0x3c */
	u32 macaddr0hi;		/* 0x40 */
	u32 macaddr0lo;		/* 0x44 */
};

struct stmmac_dma_regs {
	u32 busmode;		/* 0x00 */
	u32 tx_poll_demand;	/* 0x04 */
	u32 rx_poll_demand;	/* 0x08 */
	u32 rx_desc_base;	/* 0x0c */
	u32 tx_desc_base;	/* 0x10 */
	u32 status;		/* 0x14 */
	u32 opmode;		/* 0x18 */
	u32 intenable;		/* 0x1c */
	u8 reserved[40];
	u32 currhosttxdesc;	/* 0x48 */
	u32 currhostrxdesc;	/* 0x4c */
	u32 currhosttxbuffaddr;	/* 0x50 */
	u32 currhostrxbuffaddr;	/* 0x54 */
};

struct stmmac_dma_desc {
	u32 status;
	u32 cntrl;
	void *buff;
	struct stmmac_dma_desc *next;
};

struct ethernetif {
	struct stmmac_dma_desc tx_desc[STMMAC_TXDESC_NUM];
	struct stmmac_dma_desc rx_desc[STMMAC_RXDESC_NUM];
	u8 tx_buf[TX_TOTAL_BUFSIZE];
	u8 rx_buf[RX_TOTAL_BUFSIZE];

	struct stmmac_mac_regs *mac_regs;
	struct stmmac_dma_regs *dma_regs;

	u32 tx_currdescnum;
	u32 rx_currdescnum;

	int irq;
	u32 phy_addr;
	u32 cur_rxfrm_len;
	struct eth_addr *ethaddr;

	void (*recv)(struct netif *netif);
	int (*send)(struct ethernetif *eif, void *packet, int len);

} __attribute__ ((aligned(8)));

int stmmac_eth_init(struct netif *netif);

#endif

#include <platform.h>

#include <stdio.h>
#include <arch/stmmac.h>
#include <arch/mii.h>
#include <string.h>
#include <lwip/mem.h>

static void stmmac_interrupt(void *data)
{
	struct netif *netif = (struct netif *)data;
	struct ethernetif *eif = netif->state;
	u32 desc_num = eif->rx_currdescnum;
	struct stmmac_dma_desc *desc = &eif->rx_desc[desc_num];

	//serial_puts(".");
	/* Only process RX interrupt */
	u32 status = readl(&eif->dma_regs->status);
	if (!(status & DMA_STATUS_RI))
		return;

	while (!(desc->status & DESC_RXSTS_OWNBYDMA)) {

		/* Get the length of current received frame */
		eif->cur_rxfrm_len = (desc->status & DESC_RXSTS_FRMLENMSK) \
				     >> DESC_RXSTS_FRMLENSHFT;

		eif->recv(netif);

		/* Make the current descriptor valid and go to the next one */
		desc->status |= DESC_RXSTS_OWNBYDMA;

		/* Test the wrap-around condition. */
		if (++desc_num >= STMMAC_RXDESC_NUM)
			desc_num = 0;

		eif->rx_currdescnum = desc_num;
		desc = desc->next;
	}

	/* Clear the interrupt by writing a logic 1 to the CSR5[15-0] */
	writel((status & 0x1ffff), &eif->dma_regs->status);
}

static void tx_descs_init(struct ethernetif *eif)
{
	struct stmmac_dma_desc *desc_table = &eif->tx_desc[0];
	struct stmmac_dma_regs *dma = eif->dma_regs;
	u8 *tx_buf = &eif->tx_buf[0];
	struct stmmac_dma_desc *desc;
	int i;

	for (i = 0; i < STMMAC_TXDESC_NUM; i++) {
		desc = &desc_table[i];
		desc->buff = &tx_buf[i * STMMAC_BUF_SZ];
		desc->next = &desc_table[i + 1];

		/* Clean bits: IC, LS, FS, DC, DP CIC, TER */
		desc->status &= ~(DESC_TXSTS_TXINT | DESC_TXSTS_TXLAST |
				DESC_TXSTS_TXFIRST | DESC_TXSTS_TXCRCDIS | \
				DESC_TXSTS_TXCHECKINSCTRL | \
				DESC_TXSTS_TXRINGEND | DESC_TXSTS_TXPADDIS);

		desc->status |= DESC_TXSTS_TXCHAIN;
		desc->cntrl = 0;
		desc->status &= ~(DESC_TXSTS_MSK | DESC_TXSTS_OWNBYDMA);
	}

	/* Correcting the last pointer of the chain */
	desc->next = &desc_table[0];

	writel((ulong)&desc_table[0], &dma->tx_desc_base);
}

static void rx_descs_init(struct ethernetif *eif)
{
	struct stmmac_dma_desc *desc_table = &eif->rx_desc[0];
	struct stmmac_dma_regs *dma = eif->dma_regs;
	struct stmmac_dma_desc *desc;
	int i;

	for (i = 0; i < STMMAC_RXDESC_NUM; i++) {
		desc = &desc_table[i];
		desc->buff = &eif->rx_buf[i * STMMAC_BUF_SZ];
		desc->next = &desc_table[i + 1];

		desc->cntrl = (MAC_MAX_FRAME_SZ & DESC_RXCTRL_SIZE1MASK) | \
			     DESC_RXCTRL_RXCHAIN;

		desc->status = DESC_RXSTS_OWNBYDMA;
	}

	/* Correcting the last pointer of the chain */
	desc->next = &desc_table[0];

	writel((ulong)&desc_table[0], &dma->rx_desc_base);
}

static void stmmac_descs_init(struct ethernetif *eif)
{
	tx_descs_init(eif);
	rx_descs_init(eif);
}

static int stmmac_reset(struct ethernetif *eif)
{
	struct stmmac_mac_regs *mac = eif->mac_regs;
	struct stmmac_dma_regs *dma = eif->dma_regs;

	/* DMA SW reset */
	writel(DMAMAC_SRST, &dma->busmode);
	writel(MII_PORTSELECT, &mac->config);

	while (readl(&dma->busmode) & DMAMAC_SRST)
		;

	return 0;
}

static int stmmac_set_hwaddr(struct ethernetif *eif)
{
	struct stmmac_mac_regs *mac = eif->mac_regs;
	u8 *mac_id = (u8 *)&eif->ethaddr[0];
	u32 macid_lo, macid_hi;

	macid_lo = mac_id[0] + (mac_id[1] << 8) +
		   (mac_id[2] << 16) + (mac_id[3] << 24);
	macid_hi = mac_id[4] + (mac_id[5] << 8);

	writel(macid_hi, &mac->macaddr0hi);
	writel(macid_lo, &mac->macaddr0lo);

	return 0;
}

static int stmmac_tx(struct ethernetif *eif, void *packet, int len)
{
	u32 desc_num = eif->tx_currdescnum;
	struct stmmac_dma_desc *desc = &eif->tx_desc[desc_num];
	struct stmmac_dma_regs *dma = eif->dma_regs;

	/* Check if the descriptor is owned by CPU */
	if (desc->status & DESC_TXSTS_OWNBYDMA)
		return -1;

	memcpy(desc->buff, packet, len);
	desc->status |= DESC_TXSTS_TXFIRST | DESC_TXSTS_TXLAST;
	desc->cntrl |= (len << DESC_TXCTRL_SIZE1SHFT) & DESC_TXCTRL_SIZE1MASK;

	desc->status &= ~(DESC_TXSTS_MSK);
	desc->status |= DESC_TXSTS_OWNBYDMA;

	/* Test the wrap-around condition. */
	if (++desc_num >= STMMAC_TXDESC_NUM)
		desc_num = 0;

	eif->tx_currdescnum = desc_num;

	/* Start the transmission */
	writel(POLL_DATA, &dma->tx_poll_demand);

	return 0;
}

static int stmmac_hw_init(struct netif *netif)
{
	struct ethernetif *eif = netif->state;
	struct stmmac_mac_regs *mac = eif->mac_regs;
	struct stmmac_dma_regs *dma = eif->dma_regs;
	u32 val;

	/* Reset ethernet hardware */
	if (stmmac_reset(eif) < 0)
		return -1;

	/* Resore the HW MAC address as it has been lost during MAC reset */
	stmmac_set_hwaddr(eif);

	writel(FIXEDBURST | PRIORXTX_41 | BURST_16, &dma->busmode);

	val = readl(&dma->opmode);
	val |= FLUSHTXFIFO | STOREFORWARD | TXSECONDFRAME;
	writel(val, &dma->opmode);

	/* duplex and gbit */
	val = FRAMEBURSTENABLE | DISABLERXOWN | FULLDPLXMODE;
	writel(val, &mac->config);

	stmmac_descs_init(eif);

	/* mask mac interrupts */
	writel(0x207, &mac->intmask);
	/* only enable rx interrupt */
	writel(DMA_INTR_ENA_RIE | DMA_INTR_ENA_NIE, &dma->intenable);

	request_irq(eif->irq, stmmac_interrupt, netif);

	/* Start/Enable xfer at dma as well as mac level */
	writel(readl(&dma->opmode) | RXSTART, &dma->opmode);
	writel(readl(&dma->opmode) | TXSTART, &dma->opmode);
	writel(readl(&mac->config) | RXENABLE | TXENABLE, &mac->config);

	return 0;
}


/********************************************************************
 * Setup PHY
 *
 *******************************************************************/
static int stmmac_mdio_read(struct ethernetif *eif, u8 addr, u8 reg, u16 *val)
{
	struct stmmac_mac_regs *mac = eif->mac_regs;
	u32 miiaddr = ((addr << MIIADDRSHIFT) & MII_ADDRMSK)
		| ((reg << MIIREGSHIFT) & MII_REGMSK);
	int limit = 100000;

	writel(miiaddr | MII_CLKRANGE_150_250M | MII_BUSY, &mac->miiaddr);
	while (readl(&mac->miiaddr) & MII_BUSY && limit--)
		*val = readl(&mac->miidata);
	return 0;
}

static int stmmac_mdio_write(struct ethernetif *eif, u8 addr, u8 reg, u16 val)
{
	struct stmmac_mac_regs *mac = eif->mac_regs;
	u32 miiaddr = ((addr << MIIADDRSHIFT) & MII_ADDRMSK)
		| ((reg << MIIREGSHIFT) & MII_REGMSK) | MII_WRITE;
	int limit = 100000;

	writel(val, &mac->miidata);
	writel(miiaddr | MII_CLKRANGE_150_250M | MII_BUSY, &mac->miiaddr);
	while (readl(&mac->miiaddr) & MII_BUSY && limit--)
		;

	/* Needed as a fix for ST-Phy */
	stmmac_mdio_read(eif, addr, reg, &val);
	return 0;
}

static int stmmac_find_phy(struct ethernetif *eif)
{
	int phy_addr = 0;
	u16 ctrl, oldctrl;

	do {
		stmmac_mdio_read(eif, phy_addr, MII_BMCR, &ctrl);
		oldctrl = ctrl & BMCR_ANENABLE;

		ctrl ^= BMCR_ANENABLE;
		stmmac_mdio_write(eif, phy_addr, MII_BMCR, ctrl);
		stmmac_mdio_read(eif, phy_addr, MII_BMCR, &ctrl);
		ctrl &= BMCR_ANENABLE;

		if (ctrl == oldctrl) {
			phy_addr++;
		} else {
			ctrl ^= BMCR_ANENABLE;
			stmmac_mdio_write(eif, phy_addr, MII_BMCR, ctrl);
			return phy_addr;
		}
	} while (phy_addr < 32);

	return -1;
}

static int stmmac_reset_phy(struct ethernetif *eif)
{
	u16 ctrl;
	int limit = 100000;
	u32 phy_addr = eif->phy_addr;

	stmmac_mdio_write(eif, phy_addr, MII_BMCR, BMCR_RESET);

	do {
		stmmac_mdio_read(eif, phy_addr, MII_BMCR, &ctrl);
	} while ((ctrl & BMCR_RESET) && --limit);

	return 0;

}

static int stmmac_emdio_write(struct ethernetif *eif, int phyaddr, int phyreg,
			      u16 phydata)
{
	int ret = stmmac_mdio_write(eif, phyaddr,
			KSZ9021_EXTREG_CTRL, 0x8000 | phyreg);
	if (ret) {
		pr_warn("stmmac_emdio_write write1 failed %d\n", ret);
		return ret;
	}

	ret = stmmac_mdio_write(eif, phyaddr,
			KSZ9021_EXTREG_DATA_WRITE, phydata);
	if (ret) {
		pr_warn("stmmac_emdio_write write2 failed %d\n", ret);
		return ret;
	}

	return ret;
}

static int socfpga_phy_reset_mii(struct ethernetif *eif, int phy_addr)
{
	int ret;

	/* add 2 ns of RXC PAD Skew and 2.6 ns of TXC PAD Skew */
	ret = stmmac_emdio_write(eif, phy_addr,
			MII_KSZ9021_EXT_RGMII_CLOCK_SKEW, 0xa0d0);

	/* set no PAD skew for data */
	ret = stmmac_emdio_write(eif, phy_addr,
			MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW, 0);
	return ret;
}

/* Set up extend functions of the altera on-board phy */
static int board_phy_init(struct ethernetif *eif)
{
	u16 bmcr;
	u16 bmsr;
	int phy_addr;
	int limit = 100000;

	phy_addr = stmmac_find_phy(eif);
	if (phy_addr < 0)
		return -1;

	eif->phy_addr = phy_addr;

	if (stmmac_reset_phy(eif) < 0)
		return -1;

	socfpga_phy_reset_mii(eif, phy_addr);

	/* Set Auto-Neg Advertisement capabilities to 10/100 half/full */
	stmmac_mdio_write(eif, phy_addr, MII_ADVERTISE, 0x1E1);

	bmcr = BMCR_ANENABLE | BMCR_ANRESTART;
	if (stmmac_mdio_write(eif, phy_addr, MII_BMCR, bmcr) < 0)
		return -1;

	do {
		stmmac_mdio_read(eif, phy_addr, MII_BMSR, &bmsr);
	} while ((bmsr & BMSR_ANEGCOMPLETE) && --limit);

	return 0;
}

int stmmac_eth_init(struct netif *netif)
{
	struct ethernetif *eif = netif->state;

	pr_info("hello world\n");

	/* Clearing emac0 PHY interface select to 0 */
	clrbits32((void *)CONFIG_SYSMGR_EMAC_CTRL, 0x3 << 2);

	/* configure to PHY interface select choosed */
	setbits32((void *)CONFIG_SYSMGR_EMAC_CTRL, 0x1 << 2);

	eif->mac_regs = (struct stmmac_mac_regs *)0xff702000;
	eif->dma_regs = (struct stmmac_dma_regs *)0xff703000;

	eif->tx_currdescnum = 0;
	eif->rx_currdescnum = 0;

	eif->phy_addr = 4; /* By default, phy address of SoCKit is 4 */
	eif->cur_rxfrm_len = 0;
	eif->irq = 152;

	eif->send = stmmac_tx;

	/* initialize phy firstly */
	if (board_phy_init(eif))
		return -1;

	/* now, perform mac initialization */
	return stmmac_hw_init(netif);
}



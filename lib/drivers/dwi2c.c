/* ~.~ *-c-*
 *
 * Based on the TI DAVINCI I2C adapter driver.
 *
 * Copyright (C) 2006 Texas Instruments.
 * Copyright (C) 2007 MontaVista Software Inc.
 * Copyright (C) 2009 Provigent Ltd.
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

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <config.h>
#include "i2c.h"

/* Registers offset */
#define DW_IC_CON		0x0
#define DW_IC_TAR		0x4
#define DW_IC_DATA_CMD		0x10
#define DW_IC_SS_SCL_HCNT	0x14
#define DW_IC_SS_SCL_LCNT	0x18
#define DW_IC_FS_SCL_HCNT	0x1c
#define DW_IC_FS_SCL_LCNT	0x20
#define DW_IC_INTR_STAT		0x2c
#define DW_IC_INTR_MASK		0x30
#define DW_IC_RAW_INTR_STAT	0x34
#define DW_IC_RX_TL		0x38
#define DW_IC_TX_TL		0x3c
#define DW_IC_CLR_INTR		0x40
#define DW_IC_CLR_RX_UNDER	0x44
#define DW_IC_CLR_RX_OVER	0x48
#define DW_IC_CLR_TX_OVER	0x4c
#define DW_IC_CLR_RD_REQ	0x50
#define DW_IC_CLR_TX_ABRT	0x54
#define DW_IC_CLR_RX_DONE	0x58
#define DW_IC_CLR_ACTIVITY	0x5c
#define DW_IC_CLR_STOP_DET	0x60
#define DW_IC_CLR_START_DET	0x64
#define DW_IC_CLR_GEN_CALL	0x68
#define DW_IC_ENABLE		0x6c
#define DW_IC_STATUS		0x70
#define DW_IC_TXFLR		0x74
#define DW_IC_RXFLR		0x78
#define DW_IC_TX_ABRT_SOURCE	0x80
#define DW_IC_ENABLE_STATUS	0x9c
#define DW_IC_COMP_PARAM_1	0xf4
#define DW_IC_COMP_TYPE		0xfc
#define DW_IC_COMP_TYPE_VALUE	0x44570140

#define DW_IC_INTR_RX_UNDER	0x001
#define DW_IC_INTR_RX_OVER	0x002
#define DW_IC_INTR_RX_FULL	0x004
#define DW_IC_INTR_TX_OVER	0x008
#define DW_IC_INTR_TX_EMPTY	0x010
#define DW_IC_INTR_RD_REQ	0x020
#define DW_IC_INTR_TX_ABRT	0x040
#define DW_IC_INTR_RX_DONE	0x080
#define DW_IC_INTR_ACTIVITY	0x100
#define DW_IC_INTR_STOP_DET	0x200
#define DW_IC_INTR_START_DET	0x400
#define DW_IC_INTR_GEN_CALL	0x800

#define DW_IC_INTR_DEFAULT_MASK		(DW_IC_INTR_RX_FULL | \
					 DW_IC_INTR_TX_EMPTY | \
					 DW_IC_INTR_TX_ABRT | \
					 DW_IC_INTR_STOP_DET)

#define DW_IC_STATUS_ACTIVITY	0x1

#define DW_IC_ERR_TX_ABRT	0x1

#define DW_IC_TAR_10BITADDR_MASTER	BIT(12)

/* status codes */
#define STATUS_IDLE			0x0
#define STATUS_WRITE_IN_PROGRESS	0x1
#define STATUS_READ_IN_PROGRESS		0x2

/*
 * hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK	0
#define ABRT_10ADDR1_NOACK	1
#define ABRT_10ADDR2_NOACK	2
#define ABRT_TXDATA_NOACK	3
#define ABRT_GCALL_NOACK	4
#define ABRT_GCALL_READ		5
#define ABRT_SBYTE_ACKDET	7
#define ABRT_SBYTE_NORSTRT	9
#define ABRT_10B_RD_NORSTRT	10
#define ABRT_MASTER_DIS		11
#define ARB_LOST		12

#define DW_IC_TX_ABRT_7B_ADDR_NOACK	(1UL << ABRT_7B_ADDR_NOACK)
#define DW_IC_TX_ABRT_10ADDR1_NOACK	(1UL << ABRT_10ADDR1_NOACK)
#define DW_IC_TX_ABRT_10ADDR2_NOACK	(1UL << ABRT_10ADDR2_NOACK)
#define DW_IC_TX_ABRT_TXDATA_NOACK	(1UL << ABRT_TXDATA_NOACK)
#define DW_IC_TX_ABRT_GCALL_NOACK	(1UL << ABRT_GCALL_NOACK)
#define DW_IC_TX_ABRT_GCALL_READ	(1UL << ABRT_GCALL_READ)
#define DW_IC_TX_ABRT_SBYTE_ACKDET	(1UL << ABRT_SBYTE_ACKDET)
#define DW_IC_TX_ABRT_SBYTE_NORSTRT	(1UL << ABRT_SBYTE_NORSTRT)
#define DW_IC_TX_ABRT_10B_RD_NORSTRT	(1UL << ABRT_10B_RD_NORSTRT)
#define DW_IC_TX_ABRT_MASTER_DIS	(1UL << ABRT_MASTER_DIS)
#define DW_IC_TX_ARB_LOST		(1UL << ARB_LOST)

#define DW_IC_TX_ABRT_NOACK		(DW_IC_TX_ABRT_7B_ADDR_NOACK | \
					 DW_IC_TX_ABRT_10ADDR1_NOACK | \
					 DW_IC_TX_ABRT_10ADDR2_NOACK | \
					 DW_IC_TX_ABRT_TXDATA_NOACK | \
					 DW_IC_TX_ABRT_GCALL_NOACK)

#define DW_IC_CON_MASTER		0x1
#define DW_IC_CON_SPEED_STD		0x2
#define DW_IC_CON_SPEED_FAST		0x4
#define DW_IC_CON_10BITADDR_MASTER	0x10
#define DW_IC_CON_RESTART_EN		0x20
#define DW_IC_CON_SLAVE_DISABLE		0x40

#define ACCESS_SWAP			0x00000001
#define ACCESS_16BIT			0x00000002

#define DW_I2C_RETRIES			3
#define DW_I2C_TIMEOUT			HZ
#define TIMEOUT				20	/* ms */

/**
 * struct dwi2c_data - private i2c-designware data
 * @dev: driver model device node
 * @base: IO registers pointer
 * @cmd_complete: tx completion indicator
 * @clk: input reference clock
 * @cmd_err: run time hadware error code
 * @msgs: points to an array of messages currently being transfered
 * @msgs_num: the number of elements in msgs
 * @msg_write_idx: the index of the current tx message in the msgs array
 * @tx_buf_len: the length of the current tx buffer
 * @tx_buf: the current tx buffer
 * @msg_read_idx: the index of the current rx message in the msgs array
 * @rx_buf_len: the length of the current rx buffer
 * @rx_buf: the current rx buffer
 * @msg_err: error status of the current transfer
 * @status: i2c master status, one of STATUS_*
 * @abort_source: copy of the TX_ABRT_SOURCE register
 * @irq: interrupt number for the i2c master
 * @adapter: i2c subsystem adapter node
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 */
struct dwi2c_data {
	void			*base;
	u32			clk;
	int			irq;
	struct completion	wait;
	int			cmd_err;
	struct i2c_msg		*msgs;
	int			msgs_num;
	int			msg_write_idx;
	u32			tx_buf_len;
	u8			*tx_buf;
	int			msg_read_idx;
	u32			rx_buf_len;
	u8			*rx_buf;
	int			msg_err;
	unsigned int		status;
	u32			abort_source;
	u32			accessor_flags;
	u32			master_cfg;
	unsigned int		tx_fifo_depth;
	unsigned int		rx_fifo_depth;
	int			rx_outstanding;
};

static void dwi2c_interrupt(void *arg);

static char *abort_src[] = {
	[ABRT_7B_ADDR_NOACK] =
		"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
		"data not acknowledged",
	[ABRT_GCALL_NOACK] =
		"no acknowledgement for a general call",
	[ABRT_GCALL_READ] =
		"read after general call",
	[ABRT_SBYTE_ACKDET] =
		"start byte acknowledged",
	[ABRT_SBYTE_NORSTRT] =
		"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
		"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] =
		"trying to use disabled adapter",
	[ARB_LOST] =
		"lost arbitration",
};

static u32 i2c_read32(struct dwi2c_data *dwi2c, int offset)
{
	u32 val;

	if (dwi2c->accessor_flags & ACCESS_16BIT)
		val = readw(dwi2c->base + offset) |
			(readw(dwi2c->base + offset + 2) << 16);
	else
		val = readl(dwi2c->base + offset);

	if (dwi2c->accessor_flags & ACCESS_SWAP)
		return uswap_32(val);
	else
		return val;
}

static void i2c_write32(struct dwi2c_data *dwi2c, u32 val, int offset)
{
	if (dwi2c->accessor_flags & ACCESS_SWAP)
		val = uswap_32(val);

	if (dwi2c->accessor_flags & ACCESS_16BIT) {
		writew((u16)val, dwi2c->base + offset);
		writew((u16)(val >> 16), dwi2c->base + offset + 2);
	} else {
		writel(val, dwi2c->base + offset);
	}
}

static u32 dwi2c_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{

	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in dwi2c_scl_lcnt().
		 */
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
}

static u32 dwi2c_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

static void __i2c_dw_enable(struct dwi2c_data *dwi2c, int enable)
{
	int timeout = 100;

	do {
		i2c_write32(dwi2c, enable, DW_IC_ENABLE);
		if ((i2c_read32(dwi2c, DW_IC_ENABLE_STATUS) & 1) == enable)
			return;

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		msleep(1);
	} while (timeout--);

	pr_warn("timeout in %sabling adapter\n", enable ? "en" : "dis");
}

static int dwi2c_hw_init(struct dwi2c_data *dwi2c)
{
	u32 input_clk;
	u32 hcnt, lcnt;
	u32 reg;

	reg = i2c_read32(dwi2c, DW_IC_COMP_TYPE);
	if (reg == uswap_32(DW_IC_COMP_TYPE_VALUE)) {
		/* Configure register endianess access */
		dwi2c->accessor_flags |= ACCESS_SWAP;
	} else if (reg == (DW_IC_COMP_TYPE_VALUE & 0x0000ffff)) {
		/* Configure register access mode 16bit */
		dwi2c->accessor_flags |= ACCESS_16BIT;
	} else if (reg != DW_IC_COMP_TYPE_VALUE) {
		pr_err("Unknown Synopsys component type: 0x%08x\n", reg);
		return -ENODEV;
	}

	/* Disable the adapter */
	__i2c_dw_enable(dwi2c, false);

	/* set standard and fast speed deviders for high/low periods */
	input_clk = dwi2c->clk / 1000;

	/* Standard-mode
	 * tHD;STA=tHIGH=4.0 us, tLOW=4.7us, both tf=0.3us */
	hcnt = dwi2c_scl_hcnt(input_clk, 40, 3,	0, 0);
	lcnt = dwi2c_scl_lcnt(input_clk, 47, 3,	0);
	i2c_write32(dwi2c, hcnt, DW_IC_SS_SCL_HCNT);
	i2c_write32(dwi2c, lcnt, DW_IC_SS_SCL_LCNT);
	pr_info("Standard-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Fast-mode
	 * tHD;STA=tHIGH=0.6us, tLOW=1.3us, both tf=0.3us */
	hcnt = dwi2c_scl_hcnt(input_clk, 6, 3, 0, 0);
	lcnt = dwi2c_scl_lcnt(input_clk, 13, 3, 0);
	i2c_write32(dwi2c, hcnt, DW_IC_FS_SCL_HCNT);
	i2c_write32(dwi2c, lcnt, DW_IC_FS_SCL_LCNT);
	pr_info("Fast-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Configure Tx/Rx FIFO threshold levels */
	i2c_write32(dwi2c, dwi2c->tx_fifo_depth - 1, DW_IC_TX_TL);
	i2c_write32(dwi2c, 0, DW_IC_RX_TL);

	/* configure the i2c master */
	i2c_write32(dwi2c, dwi2c->master_cfg, DW_IC_CON);

	return 0;
}

/* Waiting for bus not busy */
static int dwi2c_wait_bus_not_busy(struct dwi2c_data *dwi2c)
{
	int timeout = TIMEOUT;

	while (i2c_read32(dwi2c, DW_IC_STATUS) & DW_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			pr_err("timeout waiting for bus ready\n");
			return -ETIMEDOUT;
		}
		timeout--;
		msleep(1);
	}

	return 0;
}

static void dwi2c_xfer_init(struct dwi2c_data *dwi2c)
{
	struct i2c_msg *msgs = dwi2c->msgs;
	u32 ic_con, ic_tar = 0;


	/* Disable the adapter */
	__i2c_dw_enable(dwi2c, false);

	/* set the slave (target) address */
	i2c_write32(dwi2c, msgs[dwi2c->msg_write_idx].addr, DW_IC_TAR);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = i2c_read32(dwi2c, DW_IC_CON);
	if (msgs[dwi2c->msg_write_idx].flags & I2C_M_TEN) {
		ic_con |= DW_IC_CON_10BITADDR_MASTER;

		/*
		 * If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers.
		 */
		ic_tar = DW_IC_TAR_10BITADDR_MASTER;
	} else {
		ic_con &= ~DW_IC_CON_10BITADDR_MASTER;
	}

	i2c_write32(dwi2c, ic_con, DW_IC_CON);

	/*
	 * Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable.
	 */
	i2c_write32(dwi2c, msgs[dwi2c->msg_write_idx].addr | ic_tar, DW_IC_TAR);


	/* Enable the adapter */
	__i2c_dw_enable(dwi2c, true);

	/*XXX Clear and enable interrupts */
	i2c_read32(dwi2c, DW_IC_CLR_INTR);
	i2c_write32(dwi2c, DW_IC_INTR_DEFAULT_MASK, DW_IC_INTR_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from dwi2c_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void dwi2c_xfer_msg(struct dwi2c_data *dwi2c)
{
	struct i2c_msg *msgs = dwi2c->msgs;
	u32 intr_mask;
	int tx_limit, rx_limit;
	u32 addr = msgs[dwi2c->msg_write_idx].addr;
	u32 buf_len = dwi2c->tx_buf_len;
	u8 *buf = dwi2c->tx_buf;
	bool need_restart = false;

	intr_mask = DW_IC_INTR_DEFAULT_MASK;
	for (; dwi2c->msg_write_idx < dwi2c->msgs_num; dwi2c->msg_write_idx++) {
		/*
		 * if target address has changed, we need to
		 * reprogram the target address in the i2c
		 * adapter when we are done with this transfer
		 */
		if (msgs[dwi2c->msg_write_idx].addr != addr) {
			pr_err("%s: invalid target address\n", __func__);
			dwi2c->msg_err = -EINVAL;
			break;
		}

		if (msgs[dwi2c->msg_write_idx].len == 0) {
			pr_err("%s: invalid message length\n", __func__);
			dwi2c->msg_err = -EINVAL;
			break;
		}

		if (!(dwi2c->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dwi2c->msg_write_idx].buf;
			buf_len = msgs[dwi2c->msg_write_idx].len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages.
			 */
			if ((dwi2c->master_cfg & DW_IC_CON_RESTART_EN) &&
					(dwi2c->msg_write_idx > 0))
				need_restart = true;
		}

		tx_limit = dwi2c->tx_fifo_depth
				- i2c_read32(dwi2c, DW_IC_TXFLR);
		rx_limit = dwi2c->rx_fifo_depth
				- i2c_read32(dwi2c, DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */
			if (dwi2c->msg_write_idx == dwi2c->msgs_num - 1 &&
					buf_len == 1)
				cmd |= BIT(9);

			if (need_restart) {
				cmd |= BIT(10);
				need_restart = false;
			}

			if (msgs[dwi2c->msg_write_idx].flags & I2C_M_RD) {

				/* avoid rx buffer overrun */
				if (rx_limit - dwi2c->rx_outstanding <= 0)
					break;

				i2c_write32(dwi2c, cmd | 0x100, DW_IC_DATA_CMD);
				rx_limit--;
				dwi2c->rx_outstanding++;
			} else {
				i2c_write32(dwi2c, cmd | *buf++,
						DW_IC_DATA_CMD);
			}
			tx_limit--; buf_len--;
		}

		dwi2c->tx_buf = buf;
		dwi2c->tx_buf_len = buf_len;

		if (buf_len > 0) {
			/* more bytes to be written */
			dwi2c->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dwi2c->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dwi2c->msg_write_idx == dwi2c->msgs_num)
		intr_mask &= ~DW_IC_INTR_TX_EMPTY;

	if (dwi2c->msg_err)
		intr_mask = 0;

	i2c_write32(dwi2c, intr_mask,  DW_IC_INTR_MASK);
}

static void dwi2c_read(struct dwi2c_data *dwi2c)
{
	struct i2c_msg *msgs = dwi2c->msgs;
	int rx_valid;

	for (; dwi2c->msg_read_idx < dwi2c->msgs_num; dwi2c->msg_read_idx++) {
		u32 len;
		u8 *buf;

		if (!(msgs[dwi2c->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dwi2c->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dwi2c->msg_read_idx].len;
			buf = msgs[dwi2c->msg_read_idx].buf;
		} else {
			len = dwi2c->rx_buf_len;
			buf = dwi2c->rx_buf;
		}

		rx_valid = i2c_read32(dwi2c, DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = i2c_read32(dwi2c, DW_IC_DATA_CMD);
			dwi2c->rx_outstanding--;
		}

		if (len > 0) {
			dwi2c->status |= STATUS_READ_IN_PROGRESS;
			dwi2c->rx_buf_len = len;
			dwi2c->rx_buf = buf;
			return;
		} else
			dwi2c->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int dwi2c_handle_tx_abort(struct dwi2c_data *dwi2c)
{
	unsigned long abort_source = dwi2c->abort_source;
	int nr = sizeof(abort_src) / sizeof(abort_src[0]);
	int i;

	for (i = 0; i < nr; i++) {
		if (abort_source & (1 << i))
			pr_err("%s: %s\n", __func__, abort_src[i]);
	}

	if (abort_source & DW_IC_TX_ABRT_NOACK)
		return -EREMOTEIO;

	if (abort_source & DW_IC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & DW_IC_TX_ABRT_GCALL_READ)
		return -EINVAL; /* wrong msgs[] data */
	else
		return -EIO;
}

static int
dwi2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	int rval;
	struct dwi2c_data *dwi2c;

	dwi2c = (struct dwi2c_data *)adap->priv;
	if (dwi2c == NULL) {
		pr_err("%s: adapter has not been registered\n", __func__);
		return -EINVAL;
	}

	INIT_COMPLETION(dwi2c->wait);
	dwi2c->msgs = msgs;
	dwi2c->msgs_num = num;
	dwi2c->cmd_err = 0;
	dwi2c->msg_write_idx = 0;
	dwi2c->msg_read_idx = 0;
	dwi2c->msg_err = 0;
	dwi2c->status = STATUS_IDLE;
	dwi2c->abort_source = 0;
	dwi2c->rx_outstanding = 0;

	rval = dwi2c_wait_bus_not_busy(dwi2c);
	if (rval < 0)
		return rval;

	/* start the transfers */
	dwi2c_xfer_init(dwi2c);

	/* wait for tx to complete */
	rval = wait_for_completion_timeout(&dwi2c->wait, adap->timeout);
	if (rval == 0) {
		pr_err("controller timed out\n");
		dwi2c_hw_init(dwi2c);
		return -ETIMEDOUT;
	}

	__i2c_dw_enable(dwi2c, false);

	if (dwi2c->msg_err)
		return dwi2c->msg_err;

	/* no error */
	if (!dwi2c->cmd_err)
		return num;

	/* We have an error */
	if (dwi2c->cmd_err == DW_IC_ERR_TX_ABRT)
		return dwi2c_handle_tx_abort(dwi2c);

	return -EIO;
}

static u32 dwi2c_read_clear_intrbits(struct dwi2c_data *dwi2c)
{
	u32 stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = i2c_read32(IC_INTR_STAT);
	 * equals to,
	 *   stat = i2c_read32(IC_RAW_INTR_STAT) & i2c_read32(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = i2c_read32(dwi2c, DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * i2c_read32(IC_INTR_STAT) to i2c_read32(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_RX_UNDER)
		i2c_read32(dwi2c, DW_IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		i2c_read32(dwi2c, DW_IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		i2c_read32(dwi2c, DW_IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RD_REQ)
		i2c_read32(dwi2c, DW_IC_CLR_RD_REQ);
	if (stat & DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		dwi2c->abort_source = i2c_read32(dwi2c, DW_IC_TX_ABRT_SOURCE);
		i2c_read32(dwi2c, DW_IC_CLR_TX_ABRT);
	}
	if (stat & DW_IC_INTR_RX_DONE)
		i2c_read32(dwi2c, DW_IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		i2c_read32(dwi2c, DW_IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		i2c_read32(dwi2c, DW_IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		i2c_read32(dwi2c, DW_IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		i2c_read32(dwi2c, DW_IC_CLR_GEN_CALL);

	return stat;
}

static void dwi2c_interrupt(void *arg)
{
	struct dwi2c_data *dwi2c = (struct dwi2c_data *)arg;
	u32 stat, enabled;

	enabled = i2c_read32(dwi2c, DW_IC_ENABLE);
	stat = i2c_read32(dwi2c, DW_IC_RAW_INTR_STAT);
	pr_debug("%s:  enabled= 0x%x stat=0x%x\n", __func__, enabled, stat);
	if (!enabled || !(stat & ~DW_IC_INTR_ACTIVITY)) {
		dwi2c_read_clear_intrbits(dwi2c);
		return;
	}

	stat = dwi2c_read_clear_intrbits(dwi2c);

	if (stat & DW_IC_INTR_TX_ABRT) {
		dwi2c->cmd_err |= DW_IC_ERR_TX_ABRT;
		dwi2c->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed.  Make sure to skip them.
		 */
		i2c_write32(dwi2c, 0, DW_IC_INTR_MASK);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_RX_FULL)
		dwi2c_read(dwi2c);

	if (stat & DW_IC_INTR_TX_EMPTY)
		dwi2c_xfer_msg(dwi2c);

	/*
	 * No need to modify or disable the interrupt mask here.
	 * dwi2c_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */
tx_aborted:
	if ((stat & (DW_IC_INTR_TX_ABRT|DW_IC_INTR_STOP_DET)) || dwi2c->msg_err)
		complete(&dwi2c->wait);
}

int dwi2c_add_numbered_adapter(int num)
{
	struct i2c_adapter *adap;
	struct dwi2c_data *dwi2c;
	int rval;

	if (i2c_is_numbered_adapter_registered(num))
		return -EINVAL;

	adap = calloc(1, sizeof(struct i2c_adapter));
	if (adap == NULL) {
		pr_err("%s: no memory for i2c adapter\n", __func__);
		return -ENOMEM;
	}

	dwi2c = calloc(1, sizeof(struct dwi2c_data));
	if (dwi2c == NULL) {
		pr_err("%s: no memory for private data\n", __func__);
		rval = ENOMEM;
		goto err_malloc_dwi2c;
	}

	dwi2c->irq = 190 + num;
	dwi2c->clk = CONFIG_HPS_CLK_PERVCO_HZ;
	dwi2c->base = (void *)(SOCFPGA_I2C0_ADDRESS + num * 0x1000);

	rval = i2c_read32(dwi2c, DW_IC_COMP_PARAM_1);
	dwi2c->tx_fifo_depth = ((rval >> 16) & 0xff) + 1;
	dwi2c->rx_fifo_depth = ((rval >> 8)  & 0xff) + 1;
	dwi2c->master_cfg = DW_IC_CON_MASTER
		| DW_IC_CON_SLAVE_DISABLE
		| DW_IC_CON_RESTART_EN
		| DW_IC_CON_SPEED_STD;
		//| DW_IC_CON_SPEED_FAST;

	rval = dwi2c_hw_init(dwi2c);
	if (rval)
		goto err_hw_init;

	init_completion(&dwi2c->wait);

	/* disable interrupt */
	i2c_write32(dwi2c, 0, DW_IC_INTR_MASK);
	i2c_read32(dwi2c, DW_IC_CLR_INTR);

	request_irq(dwi2c->irq, dwi2c_interrupt, dwi2c);

	adap->priv = dwi2c;
	adap->xfer = dwi2c_xfer;
	adap->retries = DW_I2C_RETRIES;
	adap->timeout = DW_I2C_TIMEOUT;

	/* to register a adapter, number xfer must be initialized */
	rval = i2c_register_adapter(adap, num);
	if (rval)
		goto err_register_adapter;

	return 0;

err_register_adapter:
	free_irq(dwi2c->irq);
err_hw_init:
	free(dwi2c);
err_malloc_dwi2c:
	free(adap);
	return rval;
}


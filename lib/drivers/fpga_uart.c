/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Feb 27 14:49:49 CST 2014
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

/* XXX
 * 1. this uart has a pre-setting and could not be configured
 * 2. 4 channels, each channel binds to an adapter
 */

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include <fifo.h>
#include <delay.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <platform.h>
#include <fpga_uart.h>

#define CONFIG_UART_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x1000)
#define CONFIG_UART_IRQ		77
#define CONFIG_UART_FIFO_SIZE	64
#define CONFIG_UART_CNT		0x4
#define CONFIG_UART_BUFSIZE	256

#define UART_SR_OFFS		0x00
#define UART_CR_OFFS		0x04
#define UART_IR_OFFS		0x08
#define UART_IER_OFFS		0x0c
#define UART_IMR_OFFS		0x10
#define UART_CIR_OFFS		0x14
#define UART_HEAD0_COM_OFFS	0x20
#define UART_HEAD1_COM_OFFS	0x24
#define UART_HEAD2_COM_OFFS	0x28
#define UART_HEAD3_COM_OFFS	0x2c

#define UFLAG_RX		1
#define UFLAG_TX		0

struct __msg {
	char	*buf;
	int	len;
	int	idx;
};

struct __adapter {
	void			*base;
	void			*dreg;		/* fifo r/w reg addr */
	int			num;

	char			rxbuf[CONFIG_UART_BUFSIZE];
	struct fifo		*rxfifo;
	struct completion	rxwait;
	struct mutex		rx_mutex;

	struct __msg		tx_msg;
	struct completion	txwait;
	struct mutex		tx_mutex;

	unsigned long		timeout;
};

static struct __adapter *__adapters;

static inline struct __adapter *get_numbered_adapter(int num)
{
	return &__adapters[num];
}

static inline void __irq_enable(struct __adapter *adap, u32 mask)
{
	setbits32(adap->base + UART_IER_OFFS, mask);
}
static inline void __irq_disable(struct __adapter *adap, u32 mask)
{
	clrbits32(adap->base + UART_IER_OFFS, mask);
}
static inline void __irq_clr(struct __adapter *adap, u32 mask)
{
	setbits32(adap->base + UART_CIR_OFFS, mask);
}
static inline int __is_bus_busy(struct __adapter *adap, u32 mask)
{
	return readl(adap->base + UART_SR_OFFS) & mask;
}

/*
 * cache data rxed to the uart ring fifo
 * if the ring is full, then stop rxirq and so rx is disabled.
 * we will enable it while read out data from the fifo.
 */
static void rx_handler(struct __adapter *adap, u32 mask)
{
	while (!__is_bus_busy(adap, mask)) {
		u8 val;

		if (fifo_unused(adap->rxfifo) == 0) {
			__irq_disable(adap, mask);
			break;
		}

		val = readb(adap->dreg);
		fifo_in(adap->rxfifo, &val, 1);
	}
	__irq_clr(adap, mask);
}

static void tx_handler(struct __adapter *adap, u32 mask)
{
	struct __msg *msg = &adap->tx_msg;

	while (!__is_bus_busy(adap, mask)) {

		if (msg->idx >= msg->len) {
			__irq_disable(adap, mask);
			complete(&adap->txwait);
			break;
		} else {
			writeb(msg->buf[msg->idx++], adap->dreg);
		}
	}

	__irq_clr(adap, mask);
}

static void irq_handler(void *arg)
{
	u32 stat;
	int adap_num;
	struct __adapter *adap0 = get_numbered_adapter(0);

	stat = readl(adap0->base + UART_IR_OFFS);
	for (adap_num = 0; adap_num < CONFIG_UART_CNT; adap_num++) {
		struct __adapter *adap = get_numbered_adapter(adap_num);

		/* txfifo is not full */
		if (stat & (1 << adap->num))
			tx_handler(adap, 1 << adap->num);

		/* rxfifo is not empty */
		if (stat & (1 << (adap->num + 4)))
			rx_handler(adap, 1 << (adap->num + 4));
	}
}

/* TODO: now, only support non-blocking I/O. */
int uart_read(int adap_num, void *data, u32 len)
{
	struct __adapter *adap = get_numbered_adapter(adap_num);
	int retry = 5;
	int rval;

	mutex_lock(&adap->tx_mutex);
	while (len > fifo_cached(adap->rxfifo)) {
		retry--;
		if (!retry) {
			rval = -EAGAIN;
			goto out;
		}
		msleep(1);
	}

	rval = fifo_out(adap->rxfifo, data, len);
	__irq_enable(adap, 1 << (adap_num + 4));

out:
	mutex_unlock(&adap->tx_mutex);
	return rval;
}

int uart_write(int adap_num, const void *data, u32 len)
{
	struct __adapter *adap = get_numbered_adapter(adap_num);
	u32 mask = 1 << adap->num;
	int rval;

	mutex_lock(&adap->tx_mutex);

	INIT_COMPLETION(adap->txwait);
	adap->tx_msg.idx = 0;
	adap->tx_msg.len = len;
	adap->tx_msg.buf = (char *)data;

	__irq_enable(adap, mask);
	rval = wait_for_completion_timeout(&adap->txwait, adap->timeout);

	__irq_disable(adap, mask);
	__irq_clr(adap, mask);

	if (rval == 0) {
		pr_err("controller timed out\n");
		setbits32(adap->base + UART_CR_OFFS, mask << 16);
		rval = -ETIMEDOUT;
	} else {
		rval = len;
	}

	mutex_unlock(&adap->tx_mutex);
	return rval;
}

int uart_init(void)
{
	int adap_num;

	__adapters = calloc(CONFIG_UART_CNT, sizeof(struct __adapter));
	if (__adapters == NULL) {
		pr_err("%s: no memory for adapters\n", __func__);
		return -ENOMEM;
	}

	for (adap_num = 0; adap_num < CONFIG_UART_CNT; adap_num++) {
		struct __adapter *adap = get_numbered_adapter(adap_num);

		adap->base = (void *)CONFIG_UART_BASE;
		adap->dreg = adap->base + UART_HEAD0_COM_OFFS + 4 * adap_num;
		adap->num = adap_num;

		adap->rxfifo = fifo_init(adap->rxbuf, 1, sizeof(adap->rxbuf));
		init_completion(&adap->rxwait);
		mutex_init(&adap->rx_mutex);

		init_completion(&adap->txwait);
		mutex_init(&adap->tx_mutex);

		adap->timeout = 5 * HZ;
	}

	writel(0xf0, __adapters->base + UART_IER_OFFS);	/* enable all rx irq */
	writel(0xf0, __adapters->base + UART_CR_OFFS);	/* enable all rx */

	request_irq(CONFIG_UART_IRQ, irq_handler, __adapters);

	return 0;
}


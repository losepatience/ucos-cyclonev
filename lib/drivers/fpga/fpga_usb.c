/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Feb 27 14:50:00 CST 2014
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

/* this device is function specified and not for common use
 * it has tow endpiont. one is w/r bidirected while another
 * is only for w.
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <platform.h>
#include <fpga_usb.h>

#define CONFIG_USB_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x800)
#define CONFIG_USB_IRQ		76
#define CONFIG_USB_PORT_CNT	3
#define CONFIG_USB_WRPORT_CNT	2

#define USB_SR_OFFS		0x0
#define USB_CR_OFFS		0x4
#define USB_IR_OFFS		0x8
#define USB_IER_OFFS		0xc
#define USB_IMR_OFFS		0x10
#define USB_CIR_OFFS		0x14

#define USB_CTX_OFFS		0x20
#define USB_CRX_OFFS		0x24
#define USB_STX_OFFS		0x28

#define SUPPORT_USB3
#ifndef SUPPORT_USB3
#  define MAXLEN		64
#else
#  define MAXLEN		512
#endif

#define BURST			12

struct __endpoint {
	void			*base;
	void			*io;
	u32			mask;
	int			burst;
#define PORT_TYPE_OUT		1
#define PORT_TYPE_IN		0

	spinlock_t		lock;
};

static struct __endpoint *__ep;

static inline struct __endpoint *__get_endpoint(int num)
{
	return &__ep[num];
}

static int wait_bus_not_busy(struct __endpoint *ep)
{
	unsigned int stat;
	int retries = 5;

	do {
		stat = readl(ep->base + USB_SR_OFFS);
		if (!(stat & ep->mask))	/*FIXME*/
			return 1;

		udelay(10);

	} while (retries--);

	return 0;
}

static void fx3_isr(void *arg)
{
	struct __endpoint *ep = (struct __endpoint *)arg;
	unsigned int request[2];

	if (!ep->rx_callback)
		goto out;

	request[0] = readl(ep->io);
	request[1] = readl(ep->io);
	ep->rx_callback(request);

out:
	/* clean the corresponding irq bit */
	setbits32(ep->base + USB_CIR_OFFS, ep->mask);
}

ssize_t fx3_read(int num, void *buf, ssize_t len)
{
	struct __endpoint *ep = __get_endpoint(num);
	char *pbuf = buf;
	ssize_t tmp = len;
	int retries = 5;

	unsigned long flags = 0;

	spin_lock_irqsave(&ep->lock, flags);

	do {
		if (readl(ep->base + USB_SR_OFFS) & ep->mask) {
			if (retries--)
				udelay(100);
			else
				break;
		}

		*pbuf++ = readl(ep->io);
	} while (--tmp);

	spin_unlock_irqrestore(&ep->lock, flags);

	return len - tmp;
}


/*
 * 1. txirq is triggled only while the txfifo is empty.
 * 2. the fifo is large enough for a packet(12-512)
 * 3. this func could be called in irq, and could not sleep.
 */
static ssize_t __write(struct __endpoint *ep, const void *buf,
			ssize_t len, int flag)
{
	int retries = 5;
	u32 status;

	u8 tmp[BURST] = { 0 };
	u32 padding;
	u32 *p;

	unsigned long flags = 0;

	do {
		status = readl(ep->base + USB_SR_OFFS);

		if (--retries == 0)
			break;

		msleep(1);

	} while (!(status & ep->mask));

	if (!(status & ep->mask));
		return -ETIMEDOUT;

	padding = ep->burst - len % ep->burst;
	tmp[0] = padding;
	if (flag)
		tmp[0] |= 1 << 4;

	memcpy(tmp + padding, buf, ep->burst - padding);

	spin_lock_irqsave(&ep->lock, flags);
	for (p = (u32 *)tmp; (u8 *)p < tmp + ep->burst; p++)
		writel(*p, ep->io);

	p = (u32 *)(buf + ep->burst - padding);
	while ((void *)p < buf + len)
		writel(*p++, ep->io);

	/* start to xmit */
	writel(ep->mask, ep->base + USB_CR_OFFS);
	spin_unlock_irqrestore(&ep->lock, flags);

	return len;
}

inline ssize_t fx3_write(int num, const void *buf, ssize_t len)
{
	return __write(__get_endpoint(num), buf, len, 0);
}

inline ssize_t fx3_send_cmd(int num, const void *buf, ssize_t len)
{
	return __write(__get_endpoint(num), buf, len, 1);
}

void fx3_flush_fifo(int num)
{
	struct __endpoint *ep = __get_endpoint(num);
	writel(ep->mask << 16, ep->base + USB_CR_OFFS);
}

inline void fx3_register_request_cb(int (*func)(void *))
{
	__get_endpoint(1)->rx_callback = func;
}

int fx3_init(void)
{
	struct __endpoint *ep;
	int i;
	u32 mask[] = { 1 << 3, 1 << 1, 1 << 4, };

	__ep = calloc(CONFIG_USB_PORT_CNT, sizeof(struct __endpoint));
	if (!__ep) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	for (i = 0; i < CONFIG_USB_PORT_CNT; i++) {
		ep = __get_endpoint(i);

		ep->base	= (void *)CONFIG_USB_BASE;
		ep->io		= ep->base + USB_CTX_OFFS + i * 4;
		ep->mask	= mask[i];
		ep->burst	= BURST;
	}

	/* enable ep0 rx irq */
	ep = __get_endpoint(1);
	request_irq(CONFIG_USB_IRQ, fx3_isr, ep);
	setbits32(ep->base + USB_IER_OFFS, ep->mask);

	return 0;
}


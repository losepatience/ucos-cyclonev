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

struct __port {
	void			*base;
	u32			offs;
	u32			mask;
	int			burst;
#define PORT_TYPE_OUT		1
#define PORT_TYPE_IN		0

	spinlock_t		lock;
	int			(*rx_callback)(void *);
};

static u32 ports_mask[] = {
	1 << 3,	/* 1:cmd tx fifo empty */
	1 << 1,	/* 1:cmd rx fifo empty */
	1 << 4,	/* 1:status tx fifo empty */
};

static struct __port *__ports;

static inline struct __port *__get_port(int num)
{
	return &__ports[num];
}

static void fx3_request_interrupt(void *arg)
{
	struct __port *port = (struct __port *)arg;
	unsigned int request[2];

	if (!port->rx_callback)
		goto out;

	request[0] = readl(port->base + port->offs);
	request[1] = readl(port->base + port->offs);
	port->rx_callback(request);

out:
	/* clean the corresponding irq bit */
	setbits32(port->base + USB_CIR_OFFS, port->mask);
}

ssize_t fx3_read(int num, void *buf, ssize_t len)
{
	struct __port *port = __get_port(num);
	char *pbuf = buf;
	ssize_t tmp = len;
	int retries = 5;

	unsigned long flags = 0;

	spin_lock_irqsave(&port->lock, flags);

	do {
		if (readl(port->base + USB_SR_OFFS) & port->mask) {
			if (retries--)
				udelay(100);
			else
				break;
		}

		*pbuf++ = readl(port->base + port->offs);
	} while (--tmp);

	spin_unlock_irqrestore(&port->lock, flags);

	return len - tmp;
}


/*
 * 1. txirq is triggled only while the txfifo is empty.
 * 2. the fifo is large enough for a packet(12-512)
 * 3. this func could be called in irq, and could not sleep.
 */
static ssize_t __write(struct __port *port, const void *buf,
			ssize_t len, int flag)
{
	int retries = 5;
	u32 status;

	u8 tmp[BURST] = { 0 };
	u32 padding;
	u32 *p;

	unsigned long flags = 0;

	do {
		status = readl(port->base + USB_SR_OFFS);

		if (--retries == 0)
			break;

		msleep(1);

	} while (!(status & port->mask));

	if (!(status & port->mask));
		return -ETIMEDOUT;

	padding = port->burst - len % port->burst;
	tmp[0] = padding;
	if (flag)
		tmp[0] |= 1 << 4;

	memcpy(tmp + padding, buf, port->burst - padding);

	spin_lock_irqsave(&port->lock, flags);
	for (p = (u32 *)tmp; (u8 *)p < tmp + port->burst; p++)
		writel(*p, port->base + port->offs);

	p = (u32 *)(buf + port->burst - padding);
	while ((void *)p < buf + len)
		writel(*p++, port->base + port->offs);

	/* start to xmit */
	writel(port->mask, port->base + USB_CR_OFFS);
	spin_unlock_irqrestore(&port->lock, flags);

	return len;
}

inline ssize_t fx3_write(int num, const void *buf, ssize_t len)
{
	return __write(__get_port(num), buf, len, 0);
}

inline ssize_t fx3_send_cmd(int num, const void *buf, ssize_t len)
{
	return __write(__get_port(num), buf, len, 1);
}

void fx3_flush_fifo(int num)
{
	struct __port *port = __get_port(num);
	writel(port->mask << 16, port->base + USB_CR_OFFS);
}

inline void fx3_register_request_cb(int (*func)(void *))
{
	__get_port(1)->rx_callback = func;
}

int fx3_init(void)
{
	int i;
	void *base;

	__ports = calloc(CONFIG_USB_PORT_CNT, sizeof(struct __port));
	if (!__ports) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	base = (void *)CONFIG_USB_BASE;
	for (i = 0; i < CONFIG_USB_PORT_CNT; i++) {
		struct __port *port = __get_port(i);

		port->base = base;
		port->offs = USB_CTX_OFFS + 4 * i;
		port->mask = ports_mask[i];
		port->burst = BURST;
	}

	request_irq(CONFIG_USB_IRQ, fx3_request_interrupt, &__ports[1]);

	/* enable ep0 rx interrupt */
	setbits32(base + USB_IER_OFFS, 1 << 1);
	return 0;
}


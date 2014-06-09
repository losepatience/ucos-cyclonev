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

/* warning: this struct use u32 as unit */
struct __msg {
	u32	*buf;	/* 32bits register */
	int	idx;
	int	len;
};

struct __port {
	u32			base;
	u32			offs;
	u32			bitmask;
	int			type;
	int			burst;
#define PORT_TYPE_OUT		1
#define PORT_TYPE_IN		0

	spinlock_t		lock;
	int			(*rx_callback)(void *);
};

static struct __port *__ports;

static inline struct __port *__get_port(int num)
{
	return &__ports[num];
}

static inline void __irq_enable(struct __port *port)
{
	setbits_le32(port->base + USB_IER_OFFS, port->bitmask);
}

static inline void __irq_disable(struct __port *port)
{
	clrbits_le32(port->base + USB_IER_OFFS, port->bitmask);
}

static inline void __irq_clr(struct __port *port)
{
	setbits_le32(port->base + USB_CIR_OFFS, port->bitmask);
}

static inline int __is_port_busy(struct __port *port)
{
	if (port->type == PORT_TYPE_IN)
		return readl(port->base + USB_SR_OFFS) & port->bitmask;

	return !(readl(port->base + USB_SR_OFFS) & port->bitmask << 3);
}

static int __wait_bus_not_busy(struct __port *port)
{
	int retry = 10000;

	while (__is_port_busy(port)) {
		if (retry <= 0) {
			pr_warn("timeout waiting for bus ready\n");
			return -ETIMEDOUT;
		}
		retry--;
	}

	return 0;
}

static void usb_irq(void *arg)
{
	struct __port *port = &__ports[1];
	unsigned int request[2];

	request[0] = readl(port->base + port->offs);
	request[1] = readl(port->base + port->offs);

	if (port->rx_callback)
		port->rx_callback(request);

	__irq_clr(port);
}

ssize_t usb_read(int num, void *buf, ssize_t len)
{
	struct __port *port = __get_port(num);
	u32 *tmp = (u32 *)buf;
	unsigned long flags = 0;

	spin_lock_irqsave(&port->lock, flags);

	while ((void *)tmp < (buf + len) && !__is_port_busy(port))
		*tmp++ = readl(port->base + port->offs);

	__irq_clr(port);

	spin_unlock_irqrestore(&port->lock, flags);

	return buf + len - (void *)tmp;
}


/*
 * 1. txirq is triggled only while the txfifo is empty.
 * 2. the fifo is large enough for a packet(12-512)
 * 3. this func could be called in irq, and could not sleep.
 */
static ssize_t __write(struct __port *port, const void *buf,
			ssize_t len, int flag)
{
	unsigned long flags = 0;
	u8 tmp[BURST] = {0};
	u32 padding;
	u32 *p;
	int i;

	if (__wait_bus_not_busy(port))
		return -ETIMEDOUT;

	padding = port->burst - len % port->burst;
	tmp[0] = padding;
	if (flag)
		tmp[0] |= 1 << 4;

	for (i = padding; i < port->burst; i++)
		tmp[i] = ((u8 *)buf)[i - padding];

	p = (u32 *)tmp;

	spin_lock_irqsave(&port->lock, flags);
	while ((u8 *)p < tmp + port->burst)
		writel(*p++, port->base + port->offs);

	p = (u32 *)(buf + port->burst - padding);
	while ((void *)p < buf + len)
		writel(*p++, port->base + port->offs);

	/* start to xmit */
	writel(port->bitmask, port->base + USB_CR_OFFS);
	spin_unlock_irqrestore(&port->lock, flags);

	return len;
}

inline ssize_t usb_write(int num, const void *buf, ssize_t len)
{
	return __write(__get_port(num), buf, len, 0);
}

inline ssize_t usb_send_cmd(int num, const void *buf, ssize_t len)
{
	return __write(__get_port(num), buf, len, 1);
}

inline void usb_flush_fifo(int num)
{
	struct __port *port = __get_port(num);
	writel(port->bitmask << 16, port->base + USB_CR_OFFS);
}

inline void usb_disable(void)
{
	__irq_disable(&__ports[1]);
}

inline void usb_enable(void)
{
	__irq_enable(&__ports[1]);
}

inline void usb_register_callback(int (*func)(void *))
{
	__get_port(1)->rx_callback = func;
}

int usb_init(void)
{
	int i;

	__ports = calloc(CONFIG_USB_PORT_CNT, sizeof(struct __port));
	if (!__ports) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	for (i = 0; i < CONFIG_USB_PORT_CNT; i++) {
		struct __port *port = __get_port(i);

		port->base = CONFIG_USB_BASE;
		port->offs = USB_CTX_OFFS + 4 * i;
		port->bitmask = 1 << i;
		port->burst = BURST;
		port->type = PORT_TYPE_OUT;
	}
	__ports[1].type = PORT_TYPE_IN;

	request_irq(CONFIG_USB_IRQ, usb_irq, __ports);
	__irq_enable(&__ports[1]);
	return 0;
}


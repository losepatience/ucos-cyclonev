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
 * 2. 4 channels, each channel binds to an portter
 */

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include <fifo.h>
#include <delay.h>
#include <asm/io.h>
#include <platform.h>
#include <fpga_uart.h>

#define CONFIG_UART_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x1000)
#define CONFIG_UART_IRQ		77
#define CONFIG_UART_FIFO_SIZE	64
#define CONFIG_UART_CNT		0x4

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

struct __port {
	void			*base;
	void			*dreg;	/* fifo r/w reg addr */
	int			num;

	u32			rxmask;
	u32			txmask;

	struct mutex            rxmutex;
	struct mutex            txmutex;

	u8			*rxbuf;
	int			rxlen;
	int			rxidx;
	bool			rx_ready;

	unsigned long		timeout;
};

struct __port *__ports;

static inline struct __port *get_numbered_port(int num)
{
	return &__ports[num];
}

static void rx_handler(struct __port *port)
{
	while (port->rxidx < port->rxlen) {
		u32 stat = readl(port->base + UART_SR_OFFS);
		if (stat & port->rxmask)
			return;

		port->rxbuf[port->rxidx++] = readb(port->dreg);
	}

	/* disable rx irq of current port */
	clrbits32(port->base + UART_IER_OFFS, port->rxmask);
	port->rx_ready = true;
}

static void interrupt_rx(void *arg)
{
	u32 stat;
	int port_num = 0;
	struct __port *port = get_numbered_port(0);

	stat = readl(port->base + UART_IR_OFFS);
	for ( ; port_num < CONFIG_UART_CNT; port++, port_num++) {

		if (stat & port->rxmask) {
			rx_handler(port);

			/* clear the irq bit */
			setbits32(port->base + UART_CIR_OFFS, port->rxmask);
		}
	}
}

int fpga_uart_read(int num, u8 *buf, int len)
{
	struct __port *port = get_numbered_port(num);
	int rval;

	mutex_lock(&port->rxmutex);

	port->rxidx = 0;
	port->rxlen = len;
	port->rxbuf = buf;
	port->rx_ready = false;

	/* enable irq */
	setbits32(port->base + UART_IER_OFFS, port->rxmask);
	if (wait_for_condition(&port->rx_ready, port->timeout))	/* 100 ms */
		rval = len;
	else
		rval = -ETIMEDOUT;
	clrbits32(port->base + UART_IER_OFFS, port->rxmask);

	mutex_unlock(&port->rxmutex);
	return rval;
}

int fpga_uart_write(int num, const u8 *buf, int len)
{
	struct __port *port = get_numbered_port(num);
	int idx = 0;

	mutex_lock(&port->txmutex);

	while (idx < len) {

		/* txfifo full */
		u32 stat = readl(port->base + UART_SR_OFFS);

		if (stat & port->txmask)
			break;
		else
			writeb(buf[idx++], port->dreg);
	}

	mutex_unlock(&port->txmutex);

	if (idx == 0)
		return -EIO;

	return idx;
}

int fpga_uart_init(int num)
{
	//u32 val;

	__ports = (struct __port *)calloc(4, sizeof(struct __port));
	if (__ports == NULL) {
		pr_err("%s: no memory aveilable\n", __func__);
		return -ENOMEM;
	}

	for (num = 0; num < CONFIG_UART_CNT; num++) {
		struct __port *port = get_numbered_port(num);

		port->base = (void *)CONFIG_UART_BASE;;
		port->dreg = port->base + UART_HEAD0_COM_OFFS + 4 * num;
		port->num = num;
		port->txmask = 1 << num;
		port->rxmask = 1 << (num + 4);
		port->rx_ready = false;
		port->timeout = 100;

		mutex_init(&port->rxmutex);
		mutex_init(&port->txmutex);
	}

	/* enable all rx */
	writel(0xf0, __ports->base + UART_CR_OFFS);
	request_irq(CONFIG_UART_IRQ, interrupt_rx, __ports);

	return 0;
}


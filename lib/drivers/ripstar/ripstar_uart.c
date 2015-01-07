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
 * 2. 4 channels, each channel binds to a port
 */

#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <uart_core.h>

#define CONFIG_UART_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x1000)
#define CONFIG_UART_IRQ		77
#define CONFIG_UART_FIFO_SIZE	64

#define UARTSR		0x00
#define UARTCR		0x04
#define UARTIR		0x08
#define UARTIER		0x0c
#define UARTIMR		0x10
#define UARTCIR		0x14
#define UARTIOR		0x20

static inline void cyc_uart_start_tx(struct uart_port *port)
{
	setbits32(port->base + UARTIER, 1 << port->id);
}

static inline void cyc_uart_stop_tx(struct uart_port *port)
{
	clrbits32(port->base + UARTIER, 1 << port->id);
}

static void cyc_uart_tx_chars(struct uart_port *port)
{
	char ch;
	struct fifo *fifo = port->txfifo;

	if (!fifo_cached(fifo)) {
		cyc_uart_stop_tx(port);
		return;
	}

	fifo_out(fifo, &ch, 1);
	writeb(ch, port->iobase);
}

static void cyc_uart_rx_chars(struct uart_port *port)
{
	char ch;
	struct fifo *fifo = port->rxfifo;

	if (!fifo_unused(fifo))
		return;

	ch = readb(port->iobase);
	fifo_in(fifo, &ch, 1);
}

static void cyc_uart_isr(void *arg)
{
	unsigned int shift;
	unsigned int estat;

	estat = readl((void *)CONFIG_UART_BASE + UARTIR);

	for (shift = 0; estat && (shift < 4); shift++) {

		struct uart_port *port = to_uart_port(shift);
		unsigned int rx = 1 << (shift + 4);
		unsigned int tx = 1 << shift;

		if (estat & tx) {
			estat &= ~tx;
			cyc_uart_tx_chars(port);
			setbits32(port->base + UARTCIR, tx);
		}

		if (estat & rx) {
			estat &= ~rx;
			cyc_uart_rx_chars(port);

			if (port->rxcb)
				port->rxcb(&port->id);

			setbits32(port->base + UARTCIR, rx);
		}

	}
}

int cyc_uart_port_add(int num)
{
	struct uart_port *port;
	unsigned int size;
	void *buf;

	port = (struct uart_port *)malloc(sizeof(struct uart_port));
	if (!port) {
		pr_err("%s: no memory aveilable\n", __func__);
		return -ENOMEM;
	}
	memset(port, 0, sizeof(struct uart_port));

	size = UART_XMIT_SIZE;
	buf = malloc(size * 2);
	if (!port) {
		free(port);
		pr_err("%s: no memory aveilable\n", __func__);
		return -ENOMEM;
	}

	port->base	= (void *)CONFIG_UART_BASE;;
	port->iobase	= port->base + UARTIOR + (num << 2);
	port->id	= num;
	port->start_tx	= cyc_uart_start_tx;
	port->rxfifo	= fifo_init(buf, 1, size);
	port->txfifo	= fifo_init(buf + size, 1, size);
	port->irq	= CONFIG_UART_IRQ;
	port->fifosize	= size;

	request_irq(port->irq, cyc_uart_isr, NULL);
	uart_port_add(port);

	/* enable rx */
	setbits32(port->base + UARTIER, 1 << (num + 4));
	setbits32(port->base + UARTCR, 1 << (num + 4));

	return 0;
}



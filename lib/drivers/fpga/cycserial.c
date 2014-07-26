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
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <serial_core.h>

#define CONFIG_UART_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x1000)
#define CONFIG_UART_IRQ		77
#define CONFIG_UART_FIFO_SIZE	64

#define UARTSR		0x00
#define UARTCR		0x04
#define UARTIR		0x08
#define UARTIER		0x0c
#define UARTIMR		0x10
#define UARTCIR		0x14
#define UARTIO0		0x20

/* return 1: still busy */
static int wait_bus_not_busy(struct uart_port *port, unsigned int bits)
{
	int retries = 5;

	do {
		unsigned int stat = readl(port->base + UARTSR);

		if (!(stat & bits))
			return 0;

		udelay(100);

	} while (retries--);

	return 1;
}

static void cycserial_isr(void *arg)
{
	struct uart_port *port = (struct uart_port *)arg;
	unsigned int stat;
	int i;

	stat = readl(port->base + UARTIR);
	for (i = 0; i < ARCH_NR_UARTPORTS - 1; i++) {
		unsigned int mask = 1 << (i + 4);

		if (stat & mask) {
			if (port->rxcb)
				port->rxcb(&port->id);

			/* clear the irq bit */
			setbits32(port->base + UARTCIR, mask);
		}
	}
}

static int cycserial_read(struct uart_port *port, u8 *buf, int len)
{
	int i = 0;

	while (len--) {

		unsigned int stat = readl(port->base + UARTSR);

		if (stat & (port->mask << 4))
			break;

		buf[i++] = readb(port->iobase);
	}

	return i;
}

static void cycserial_putchar(struct uart_port *port, unsigned char c)
{
	/* check if tx buffer is full */
	wait_bus_not_busy(port, port->mask);
	writeb(c, port->iobase);
}

static void cycserial_start_tx(struct uart_port *port)
{
	struct fifo *fifo = port->txfifo;

	while (fifo_cached(fifo)) {
		char ch;

		fifo_out(fifo, &ch, 1);
		cycserial_putchar(port, ch);
	}
}

int cycserial_init(void (*func)(void *))
{
	int i;
	unsigned long offs;
	struct uart_port *port, *saved;

	port = (struct uart_port *)calloc(4, sizeof(struct uart_port));
	if (!port) {
		pr_err("%s: no memory aveilable\n", __func__);
		return -ENOMEM;
	}

	saved = port;

	offs = UARTIO0;
	for (i = 0; i < ARCH_NR_UARTPORTS - 1; port++, i++, offs += 4) {

		port->base = (void *)CONFIG_UART_BASE;;
		port->iobase = port->base + offs;
		//port->id = i + 1;	/* 1 port reserved */
		port->id = i;
		port->mask = 1 << i;

		port->start_tx = cycserial_start_tx;
		port->read = cycserial_read;

		if (func) {
			port->rxcb = func;
			setbits32(port->base + UARTIER, port->mask << 4);
		}

		port->txfifo = fifo_init(port->txbuf, 1, UART_XMIT_SIZE);

		/* enable rx */
		setbits32(port->base + UARTCR, port->mask << 4 );

		uartport_add(port);
	}

	request_irq(CONFIG_UART_IRQ, cycserial_isr, saved);
	return 0;
}


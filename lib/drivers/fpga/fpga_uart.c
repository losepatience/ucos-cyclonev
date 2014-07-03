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
#include <asm/regs.h>
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

	unsigned long		retries;
};

struct __port *__ports[10];

static inline struct __port *get_numbered_port(int num)
{
	return __ports[num];
}

static inline int is_rxfifo_empty(struct __port *port)
{
	return readl(port->base + UART_SR_OFFS) & port->rxmask;
}

static inline int is_txfifo_full(struct __port *port)
{
	return readl(port->base + UART_SR_OFFS) & port->txmask;
}

int fpga_uart_read(int num, char *buf, int len)
{
	struct __port *port = get_numbered_port(num);
	int idx = 0;

	mutex_lock(&port->rxmutex);

	while (idx < len) {
		int retries = port->retries;

		if (is_rxfifo_empty(port)) {
			if (retries == 0) {
				break;
			} else {
				retries--;
				msleep(1);
			}
		}

		buf[idx++] = readb(port->dreg);
	}

	mutex_unlock(&port->rxmutex);
	return idx;
}

int fpga_uart_write(int num, const char *buf, int len)
{
	struct __port *port = get_numbered_port(num);
	int idx = 0;

	mutex_lock(&port->txmutex);

	while (idx < len) {
		int retries = port->retries;

		if (is_txfifo_full(port)) {
			if (retries == 0) {
				break;
			} else {
				retries--;
				msleep(1);
			}
		}

		writeb(buf[idx++], port->dreg);
	}

	mutex_unlock(&port->txmutex);
	return idx;
}

int fpga_uart_init(int num)
{
	struct __port *port;

	port = calloc(1, sizeof(struct __port));
	if (port == NULL) {
		pr_err("%s: no memory aveilable\n", __func__);
		return -ENOMEM;
	}

	port->base = (void *)CONFIG_UART_BASE;;
	port->dreg = port->base + UART_HEAD0_COM_OFFS + 4 * num;
	port->num = num;
	port->txmask = 1 << num;
	port->rxmask = 1 << (num + 4);

	mutex_init(&port->rxmutex);
	mutex_init(&port->txmutex);
	port->retries = 5;

	__ports[num] = port;

	return 0;
}


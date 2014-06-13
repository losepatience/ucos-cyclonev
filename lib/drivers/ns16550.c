/* ~.~ *-c-*
 *
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

#include "ns16550.h"
#include <stdio.h>

#define MODE_X_DIV 16

static struct ns16550 *port = (struct ns16550 *)SOCFPGA_UART0_ADDRESS;

static int calc_divisor(int baudrate)
{
	return (CONFIG_SYS_NS16550_CLK + (baudrate * (MODE_X_DIV / 2)))
		/ (MODE_X_DIV * baudrate);
}

static int ns16550_setup(void)
{
	u32 dummy;
	u32 baud_div;
	int i;

	/*
	 * fixed Synopsys UART v3.13a bug
	 * using a dummy loop to avoid it being optimized
	 */
	for (i = 0; i < 2; i++) {
		dummy = readb(&port->msr);
		if (dummy)
			break;
	}

	writeb(CONFIG_SYS_NS16550_IER, &port->ier);
	writeb(UART_LCR_BKSE | UART_LCRVAL, &port->lcr);
	writeb(0, &port->dll);
	writeb(0, &port->dlm);
	writeb(UART_LCRVAL, &port->lcr);	/* 8IN1 */
	writeb(UART_MCRVAL, &port->mcr);
	writeb(UART_FCRVAL, &port->fcr);
	writeb(UART_LCR_BKSE | UART_LCRVAL, &port->lcr);

	baud_div = calc_divisor(CONFIG_BAUDRATE);
	writeb(baud_div & 0xff, &port->dll);
	writeb((baud_div >> 8) & 0xff, &port->dlm);
	writeb(UART_LCRVAL, &port->lcr);
	return 0;
}

static void ns16550_putc(char c)
{
	while ((readb(&port->lsr) & UART_LSR_THRE) == 0)
		;
	writeb(c, &port->thr);
}

static struct stdio_dev ns16550 = {
	.name = "ns16550",
	.putc = ns16550_putc
};

void ns16550_init(void)
{
	ns16550_setup();
	stdio_register(stdout, &ns16550);
}


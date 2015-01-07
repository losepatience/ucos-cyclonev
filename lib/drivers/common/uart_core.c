/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 21 16:39:15 CST 2014
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

#include <uart_core.h>
#include <string.h>

struct uart_port *uart_ports[ARCH_NR_UARTPORTS];

struct uart_port *to_uart_port(int num)
{
	return uart_ports[num];
}

static void uart_stop(struct uart_port *port)
{
	local_irq_save(port->irqflags);
	port->stop_tx(port);
	local_irq_restore(port->irqflags);
}

static void uart_start(struct uart_port *port)
{
	struct fifo *fifo = port->txfifo;

	local_irq_save(port->irqflags);
	if (fifo_cached(fifo))
		port->start_tx(port);
	local_irq_restore(port->irqflags);
}

int uart_write(int num, const unsigned char *buf, int count)
{
	struct uart_port *port = to_uart_port(num);
	struct fifo *fifo = port->txfifo;
	int ret = 0;

	local_irq_save(port->irqflags);
	while (1) {
		int c = fifo_unused(fifo);
		if (count < c)
			c = count;
		if (c <= 0)
			break;
		fifo_in(fifo, buf, c);

		buf += c;
		count -= c;
		ret += c;
	}
	local_irq_restore(port->irqflags);

	uart_start(port);
	return ret;
}

int uart_read(int num, unsigned char *buf, int count)
{
	struct uart_port *port = to_uart_port(num);
	struct fifo *fifo = port->rxfifo;
	int ret = 0;

	local_irq_save(port->irqflags);
	while (1) {
		int c = fifo_cached(fifo);
		if (count > c)
			c = count;
		if (c <= 0)
			break;
		fifo_out(fifo, buf, c);

		buf += c;
		count -= c;
		ret += c;
	}
	local_irq_restore(port->irqflags);

	return ret;
}

void uart_register_rxcb(int num, callback_t rxcb)
{
	struct uart_port *port = to_uart_port(num);

	local_irq_save(port->irqflags);
	port->rxcb = rxcb;
	local_irq_restore(port->irqflags);
}

int uart_port_add(struct uart_port *port)
{
	local_irq_save(port->irqflags);
	uart_ports[port->id] = port;
	local_irq_restore(port->irqflags);

	return 0;
}


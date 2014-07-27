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

#include <serial_core.h>
#include <string.h>

struct uart_port *serial_ports[ARCH_NR_UARTPORTS];

struct uart_port *to_uart_port(int num)
{
	return serial_ports[num];
}

static void serial_start_tx(struct uart_port *port)
{
	struct fifo *fifo = port->txfifo;

	spin_lock_irqsave(&port->lock, port->irqflags);
	if (fifo_cached(fifo))
		port->start_tx(port);
	spin_unlock_irqrestore(&port->lock, port->irqflags);
}

int serial_write(int num, const unsigned char *buf, int count)
{
	struct uart_port *port = to_uart_port(num);
	struct fifo *fifo = port->txfifo;
	int ret = 0;

	spin_lock_irqsave(&port->lock, port->irqflags);
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
	spin_unlock_irqrestore(&port->lock, port->irqflags);

	serial_start_tx(port);
	return ret;
}

int serial_read(int num, unsigned char *buf, int count)
{
	struct uart_port *port = to_uart_port(num);
	struct fifo *fifo = port->rxfifo;
	int ret = 0;

	spin_lock_irqsave(&port->lock, port->irqflags);
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
	spin_unlock_irqrestore(&port->lock, port->irqflags);

	return ret;
}

void serial_register_rxcb(int num, callback_t rxcb)
{
	struct uart_port *port = to_uart_port(num);
	spin_lock_irqsave(&port->lock, port->irqflags);
	port->rxcb = rxcb;
	spin_unlock_irqrestore(&port->lock, port->irqflags);
}

int serial_port_add(struct uart_port *port)
{
	spin_lock_irqsave(&port->lock, port->irqflags);
	serial_ports[port->id] = port;
	spin_unlock_irqrestore(&port->lock, port->irqflags);

	return 0;
}


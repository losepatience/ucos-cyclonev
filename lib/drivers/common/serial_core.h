/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 21 16:34:44 CST 2014
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


#ifndef __SERIAL_CORE_H__
#define __SERIAL_CORE_H__

#include <platform.h>
#include <fifo.h>
#include <stddef.h>

#define ARCH_NR_UARTPORTS	(1 + 4)
#define UART_XMIT_SIZE		(512)

struct uart_port {
	spinlock_t	lock;
	unsigned char	*base;
	void		*iobase;

	unsigned int	id;
	unsigned int	irq;
	unsigned int	mask;

	struct fifo	*txfifo;
	char		txbuf[UART_XMIT_SIZE];
	void		(*start_tx)(struct uart_port *);

	callback_t	rxcb;
	int		(*read)(struct uart_port *, unsigned char *, int);
	
	void		*priv;
};

int uart_read(int num, unsigned char *buf, int count);
int uart_write(int num, const unsigned char *buf, int count);
int uartport_add(struct uart_port *port);

#endif

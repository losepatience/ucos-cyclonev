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
#include <cycfx3.h>

#define CONFIG_FX3_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x800)
#define CONFIG_FX3_IRQ		76
#define CONFIG_FX3_PORT_CNT	3
#define CONFIG_FX3_WRPORT_CNT	2

#define FX3SR			0x00
#define FX3CR			0x04
#define FX3IR			0x08
#define FX3IER			0x0C
#define FX3IMR			0x10
#define FX3CIR			0x14

#define FX3CTX			0x20
#define FX3CRX			0x24
#define FX3STX			0x28

#define SUPPORT_FX33
#ifndef SUPPORT_FX33
#  define MAXLEN		64
#else
#  define MAXLEN		512
#endif

#define BURST			12

struct cycfx3_port {
	void			*base;
	void			*iobase;
	int			type;
	u32			mask;
	int			burst;
#define PORT_TYPE_OUT		1
#define PORT_TYPE_IN		0

	spinlock_t		lock;
};

static struct cycfx3_port *cycfx3_ports;

static int is_port_busy(struct cycfx3_port *port)
{
	u32 busy;

	busy = readl(port->base + FX3SR) & port->mask;
	if (port->type == CYCFX3_TX_PORT)
		busy = !busy;

	return busy;
}

static void cycfx3_isr(void *arg)
{
	struct cycfx3_port *port = (struct cycfx3_port *)arg;
	unsigned int req[2];

	if (!port->rx_callback)
		goto out;

	req[0] = readl(port->iobase);
	req[1] = readl(port->iobase);
	port->rx_callback(req);

out:
	/* clean the corresponding irq bit */
	setbits32(port->base + FX3CIR, port->mask);
}

static int cycfx3_read(struct cycfx3_port *port, char *buf, int len)
{
	int i = 0;
	unsigned long flags = 0;

	/* must be 32bits aligned */
	if (len % 4)
		len -= len % 4;

	spin_lock_irqsave(&port->lock, flags);
	while (len) {

		if (is_port_busy(port))
			break;

		buf[i] = readl(port->iobase);
		i += 4;
		len -= 4;
	}
	spin_unlock_irqrestore(&port->lock, flags);
	return i;
}


/*
 * 1. txirq is triggled only while the txfifo is empty.
 * 2. the fifo is large enough for a packet(12-512)
 * 3. this func could be called in irq, and could not sleep.
 */
static int __write(struct cycfx3_port *port, const char *buf, int len, int flag)
{
	char padbuf[BURST] = { 0 };
	char *p = padbuf;
	const char *p1;
	u32 padding;

	unsigned long flags = 0;

	if (is_port_busy(port))
		return 0;

	padding = port->burst - len % port->burst;
	padbuf[0] = padding;
	if (flag)
		padbuf[0] |= 1 << 4;

	memcpy(padbuf + padding, buf, port->burst - padding);

	spin_lock_irqsave(&port->lock, flags);
	for (; p < padbuf + port->burst; p += 4)
		writel(*((u32 *)p), port->iobase);

	for (p1 = buf + port->burst - padding; p1 < buf + len; p1 += 4)
		writel(*((u32 *)p1), port->iobase);

	/* start to xmit */
	writel(port->mask, port->base + FX3CR);
	spin_unlock_irqrestore(&port->lock, flags);

	return len;
}

static inline ssize_t cycfx3_write(struct cycfx3_port *port,
				const char *buf, int len)
{
	return __write(port, buf, len, 0);
}

static inline int cycfx3_echo(struct cycfx3_port *port,
				const void *buf, int len)
{
	return __write(port, buf, len, 1);
}

static inline void cycfx3_flush_fifo(struct cycfx3_port *port)
{
	writel(port->mask << 16, port->base + FX3CR);
}

#if 0
inline void cycfx3_register_request_cb(int (*func)(void *))
{
	__get_port(1)->rx_callback = func;

}
#endif

int cycfx3_init(void)
{
	struct cycfx3_port *port;
	int i;
	u32 mask[] = { 1 << 3, 1 << 1, 1 << 4, };
	u32 offset;

	cycfx3_ports = calloc(CONFIG_FX3_PORT_CNT, sizeof(struct cycfx3_port));
	if (!cycfx3_ports) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	offset = FX3CTX;
	for (i = 0; i < CONFIG_FX3_PORT_CNT; i++, offset += 4) {
		port = __get_port(i);

		port->base	= (void *)CONFIG_FX3_BASE;
		port->iobase	= port->base + offset;
		port->mask	= mask[i];
		port->burst	= BURST;
	}

	/* enable ep0 rx irq */
	port = __get_port(1);
	request_irq(CONFIG_FX3_IRQ, cycfx3_isr, port);
	setbits32(port->base + FX3IER, port->mask);

	return 0;
}




u8 usb_request_type(usb_request_t *req)
{
	return (req->type >> 5) & 0x3;
}

u16 usb_request_len(usb_request_t *req)
{
	return req->len;
}

u8 usb_request_dir(usb_request_t *req)
{
	return req->type & 0x80;
}

char usb_read(u8 ep, void *data, u32 len, TransferCallback cb, void *arg)
{
	int cnt = cycfx3_read(1, data, len);
	if (cb)
		cb(arg, USB_STATUS_SUCCESS, cnt, len - cnt);
	return USB_STATUS_SUCCESS;
}

char usb_write(u8 ep, const void *data, u32 l, TransferCallback cb, void *arg)
{
	u8 status = USB_STATUS_SUCCESS;
	int cnt = cycfx3_write(ep / 3, data, l);

	if (cnt != l)
		status = USB_STATUS_LOCKED;

	if (cb)
		cb(arg, status, cnt, l - cnt);

	return status;
}

u8 usb_get_state(void)
{
	return __usb_state;
}

u8 usb_is_high_speed(void)
{
	return __usb_speed == CY_U3P_SUPER_SPEED;
}

void usb_connect(void)
{
	usb_cmd_t cmd = {
		.len = 0,
		.cmd = USB_CMD_CONNECT,
	};

	//cycfx3_enable();
	cycfx3_echo(0, &cmd, sizeof(cmd));
}

void usb_disconnect(void)
{
	usb_cmd_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};

	__usb_state = USB_STATE_DEFAULT;
	//cycfx3_disable();
	cycfx3_echo(0, &cmd, sizeof(cmd));
}

u8 usb_stall(u8 endpoint)
{
	char buf[sizeof(usb_cmd_t) + 1];
	usb_cmd_t *cmd = (usb_cmd_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	return USB_STATUS_SUCCESS;
}

char usb_read_abort(u8 endpoint)
{
	char buf[sizeof(usb_cmd_t) + 1];
	usb_cmd_t *cmd = (usb_cmd_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	cycfx3_flush_fifo(endpoint);
	return USB_STATUS_SUCCESS;
}

char usb_write_abort(u8 endpoint)
{
	char buf[sizeof(usb_cmd_t) + 1];
	usb_cmd_t *cmd = (usb_cmd_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_WRITE;
	cmd->data[0] = endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	cycfx3_flush_fifo(endpoint / 3);	/* XXX */
	return USB_STATUS_SUCCESS;
}


extern void USBCallbacks_RequestReceived(const usb_request *req);

/* called in irq, so could not have mutex */
int cycfx3_request_cb(void *arg)
{
	usb_request *req = (usb_request *)arg;

	if (ubr_GetType(req) == ubr_STANDARD) {
		/* __usb_state = req->wValue; */
		__usb_state = USB_STATE_CONFIGURED;
		__usb_speed = req->wIndex;
	}

	USBCallbacks_RequestReceived(req);
	return 0;
}

u8 USB_Init(void)
{
	if (cycfx3_init())
		return false;

	cycfx3_register_request_cb(cycfx3_request_cb);
	return true;
}

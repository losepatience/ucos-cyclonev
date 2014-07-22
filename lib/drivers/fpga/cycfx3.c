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
	spinlock_t		lock;

	void			*base;
	void			*iobase;

	int			type;
	u32			mask;
	int			burst;
	void			(*request_callback)(USBGenericRequest *);

};

static struct cycfx3_port *cycfx3_ports;

static int is_port_busy(struct cycfx3_port *port)
{
	u32 busy;

	busy = readl(port->base + FX3SR) & port->mask;
	if (port->type == USBGenericRequest_IN)
		busy = !busy;

	return busy;
}

/* the waterlevel is fixed to 8 bytes */
static void cycfx3_isr(void *arg)
{
	struct cycfx3_port *port = (struct cycfx3_port *)arg;
	unsigned int q[2];

	if (!port->request_callback)
		goto out;

	q[0] = readl(port->iobase);
	q[1] = readl(port->iobase);
	port->request_callback((USBGenericRequest *)q);

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

int cycfx3_init(void (*func)(USBGenericRequest *))
{
	struct cycfx3_port *port;
	u32 mask[] = { 1 << 3, 1 << 1, 1 << 4, };
	u32 type[] = { 1, 0, 1 };
	u32 offset;
	int i;

	cycfx3_ports = calloc(CONFIG_FX3_PORT_CNT, sizeof(struct cycfx3_port));
	if (!cycfx3_ports) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	offset = FX3CTX;
	for (i = 0; i < CONFIG_FX3_PORT_CNT; i++, offset += 4) {
		port = &cycfx3_ports[i];

		port->base	= (void *)CONFIG_FX3_BASE;
		port->iobase	= port->base + offset;
		port->mask	= mask[i];
		port->type	= type[i];
		port->burst	= BURST;
	}

	port = &cycfx3_ports[1];
	port->request_callback = func;

	/* enable ep0 rx irq */
	request_irq(CONFIG_FX3_IRQ, cycfx3_isr, port);
	setbits32(port->base + FX3IER, port->mask);

	return 0;
}



static u16 __usb_state;
static u16 __usb_speed;

u8 USBGenericRequest_GetType(const USBGenericRequest *request)
{
	return ((request->bmRequestType >> 5) & 0x3);
}

u8 USBGenericRequest_GetRequest(const USBGenericRequest *request)
{
	return request->bRequest;
}

u16 USBGenericRequest_GetValue(const USBGenericRequest *request)
{
	return request->wValue;
}

u16 USBGenericRequest_GetIndex(const USBGenericRequest *request)
{
	return request->wIndex;
}

u16 USBGenericRequest_GetLength(const USBGenericRequest *request)
{
	return request->wLength;
}

u8 USBGenericRequest_GetDirection(const USBGenericRequest *request)
{
	if ((request->bmRequestType & 0x80) != 0)
		return USBGenericRequest_IN;
	else
		return USBGenericRequest_OUT;
}

char USBD_Read(u8 ep, void *data, u32 len, TransferCallback cb, void *arg)
{
	int cnt = cycfx3_read(1, data, len);
	if (cb)
		cb(arg, USBD_STATUS_SUCCESS, cnt, len - cnt);
	return USBD_STATUS_SUCCESS;
}

char USBD_Write(u8 ep, const void *data, u32 l, TransferCallback cb, void *arg)
{
	u8 status = USBD_STATUS_SUCCESS;
	int cnt = cycfx3_write(ep / 3, data, l);

	if (cnt != l)
		status = USBD_STATUS_LOCKED;

	if (cb)
		cb(arg, status, cnt, l - cnt);

	return status;
}

u8 USBD_GetState(void)
{
	return __usb_state;
}

u8 USBD_IsHighSpeed(void)
{
	return __usb_speed == CY_U3P_SUPER_SPEED;
}

void USBD_Connect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_CONNECT,
	};

	//usb_enable();
	cycfx3_echo(0, &cmd, sizeof(cmd));
}

void USBD_Disconnect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};

	__usb_state = USBD_STATE_DEFAULT;
	//usb_disable();
	cycfx3_echo(0, &cmd, sizeof(cmd));
}

u8 USBD_Stall(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = Endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataRead(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = Endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	cycfx3_flush_fifo(Endpoint);
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataWrite(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_WRITE;
	cmd->data[0] = Endpoint;

	cycfx3_echo(0, cmd, sizeof(buf));
	cycfx3_flush_fifo(Endpoint / 3);	/* XXX */
	return USBD_STATUS_SUCCESS;
}


extern void USBDCallbacks_RequestReceived(const USBGenericRequest *req);

/* called in irq, so could not have mutex */
static int usb_request_callback(void *arg)
{
	USBGenericRequest *req = (USBGenericRequest *)arg;	

	if (USBGenericRequest_GetType(req) == USBGenericRequest_STANDARD) {
		/* __usb_state = req->wValue; */
		__usb_state = USBD_STATE_CONFIGURED;
		__usb_speed = req->wIndex;
	}

	USBDCallbacks_RequestReceived(req);
	return 0;
}

u8 USB_Init(void)
{
	return !cycfx3_init(usb_request_callback);
}



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
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <platform.h>
#include <ezusb.h>

#define CONFIG_EZUSB__BASE	(SOCFPGA_LWH2F_ADDRESS + 0x800)
#define CONFIG_EZUSB_IRQ		76
#define EZUSB_PORT_CNT			3

#define EZUSB_SR			0x00
#define EZUSB_CR			0x04
#define EZUSB_IR			0x08
#define EZUSB_IER			0x0C
#define EZUSB_IMR			0x10
#define EZUSB_CIR			0x14

#define EZUSB_CTX			0x20
#define EZUSB_CRX			0x24
#define EZUSB_STX			0x28

#define EZUSB_FLAGS_PAD			(1 << 4)
#define BURST				12

static int io_limit = 512;

typedef struct ezusb_ids {
	int			ep;
	int			type;

	u32			mask;
} __ids_t;

struct ezusb_port {
	spinlock_t		lock;

	__ids_t			id;
	void			*base;
	void			*iobase;

	int			ready;
	int			burst;
	int			retries;
	char			wrbuf[BURST];
	callback_t		callback;
};


static __ids_t ids[] = {
	[0] = { 0, USBGenericRequest_IN, 1 << 3 },
	[1] = { 0, USBGenericRequest_OUT, 1 << 1 },
	[2] = { 6, USBGenericRequest_IN, 1 << 4 },
};

static struct ezusb_port *ezusb_ports;

static struct ezusb_port *__to_ezusb_port(u8 ep)
{
	int i;

	for (i = 0; i < EZUSB_PORT_CNT; i++)
		if (ezusb_ports[i].id.ep == ep)
			return &ezusb_ports[i];

	return NULL;
}

/* 1: busy, 0: idle */
static int wait_port_not_busy(struct ezusb_port *port)
{
	u32 stat;
	int retries = port->retries;

	do {
		stat = readl(port->base + EZUSB_SR);
		stat &= port->id.mask;
		if (port->id.type == USBGenericRequest_IN)
			stat = !stat;

		if (!stat)
			break;

		udelay(100);
	} while (--retries);

	return stat;
}

/* the waterlevel is fixed to 8 bytes by hardware */
static void ezusb_isr(void *arg)
{
	struct ezusb_port *port = (struct ezusb_port *)arg;
	unsigned int q[2];

	if (!port->callback)
		goto out;

	q[0] = readl(port->iobase);
	q[1] = readl(port->iobase);
	port->callback((USBGenericRequest *)q);

out:
	/* clean the corresponding irq bit */
	setbits32(port->base + EZUSB_CIR, port->id.mask);
}

static int __read(struct ezusb_port *port, void *buf, int len)
{
	unsigned int *p;
	unsigned long flags = 0;

	if (wait_port_not_busy(port))
		return -EBUSY;

	if (len > io_limit)
		len = io_limit;

	len /= 4;	/* must be 4bytes aligned */

	p = (unsigned int *)buf;

	spin_lock_irqsave(&port->lock, flags);
	while (len) {
		*p++ = readl(port->iobase);
		len--;
	}
	spin_unlock_irqrestore(&port->lock, flags);
	return (char *)p - (char *)buf;
}

static int ezusb_read(struct ezusb_port *port, void *buf, int len)
{
	char *p;
	int ret;

	if (port->ready == 0)
		return -EIO;

	p = (char *)buf;
	len -= len % 4;
	ret = 0;

	while (len > 0) {
		int l = __read(port, buf, len);
		if (l <= 0)
			break;

		len -= l; 
		ret += l;
		p += l;
	}

	return ret;
}

/*
 * 1. txirq is triggled only while the txfifo is empty.
 * 2. the fifo is large enough for a packet(12-512)
 * 3. this func could be called in irq, and could not sleep.
 */
static int __write(struct ezusb_port *port, const void *buf, int len, int flag)
{
	int cnt;
	int padding;
	unsigned int *p;
	unsigned long flags = 0;

	if (wait_port_not_busy(port))
		return -EBUSY;

	p = (unsigned int *)buf;
	padding = 0;

	spin_lock_irqsave(&port->lock, flags);
	if (flag & EZUSB_FLAGS_PAD) {

		cnt = len % port->burst;
		padding = port->burst - cnt;

		if (len + padding > io_limit)
			len = io_limit - padding;

		memset(port->wrbuf, 0, sizeof(port->wrbuf));
		port->wrbuf[0] = padding | EZUSB_FLAGS_PAD;
		memcpy(port->wrbuf + padding, buf, cnt);

		p = (unsigned int *)port->wrbuf;
		cnt = port->burst / 4;
		while (cnt--)
			writel(p++, port->iobase);
	}

	cnt = (len - (port->burst - padding)) / 4;
	p = (unsigned int *)((char *)buf + (port->burst - padding));

	while (cnt--)
		writel(p++, port->iobase);

	/* start to xmit */
	writel(port->id.mask, port->base + EZUSB_CR);
	spin_unlock_irqrestore(&port->lock, flags);

	return len;
}

static inline ssize_t ezusb_write(struct ezusb_port *port,
				const void *buf, int len)
{
	char *p;
	int ret;
	int flag;

	if (port->ready == 0)
		return -EIO;

	if (port->id.ep == 0)
		flag = EZUSB_FLAGS_PAD;
	else
		flag = 0;

	p = (char *)buf;
	ret = 0;

	while (len > 0) {
		int l = __write(port, buf, len, flag);
		if (l <= 0)
			break;

		len -= l; 
		ret += l;
		p += l;
	}

	return ret;
}

static inline void ezusb_flush(u8 endpoint)
{
	struct ezusb_port *port = __to_ezusb_port(endpoint);
	writel(port->id.mask << 16, port->base + EZUSB_CR);
}

int ezusb_init(callback_t func)
{
	struct ezusb_port *port;
	u32 offset;
	int i;

	ezusb_ports = calloc(EZUSB_PORT_CNT, sizeof(struct ezusb_port));
	if (!ezusb_ports) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	offset = EZUSB_CTX;
	for (i = 0; i < EZUSB_PORT_CNT; i++, offset += 4) {
		port = &ezusb_ports[i];

		port->id	= ids[i];

		port->base	= (void *)CONFIG_EZUSB__BASE;
		port->iobase	= port->base + offset;
		port->burst	= BURST;
		port->retries	= 5;
	}

	port = &ezusb_ports[1];
	port->callback = func;

	/* enable ep0 rx irq */
	request_irq(CONFIG_EZUSB_IRQ, ezusb_isr, port);
	setbits32(port->base + EZUSB_IER, port->id.mask);

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

char USBD_Read(u8 ep, void *buf, u32 len, TransferCallback func, void *arg)
{
	struct ezusb_port *port = __to_ezusb_port(ep);
	char status;
	int cnt;

 	status = USBD_STATUS_SUCCESS;

	cnt = ezusb_read(port, buf, len);
	if (cnt < 0) {
		status = USBD_STATUS_LOCKED;
		cnt = 0;
	}

	if (func) 
		func(arg, status, cnt, len - cnt);

	return status;
}

char USBD_Write(u8 ep, const void *buf, u32 len,
		TransferCallback func, void *arg)
{

	struct ezusb_port *port = __to_ezusb_port(ep);
	char status;
	int cnt;

	status = USBD_STATUS_SUCCESS;

	cnt = ezusb_write(port, buf, len);
	if (cnt < 0) {
		status = USBD_STATUS_LOCKED;
		cnt = 0;
	}

	if (func)
		func(arg, status, cnt, len - cnt);

	return status;
}

u8 USBD_IsIdle(u8 ep)
{
	struct ezusb_port *port = __to_ezusb_port(ep);
	u32 stat;

	stat = readl(port->base + EZUSB_SR);
	stat &= port->id.mask;
	if (port->id.type == USBGenericRequest_OUT)
		stat = !stat;

	return stat;
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

	ezusb_write(0, &cmd, sizeof(cmd));
}

void USBD_Disconnect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};

	__usb_state = USBD_STATE_DEFAULT;
	ezusb_write(0, &cmd, sizeof(cmd));
}

u8 USBD_Stall(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	if (!USBD_IsIdle(Endpoint))
		return USBD_STATUS_LOCKED;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = Endpoint;

	ezusb_write(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

u8 USBD_ACK(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	if (!USBD_IsIdle(Endpoint))
		return USBD_STATUS_LOCKED;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ACK;
	cmd->data[0] = Endpoint;
	
	ezusb_write(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataRead(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = Endpoint;

	ezusb_write(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataWrite(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABROT_WRITE;
	cmd->data[0] = Endpoint;

	ezusb_write(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

extern void USBDCallbacks_RequestReceived(const USBGenericRequest *req);

/* called in irq, so could not have mutex */
static int ezusb_callback(void *arg)
{
	USBGenericRequest *req = (USBGenericRequest *)arg;	

	if (USBGenericRequest_GetType(req) == USBGenericRequest_STANDARD) {
		__usb_state = req->wIndex | 0x0F;
		__usb_speed = req->wIndex | 0xF0;
	}

	USBDCallbacks_RequestReceived(req);
	return 0;
}

u8 USB_Init(void)
{
	return !ezusb_init(ezusb_callback);
}



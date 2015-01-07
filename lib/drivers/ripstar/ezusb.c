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
#define EZUSB_PORT_CNT			4

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
	u32			status;
} __ids_t;

struct ezusb_port {
	__ids_t			id;
	void			*base;
	void			*iobase;

	int			burst;
	int			retries;
	char			wrbuf[BURST];
	callback_t		callback;
};

static __ids_t ids[] = {
	[0] = { 0, USBGenericRequest_IN, 1 << 0, 1 << 3 },
	[1] = { 1, USBGenericRequest_OUT, 1 << 1, 1 << 1 },
	[2] = { 6, USBGenericRequest_IN, 1 << 2, 1 << 4 },
	[3] = { 2, USBGenericRequest_IN, 1 << 2, 1 << 4 },
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
		stat &= port->id.status;
		if (port->id.type == USBGenericRequest_IN)
			stat = !stat;

		if (!stat)
			break;

		msleep(1);
	} while (--retries);

	return stat;
}

/* the waterlevel is fixed to 8 bytes by hardware */
static void ezusb_ep0_isr(void *arg)
{
	struct ezusb_port *port = (struct ezusb_port *)arg;
	static unsigned int q[2];

	if (port->callback) {

		q[0] = readl(port->iobase);
		q[1] = readl(port->iobase);

		port->callback(q);
	}

	/* clean the corresponding irq bit */
	setbits32(port->base + EZUSB_CIR, port->id.mask);
}

static int __read(struct ezusb_port *port, void *buf, int len)
{
	unsigned int *p;
	unsigned long flags = 0;

	len /= 4;	/* must be 4bytes aligned */

	p = (unsigned int *)buf;

	local_irq_save(flags);
	while (len) {
		u32 stat = readl(port->base + EZUSB_SR);
		if (stat & port->id.status)
			break;

		*p++ = readl(port->iobase);
		len--;
	}
	local_irq_restore(flags);
	return (char *)p - (char *)buf;
}

/* XXX: only used by EP0 */
static int ezusb_read(struct ezusb_port *port, void *buf, int len)
{
	char *p = (char *)buf;
	int ret = 0;

	len = roundup(len, 4);

	while (len > 0) {
		int l = __read(port, buf, len);
		if (l <= 0 || wait_port_not_busy(port))
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

	local_irq_save(flags);
	if (port->id.ep == 0) {

		cnt = len % port->burst;
		padding = port->burst - cnt;

		if (len + padding > io_limit)
			len = io_limit - padding;

		memset(port->wrbuf, 0, sizeof(port->wrbuf));
		port->wrbuf[0] = padding;
		if (flag & EZUSB_FLAGS_PAD)
			port->wrbuf[0] |= EZUSB_FLAGS_PAD;
		memcpy(port->wrbuf + padding, buf, cnt);

		p = (unsigned int *)port->wrbuf;
		cnt = port->burst / 4;
		while (cnt--)
			writel(*p++, port->iobase);
	}

	cnt = (len - (port->burst - padding)) / 4;
	p = (unsigned int *)((char *)buf + (port->burst - padding));

	while (cnt--)
		writel(*p++, port->iobase);

	/* start to xmit */
	writel(port->id.mask, port->base + EZUSB_CR);
	local_irq_restore(flags);

	return len;
}

/* used by ep0 & ep6 */
static ssize_t ezusb_write(struct ezusb_port *port,
		const void *buf, int len, int flags)
{
	char *p;
	int ret;

	p = (char *)buf;
	ret = 0;
	if (flags)
		flags = EZUSB_FLAGS_PAD;

	while (len > 0) {
		int l = __write(port, p, len, flags);
		if (l <= 0)
			break;

		len -= l;
		ret += l;
		p += l;
	}

	return ret;
}

#define BULK_BASE	0xff200200
#define BULK_STAT	0x0
#define BULK_BUSY	1 << 1

static struct completion	__wait;

static void ezusb_bulk_isr(void *arg)
{
	complete(&__wait);
}

static ssize_t bulk_in(struct ezusb_port *port, const void *buf, int len)
{
	int retries = 10;
	int rval;

	do {
		if (!(readl(port->base) & BULK_BUSY))
			break;

		msleep(1);

	} while (--retries);

	if (retries == 0) {
		pr_err("%s: busy\n", __func__);
		return -EBUSY;
	}

	INIT_COMPLETION(__wait);
	writel(0, port->base);
	writel(buf, port->base + 0x4);
	writel(len / 4, port->base + 0xC);
	writel(0x9c, port->base + 0x18);

	rval = wait_for_completion_timeout(&__wait, 5 * HZ);
	if (rval == 0) {
		pr_err("controller timed out\n");
		writel(0, port->base);
		writel(0, port->base + 0x4);
		writel(0, port->base + 0xC);
		return -EIO;
	}
	return len;
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
	for (i = 0; i < EZUSB_PORT_CNT - 1; i++, offset += 4) {
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
	request_irq(CONFIG_EZUSB_IRQ, ezusb_ep0_isr, port);
	setbits32(port->base + EZUSB_IER, port->id.mask);

	port = &ezusb_ports[3];	/* ep2 */
	port->base	= (void *)BULK_BASE;
	port->id	= ids[3];
	port->burst	= 512;
	port->retries	= 5;
	init_completion(&__wait);
	request_irq(73, ezusb_bulk_isr, port);

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
	struct ezusb_port *port;
	char status;
	int cnt;

	status = USBD_STATUS_SUCCESS;

	if (ep == 0)
		ep++;
	port = __to_ezusb_port(ep);
	if (ep == 2)
		cnt = bulk_in(port, buf, len);
	else
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

	cnt = ezusb_write(port, buf, len, 0);
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
	struct ezusb_port *port = __to_ezusb_port(0);

	ezusb_write(port, &cmd, sizeof(cmd), 1);
}

void USBD_Disconnect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};
	struct ezusb_port *port = __to_ezusb_port(0);

	__usb_state = USBD_STATE_DEFAULT;
	ezusb_write(port, &cmd, sizeof(cmd), 1);
}

u8 USBD_Stall(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;
	struct ezusb_port *port = __to_ezusb_port(0);

	if (!USBD_IsIdle(Endpoint))
		return USBD_STATUS_LOCKED;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = Endpoint;

	ezusb_write(port, buf, sizeof(buf), 1);
	return USBD_STATUS_SUCCESS;
}

u8 USBD_ACK(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;
	struct ezusb_port *port = __to_ezusb_port(0);

	if (!USBD_IsIdle(Endpoint))
		return USBD_STATUS_LOCKED;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ACK;
	cmd->data[0] = Endpoint;

	ezusb_write(port, buf, sizeof(buf), 1);
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataRead(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;
	struct ezusb_port *port = __to_ezusb_port(0);

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = Endpoint;

	ezusb_write(port, buf, sizeof(buf), 1);
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataWrite(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;
	struct ezusb_port *port = __to_ezusb_port(0);

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABROT_WRITE;
	cmd->data[0] = Endpoint;

	ezusb_write(port, buf, sizeof(buf), 1);
	return USBD_STATUS_SUCCESS;
}

extern void USBDCallbacks_RequestReceived(const USBGenericRequest *req);

/* called in irq, so could not have mutex */
static void ezusb_callback(void *arg)
{
	USBGenericRequest *req = (USBGenericRequest *)arg;

	if (USBGenericRequest_GetType(req) == USBGenericRequest_STANDARD
			&& req->bRequest == 0x01
			&& req->wValue == USB_CMD_HAND_SHAKE) {

		USB_CMD_t cmd;
		struct ezusb_port *port = __to_ezusb_port(0);

		__usb_state = USBD_STATE_CONFIGURED;
		__usb_speed = CY_U3P_SUPER_SPEED;

		cmd.len = 0;
		cmd.cmd = USB_CMD_HAND_SHAKE;

		ezusb_write(port, &cmd, sizeof(cmd), 1);
	} else {

		USBDCallbacks_RequestReceived(req);
	}
}

u8 USB_Init(void)
{
	return !ezusb_init(ezusb_callback);
}



/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Sat Jul 19 22:58:34 CST 2014
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

/* button to connect or disconnect usb */
static const Pin pin_usb = {
	1 << 14,
	DW_BASE_PIOB,
	DW_ID_PIOB,
	PIO_INPUT,
	PIO_DEFAULT
};

static void pin_stat_handle(const Pin *pin)
{
	/* connected -> high*/
	if (PIO_Get(pin)) {
		USB_connect();
	} else {
		TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
		USB_abort_write(USB_IN_EP);
		USB_disconnect();
	}
}

static void USB_stat_pin_isr(const Pin *pin)
{
	pin_stat_handle(pin);
}

static void USB_configure(void)
{
	PIO_Configure(&pin_usb, 1);
	PIO_ConfigureIt(&pin_usb, USB_stat_pin_isr);
	PIO_EnableIt(&pin_usb);

	pin_stat_handle(&pin_usb);
}

bool USB_init(void)
{
	OS_CPU_SR  cpu_sr;
	//	INT8U err;

	readFlag 		= False;	/* false until register read */

	OS_ENTER_CRITICAL();

	// USB initialization, Disable Pull-up
	TRACE_INFO("USB initialization\n\r");
	USBDriver_Initialize(&usbdDriver, &usbdDriverDescriptors, 0);
	USB_Init();

	// Enable Vbus detect, connect if needed (vbus detected)
	VBus_Configure();

	OS_EXIT_CRITICAL();  


	return true;
}

#include <fpga_usb.h>
#include <old_apis.h>

static u16 __usb_state;
static u16 __usb_speed;

volatile u32 TickAfterLastComm;

extern u8 USBGenericRequest_GetType(const USBGenericRequest *request)
{
	return (request->bmRequestType >> 5) & 0x3;
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

char USB_Read(u8 ep, void *data, u32 len, TransferCallback cb, void *arg)
{
	int cnt = fx3_read(1, data, len);
	if (cb)
		cb(arg, USB_STATUS_SUCCESS, cnt, len - cnt);
	return USB_STATUS_SUCCESS;
}

char USB_Write(u8 ep, const void *data, u32 l, TransferCallback cb, void *arg)
{
	u8 status = USB_STATUS_SUCCESS;
	int cnt = fx3_write(ep / 3, data, l);

	if (cnt != l)
		status = USB_STATUS_LOCKED;

	if (cb)
		cb(arg, status, cnt, l - cnt);

	return status;
}

u8 USB_GetState(void)
{
	return __usb_state;
}

u8 USB_IsHighSpeed(void)
{
	return __usb_speed == CY_U3P_SUPER_SPEED;
}

void USB_Connect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_CONNECT,
	};

	//fx3_enable();
	fx3_send_cmd(0, &cmd, sizeof(cmd));
}

void USB_Disconnect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};

	__usb_state = USB_STATE_DEFAULT;
	//fx3_disable();
	fx3_send_cmd(0, &cmd, sizeof(cmd));
}

u8 USB_Stall(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = Endpoint;

	fx3_send_cmd(0, cmd, sizeof(buf));
	return USB_STATUS_SUCCESS;
}

char USB_AbortDataRead(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = Endpoint;

	fx3_send_cmd(0, cmd, sizeof(buf));
	fx3_flush_fifo(Endpoint);
	return USB_STATUS_SUCCESS;
}

char USB_AbortDataWrite(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_WRITE;
	cmd->data[0] = Endpoint;

	fx3_send_cmd(0, cmd, sizeof(buf));
	fx3_flush_fifo(Endpoint / 3);	/* XXX */
	return USB_STATUS_SUCCESS;
}


extern void USBCallbacks_RequestReceived(const USBGenericRequest *req);

/* called in irq, so could not have mutex */
int fx3_request_cb(void *arg)
{
	USBGenericRequest *req = (USBGenericRequest *)arg;

	if (USBGenericRequest_GetType(req) == USBGenericRequest_STANDARD) {
		/* __usb_state = req->wValue; */
		__usb_state = USB_STATE_CONFIGURED;
		__usb_speed = req->wIndex;
	}

	USBCallbacks_RequestReceived(req);
	return 0;
}

u8 USB_Init(void)
{
	if (fx3_init())
		return false;

	fx3_register_request_cb(fx3_request_cb);
	return true;
}

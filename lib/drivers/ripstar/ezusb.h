/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Apr  2 20:35:18 CST 2014
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


#ifndef __CYCFX3_H__
#define __CYCFX3_H__

#include <asm/types.h>

typedef struct {
    unsigned char bmRequestType:8;
    unsigned char bRequest:8;
    unsigned short wValue:16;
    unsigned short wIndex:16;
    unsigned short wLength:16;

} USBGenericRequest;

typedef struct USB_CMD {
	u16	len;			/* data length */
	u16	cmd;
	u8	data[0];
} USB_CMD_t;


typedef enum CyU3PUSBSpeed {
    CY_U3P_NOT_CONNECTED = 0x00,	/* not connected. */
    CY_U3P_FULL_SPEED,			/* full speed */
    CY_U3P_HIGH_SPEED,			/* High speed. USB2.0 */
    CY_U3P_SUPER_SPEED			/* Super speed. USB3.0 */

} CyU3PUSBSpeed_t;

typedef enum USBCmd {
	USB_CMD_GET_SPEED = 0x01,
	USB_CMD_CONNECT,
	USB_CMD_DISCONNECT,
	USB_CMD_ABORT_READ,
	USB_CMD_ABROT_WRITE,
	USB_CMD_STALL,
	USB_CMD_ACK,
	USB_CMD_RESERVED = 0x0F,

	USB_CMD_HAND_SHAKE = 0x10,
} USBCmd_t;


#define USBD_STATE_SUSPENDED            0
#define USBD_STATE_ATTACHED             1
#define USBD_STATE_POWERED              2
#define USBD_STATE_DEFAULT              3
#define USBD_STATE_ADDRESS              4
#define USBD_STATE_CONFIGURED           5

#define USBGenericRequest_STANDARD	0	/* standard */
#define USBGenericRequest_CLASS		1	/* class-specific */
#define USBGenericRequest_VENDOR	2	/* vendor-specific */


#define USBD_STATUS_SUCCESS		0
#define USBD_STATUS_LOCKED		1	/* device busy */
#define USBD_STATUS_ABORTED		2	/* Operation aborted */
#define USBD_STATUS_RESET		3


#define USBGenericRequest_OUT		0	/* host to device */
#define USBGenericRequest_IN		1	/* device to host */


typedef void (*TransferCallback)(void *, u8, u32, u32);

u8 USBGenericRequest_GetType(const USBGenericRequest *request);
u8 USBGenericRequest_GetRequest(const USBGenericRequest *request);
u16 USBGenericRequest_GetValue(const USBGenericRequest *request);
u16 USBGenericRequest_GetIndex(const USBGenericRequest *request);
u16 USBGenericRequest_GetLength(const USBGenericRequest *request);
u8 USBGenericRequest_GetDirection(const USBGenericRequest *request);

char USBD_Read(u8 ep, void *data, u32 len, TransferCallback cb, void *arg);
char USBD_Write(u8 ep, const void *data, u32 len,
		TransferCallback callback, void *arg);

u8 USBD_GetState(void);
u8 USBD_IsHighSpeed(void);
void USBD_Connect(void);
void USBD_Disconnect(void);
u8 USBD_Stall(u8 Endpoint);
char USBD_AbortDataRead(u8 Endpoint);
char USBD_AbortDataWrite(u8 Endpoint);

u8 USB_Init(void);

#endif

#ifndef __OLD_APIS_H__
#define __OLD_APIS_H__

#include <asm/types.h>
#include <stdbool.h>
#include <stdio.h>

#define BOOL	bool
#define False	false
#define FALSE	false
#define True	true
#define TRUE	true

#define U32	u32
#define U8	u8
#define UBYTE	u8


#define TRACE_INFO(fmt, args...) pr_info(fmt, ##args)

u8 IIC_WriteEEPRom(u16 addr, u8 *buf, u8 *len);
inline u8 IIC_WriteEEPRom_Ex(u16 addr, u8 *buf, u16 *len);
u8 IIC_ReadEEPRom(u16 addr, u8 *buf, u8 *len);
inline u8 IIC_ReadEEPRom_Ex(u16 addr, u8 *buf, u16 *len);

u8 IIC_Write_Max11614(u8 reg, u8 data, u8 pending);
float IIC_Read_Max11614(u8 Flag);

u8 IIC_WriteRegs(u8 reg, u8 *buf, u8 cnt);
inline u8 IIC_WriteReg(u8 reg, u8 data);
u8 IIC_ReadReg(u8 reg, u8 *buf, u8 cnt);

u8 IIC_Write_LCD_32(u8 reg, u8 *buf, u8 cnt);
u8 IIC_Read_LCD(u8 reg, u8 *buf, u8 cnt);

void IIC_Init(void);


void UART_Init(u8 flag);
u8 UART_SendCMD(u8 num, u8 *data);

/* ------------------------------------------------------------------------
 * APIS for USB
 * ------------------------------------------------------------------------
 */
#include <cycfx3.h>
typedef void (*TransferCallback)(void *, u8, u32, u32);

typedef struct {
	unsigned char	bmRequestType:8;
	unsigned char	bRequest:8;
	unsigned short	wValue:16;
	unsigned short	wIndex:16;
	unsigned short	wLength:16;
} USBGenericRequest;

typedef struct USB_CMD {
	u16	len;			/* data length */
	u16	cmd;
	u8	data[0];
} USB_CMD_t;

typedef enum CyU3PUSBSpeed {
	CY_U3P_NOT_CONNECTED = 0x00,	/* not connected. */
	CY_U3P_FULL_SPEED,		/* full speed */
	CY_U3P_HIGH_SPEED,		/* High speed. USB2.0 */
	CY_U3P_SUPER_SPEED		/* Super speed. USB3.0 */
} CyU3PUSBSpeed_t;

typedef enum USBCmd {
	USB_CMD_GET_SPEED = 0x01,
	USB_CMD_CONNECT,
	USB_CMD_DISCONNECT,
	USB_CMD_ABORT_READ,
	USB_CMD_ABORT_WRITE,
	USB_CMD_STALL
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


typedef void (*TransferCallback)(void *arg,
		u8 status, u32 transferred, u32 remaining);

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

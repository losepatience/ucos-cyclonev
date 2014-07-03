#include <stdio.h>
#include <string.h>
#include <i2c.h>
#include <at24.h>
#include <flash.h>
#include <old_apis.h>
#include <fpga_uart.h>
#include <errno.h>
#include <malloc.h>
#include <delay.h>

#ifndef INCOMPATIBLE_WITH_ATMEL

static struct i2c_adapter *adap_zlg7290;
static struct i2c_adapter *adap_lcd;
static struct i2c_adapter *adap_max11614;

/* ------------------------------------------------------------------------
 * APIS for 24c32
 * ------------------------------------------------------------------------
 */
#ifdef WITH_EEPROM
u8 IIC_WriteEEPRom(u16 addr, u8 *buf, u8 *len)
{
	ssize_t l;

	l = at24_write((const char *)buf, addr, *len);
	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_WriteEEPRom_Ex(u16 addr, u8 *buf, u16 *len)
{
	ssize_t l;

	l = at24_write((const char *)buf, (loff_t)addr, (size_t)(*len));
	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_ReadEEPRom(u16 addr, u8 *buf, u8 *len)
{
	ssize_t l;

	l = at24_read((char *)buf, (loff_t)addr, (size_t)(*len));

	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_ReadEEPRom_Ex(u16 addr, u8 *buf, u16 *len)
{
	ssize_t l;

	l = at24_read((char *)buf, (loff_t)addr, (size_t)(*len));

	if (l <= 0)
		return false;

	*len = l;
	return true;
}
#else
u8 IIC_WriteEEPRom(u16 addr, u8 *buf, u8 *len)
{
	ssize_t l = *len;

	l = flash_saveenv((char *)buf, addr, l);
	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_WriteEEPRom_Ex(u16 addr, u8 *buf, u16 *len)
{
	ssize_t l = *len;

	l = flash_saveenv((char *)buf, addr, l);
	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_ReadEEPRom(u16 addr, u8 *buf, u8 *len)
{
	ssize_t l = *len;

	l = flash_readenv((char *)buf, addr, l);
	if (l <= 0)
		return false;

	*len = l;
	return true;
}

u8 IIC_ReadEEPRom_Ex(u16 addr, u8 *buf, u16 *len)
{
	ssize_t l = *len;

	l = flash_readenv((char *)buf, addr, l);
	if (l <= 0)
		return false;

	*len = l;
	return true;
}
#endif

#ifndef IIC_MAX11614_ADDR
# define IIC_MAX11614_ADDR	0x33
#endif

#ifndef IIC_KB_ADDR
#define IIC_KB_ADDR		0x38
#endif
/* ------------------------------------------------------------------------
 * APIS for max11614
 * ------------------------------------------------------------------------
 */
#include <ssl.h>
unsigned char CRC8_Table(unsigned char *p, char len)
{
	return w1_crc8(p, (int)len);
}

/*
 * write 1 byte to configure max11614.
 * reg and pending is unecessary,
 * they are here to be compitable with old apis.
 */
u8 IIC_Write_Max11614(u8 reg, u8 data, u8 pending)
{
	struct i2c_msg msg;

	msg.addr = IIC_MAX11614_ADDR;
	msg.flags = 0;
	msg.buf = &data;
	msg.len = 1;

	if (i2c_transfer(adap_max11614, &msg, 1) != 1)
		return false;

	return true;
}

#define swap(a, b) do {		\
	typeof((a)) tmp = (a);	\
	*(&(a)) = (b);		\
	*(&(b)) = tmp;		\
} while (0)

float IIC_Read_Max11614(u8 Flag)
{
	u8 buf[24];
	float vol[12];
	int i, j;

	struct i2c_msg msg = {
		.addr = IIC_MAX11614_ADDR,
		.flags = I2C_M_RD,
		.len = 24,
	};

	Flag = IIC_Write_Max11614(1, 1 << 5 | 1 | Flag << 1, false);
	if (Flag == false) {
		pr_err("max11614: write cmd error\n");
		return 0.0;
	}

	msg.buf = buf;
	if (i2c_transfer(adap_max11614, &msg, 1) != 1) {
		pr_err("max11614: read error\n");
		return 0.0;
	}

	for (i = 0; i < 6; i++)
		vol[i] = ((float)((buf[i * 2] & 0x0F) * 256	\
					+ buf[i * 2 + 1])) / 4096 * 5.0f;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 5; j++)
			if ((i < 2 && vol[j] > vol[j + 1])
					|| (i >= 2 && vol[j] < vol[j + 1]))
				swap(vol[j], vol[j + 1]);

	return (vol[0] + vol[1]) / 2;
}


/* ------------------------------------------------------------------------
 * APIS for led
 * ------------------------------------------------------------------------
 */

#include <dwgpio.h>

static u8 writebuf[256];
extern void RESET_KB(void);

u8 IIC_WriteRegs(u8 reg, u8 *buf, u8 cnt)
{
	struct i2c_msg msg;

	msg.addr = IIC_KB_ADDR;
	msg.flags = 0;

	msg.buf = writebuf;
	msg.buf[0] = reg;
	memcpy(&msg.buf[1], buf, cnt);
	msg.len = 1 + cnt;

	if (i2c_transfer(adap_zlg7290, &msg, 1) != 1)
		return false;

	return true;
}

inline u8 IIC_WriteReg(u8 reg, u8 data)
{
	return IIC_WriteRegs(reg, &data, 1);
}

u8 IIC_ReadReg(u8 reg, u8 *buf, u8 cnt)
{
	struct i2c_msg msgs[2];

	msgs[0].addr = IIC_KB_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf = &reg;
	msgs[0].len = 1;

	msgs[1].addr = IIC_KB_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = buf;
	msgs[1].len = cnt;

	if (i2c_transfer(adap_zlg7290, msgs, 2) != 2) {
		RESET_KB();
		return false;
	}

	return true;
}


/* ------------------------------------------------------------------------
 * APIS for lcd
 * ------------------------------------------------------------------------
 */
u8 IIC_Write_LCD_32(u8 reg, u8 *buf, u8 cnt)
{
	struct i2c_msg msg;

	msg.addr = 0x51;
	msg.flags = 0;

	msg.buf = writebuf;
	msg.buf[0] = reg;
	memcpy(&msg.buf[1], buf, cnt);
	msg.len = 1 + cnt;

	if (i2c_transfer(adap_lcd, &msg, 1) != 1) {
		RESET_KB();
		return false;
	}

	return true;
}

u8 IIC_Read_LCD(u8 reg, u8 *buf, u8 cnt)
{
	struct i2c_msg msgs[2];
	msgs[0].addr = 0x51;
	msgs[0].flags = 0;
	msgs[0].buf = &reg;
	msgs[0].len = 1;

	msgs[1].addr = 0x51;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = buf;
	msgs[1].len = cnt;

	if (i2c_transfer(adap_lcd, msgs, 2) != 2) {
		RESET_KB();
		return false;
	}

	return true;
}


/* ------------------------------------------------------------------------
 *  Setup i2c bus
 * ------------------------------------------------------------------------
 */

extern int dwi2c_add_numbered_adapter(int num);

void IIC_Init(void)
{
	if (dwi2c_add_numbered_adapter(0)) {
		pr_err("%s: fails to add i2c adapter 0\n", __func__);
		return;
	}

	if (dwi2c_add_numbered_adapter(1)) {
		pr_err("%s: fails to add i2c adapter 1\n", __func__);
		return;
	}

	adap_zlg7290 = i2c_get_adapter(0);
	if (adap_zlg7290 == NULL)
		return;

	adap_lcd = adap_zlg7290;

	adap_max11614 = i2c_get_adapter(1);
	if (adap_max11614 == NULL)
		return;

	at24_init();
}


/* ------------------------------------------------------------------------
 * APIS for UART
 * ------------------------------------------------------------------------
 */
#include <fpga_uart.h>
#include <ssl.h>
#include <fifo.h>

#include <miscellance.h>
#ifndef MAX_SAPACKET_LEN
#  define MAX_SAPACKET_LEN	(32)
#endif

#define UART_CHNUM	3

/*
 * SA protocol:
 *
 * SA is short for simple-abc. this name comes from its simpleness
 *  1. packet is the xfer basic unit. there are 2 kinds of packet.
 *     normal packet and ACK packet, as show in the table below.
 *     .-------.-------.-------.---------------.
 *     |   0   |   1   |   2   |   3   | 4-31  |
 *     .-------+-------+-------+---------------.
 *     |   st  |toggle |chksum |length | data  |
 *     .-------+-------+-------+---------------.
 *     |  0xaa | 0/1   | xxx   |[5,32] | xxxxx |
 *     ^-------^-------^-------^-------^-------^ ------> normal packet
 *     .-------.-------.
 *     |   0   |   1   |
 *     .-------+-------.
 *     | 0xaa  | 0x55  |
 *     .-------+-------^ ------> ACK packet
 *
 *  2. TX: send 1 Pakeck --> wait for ACK
 *  3. RX: recv 1 Pakeck --> send 1 ACK
 */

struct SA_channel {
	u8		rxbuf[512];
	struct fifo	*rxfifo;
	struct fifo	*txfifo;

#define CHKMOD_ANY	0
#define CHKMOD_SUM	1
#define CHKMOD_CRC	2
	int		chkmod;
	int		toggle;
	bool		txACK;

	struct mutex	mutex;
};

static struct SA_channel *SA_chans;

static u8 SA_gen_chksum(u8 *data, int len, int flag)
{
	u8 chksum;

	if (flag == CHKMOD_SUM) {
		int i;
		chksum = 0;

		for (i = 0; i <	len; i++)
			chksum += data[i];
	} else {
		chksum = crc8(w1_crc8_table, data, len, 0);
	}

	return chksum;
}

static bool SA_is_valid_packet(int num, void *data, int len, u8 chksum)
{
	u8 tmp;

	*((u8 *)data + 2) = 0;	/* XXX ignore the chksum byte */

	if (SA_chans[num].chkmod == CHKMOD_ANY) {
		tmp = SA_gen_chksum(data, len, CHKMOD_SUM);
		if (tmp != chksum)
			tmp = SA_gen_chksum(data, len, CHKMOD_CRC);

		return chksum == tmp;
	}

	return chksum == SA_gen_chksum(data, len, SA_chans[num].chkmod);
}

/* @return: 1:at cmd head, 0:cmd head not found */
static int SA_find_packet_st(int num)
{
	char ch;

	while (fpga_uart_read(num, &ch, 1) == 1)
		if (ch == 0xaa)
			return 0;

	return -EAGAIN;
}

static int SA_recv_packet(int num)
{
	static char cmd[MAX_SAPACKET_LEN] = { 0xaa };
	int rval;
	int len;

	mutex_lock(&SA_chans[num].mutex);

	if (fifo_cached(SA_chans[num].rxfifo) < MAX_SAPACKET_LEN) {
		pr_err("%s: rxfifo is full!", __func__);
		rval = -EBUSY;
		goto __out;
	}

	rval = SA_find_packet_st(num);
	if (rval) {
		pr_err("%s: no packet st has been found!", __func__);
		goto __out;
	}

	len = fpga_uart_read(num, &cmd[1], 1);
	if (len != 1) {
		rval = -EAGAIN;
		goto __out;
	} else if (cmd[1] == 0x55) {
		SA_chans[num].txACK = true;
		goto __out;
	}

	len = fpga_uart_read(num, &cmd[2], 2);
	if (len != 2 || !SA_is_valid_packet(num, NULL, cmd[3], 0)) {
		rval = -EAGAIN;
		goto __out;
	}

	len = fpga_uart_read(num, &cmd[4], cmd[3]);
	if (len != cmd[3] || !SA_is_valid_packet(num, cmd, len, cmd[2])) {
		rval = -EAGAIN;
	} else {
		char buf[2] = { 0xaa, 0x55 };
		cmd[3] -= 3;
		fifo_in(SA_chans[num].rxfifo, &cmd[3], cmd[3]);
		fpga_uart_write(num, buf, 2);	/* ACK */
	}

__out:
	mutex_unlock(&SA_chans[num].mutex);
	return rval;
}

static bool SA_check_ACK(int num)
{
	int retry = 5;
	bool ret = false;

	while (!SA_chans[num].txACK) {
		SA_recv_packet(num);
		if (--retry == 0)
			break;
		else
			msleep(1);
	}

	mutex_lock(&SA_chans[num].mutex);
	if (SA_chans[num].txACK) {
		SA_chans[num].txACK = false;
		ret = true;
	}
	mutex_unlock(&SA_chans[num].mutex);

	return ret;
}

void UART_Init(u8 flag)
{
	int chan = UART_MOTION_CHANNEL; 
	char *buf;
	int i;

	SA_chans = calloc(UART_CHNUM, sizeof(struct SA_channel));
	if (SA_chans == NULL) {
		pr_err("%s: no memory!", __func__);
		return;
	}

	buf = calloc(1, 512);
	if (buf == NULL) {
		pr_err("%s: no memory!", __func__);
		free(SA_chans);
		return;
	}

	for (i = 0; i < UART_CHNUM; i++) {
		mutex_init(&SA_chans[i].mutex);
		SA_chans[i].chkmod = 0;
		SA_chans[i].toggle = 0;
		SA_chans[i].txACK = false;
		SA_chans[i].rxfifo = fifo_init(SA_chans[i].rxbuf, 1, 512);
	}

	SA_chans[chan].txfifo = fifo_init(buf, 1, sizeof(buf));
	fpga_uart_init(0);
}

void UART_SetCheckModel(u8 num, u8 model)
{
	mutex_lock(&SA_chans[num].mutex);
	SA_chans[num].chkmod = model;
	mutex_unlock(&SA_chans[num].mutex);
}

/* virtual uart device. data received would be stored in txfifo */
u8 UART_MotionGetCMD(u8 *data)
{
	int num = UART_MOTION_CHANNEL;

	mutex_lock(&SA_chans[num].mutex);

	if (fifo_cached(SA_chans[num].txfifo) < 1) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_out(SA_chans[num].txfifo, data, 1);
	fifo_out(SA_chans[num].txfifo, data + 1, *data - 1);

	mutex_unlock(&SA_chans[num].mutex);
	return true;
}

u8 UART_SendMotionCMD(u8 *data)
{
	u8 err;
	u8 len;
	int num = UART_MOTION_CHANNEL;

	len = data[0];

	if (len < 2)
		return false;

	mutex_lock(&SA_chans[num].mutex);
	if (fifo_unused(SA_chans[num].txfifo) < len) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_in(SA_chans[num].txfifo, data, len);
	OSFlagPost(mix_FLAG_GRP, MOTION_SEND_CMD, OS_FLAG_SET, &err);

	mutex_unlock(&SA_chans[num].mutex);
	return true;
}

u8 UART_GetCMD(u8 num, u8 *data)
{
	SA_recv_packet(num);

	mutex_lock(&SA_chans[num].mutex);

	if (fifo_cached(SA_chans[num].rxfifo) < 1) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_out(SA_chans[num].rxfifo, data, 1);
	fifo_out(SA_chans[num].rxfifo, data + 1, data[0] - 1);

	mutex_unlock(&SA_chans[num].mutex);
	return true;
}

u8 UART_SendCMD(u8 num, u8 *data)
{
	static u8 buf[MAX_SAPACKET_LEN];
	u8 l = data[0];

	if (l < 2)
		return false;

	/* motion channel */
	if (UART_MOTION_CHANNEL == num)
		return UART_SendMotionCMD(data);

	mutex_lock(&SA_chans[num].mutex);
	memcpy(&buf[3], data, l);

	l += 3;

	buf[0] = 0xAA;
	buf[1] = SA_chans[num].toggle;
	buf[2] = 0;
	buf[3] = l;

	buf[2] = SA_gen_chksum(buf, l, SA_chans[num].chkmod);

	SA_chans[num].toggle = !SA_chans[num].toggle;

	if (fpga_uart_write(num, buf, l) != l) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	mutex_unlock(&SA_chans[num].mutex);
	return SA_check_ACK(num);
}

u8 UART_MotionReportCMD(u8 *data)
{
	u8 err;
	u8 len;
	int num = UART_MOTION_CHANNEL;

	len = data[0];

	if (len < 2 || len > 28)
		return false;

	mutex_lock(&SA_chans[num].mutex);
	if (fifo_unused(SA_chans[num].rxfifo) < len) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_in(SA_chans[num].rxfifo, data, len);
	OSFlagPost(mix_FLAG_GRP, MOTION_REPORT_CMD, OS_FLAG_SET, &err);
	mutex_unlock(&SA_chans[num].mutex);

	return true;
}


/* ------------------------------------------------------------------------
 * APIS for USB
 * ------------------------------------------------------------------------
 */
#include <fpga_usb.h>
static u16 __usb_state;
static u16 __usb_speed;

volatile u32 TickAfterLastComm;

extern u8 USBGenericRequest_GetType(const USBGenericRequest *request)
{
	return (request->bmRequestType >> 5) & 0x3;
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
	int cnt = usb_read(1, data, len);
	if (cb)
		cb(arg, USBD_STATUS_SUCCESS, cnt, len - cnt);
	return USBD_STATUS_SUCCESS;
}

char USBD_Write(u8 ep, const void *data, u32 l, TransferCallback cb, void *arg)
{
	u8 status = USBD_STATUS_SUCCESS;
	int cnt = usb_write(ep / 3, data, l);

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

	usb_enable();
	usb_send_cmd(0, &cmd, sizeof(cmd));
}

void USBD_Disconnect(void)
{
	USB_CMD_t cmd = {
		.len = 0,
		.cmd = USB_CMD_DISCONNECT,
	};

	__usb_state = USBD_STATE_DEFAULT;
	usb_disable();
	usb_send_cmd(0, &cmd, sizeof(cmd));
}

u8 USBD_Stall(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_STALL;
	cmd->data[0] = Endpoint;

	usb_send_cmd(0, cmd, sizeof(buf));
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataRead(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_READ;
	cmd->data[0] = Endpoint;

	usb_send_cmd(0, cmd, sizeof(buf));
	usb_flush_fifo(Endpoint);
	return USBD_STATUS_SUCCESS;
}

char USBD_AbortDataWrite(u8 Endpoint)
{
	char buf[sizeof(USB_CMD_t) + 1];
	USB_CMD_t *cmd = (USB_CMD_t *)buf;

	cmd->len = 1;
	cmd->cmd = USB_CMD_ABORT_WRITE;
	cmd->data[0] = Endpoint;

	usb_send_cmd(0, cmd, sizeof(buf));
	usb_flush_fifo(Endpoint / 3);	/* XXX */
	return USBD_STATUS_SUCCESS;
}


extern void USBDCallbacks_RequestReceived(const USBGenericRequest *req);

/* called in irq, so could not have mutex */
int usb_rx_callback(void *arg)
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
	if (usb_init())
		return false;

	usb_register_callback(usb_rx_callback);
	return true;
}




/* ------------------------------------------------------------------------
 * APIS for HugeBuf
 * ------------------------------------------------------------------------
 */
#include <fifo.h>
#include <job.h>
#include <status.h>

#define __report	status_ReportStatus
#define HEAD_SIZE	1024	/* heads is 1024bytes aligned */

static struct fifo *dma_fifo;
static size_t curband_size;

static spinlock_t dma_lock;

void HugeBuf_Reset(void)
{
	fifo_reset(dma_fifo);
}

u8 HugeBuf_Flush(ssize_t size)
{
	int cnt;
	bool ret = true;
	unsigned long flags = 0;

	spin_lock_irqsave(&dma_lock, flags);

	cnt = fifo_size2cnt(dma_fifo, size);

	if (cnt > fifo_cached(dma_fifo))
		ret = false;
	else
		fifo_out(dma_fifo, NULL, cnt);

	spin_unlock_irqrestore(&dma_lock, flags);
	return ret;
}

u8 HugeBuf_GetInfHead(DataHeaderType *header)
{
	void *addr;
	unsigned long flags = 0;

	addr = fifo_oaddr(dma_fifo);
	memcpy(header, addr, sizeof(DataHeaderType));

	if (memcmp(&header->header_flag, "BYHX", sizeof(u32))) {
		__report(STATUS_FTA_INTERNAL_WRONGHEADER, STATUS_SET);
		while (1)
			OSTimeDly(1);

		return false;
	}

	if (header->header_size != HEAD_SIZE) {
		__report(STATUS_FTA_INTERNAL_WRONGHEADERSIZE, STATUS_SET);
		return false;
	}

	spin_lock_irqsave(&dma_lock, flags);

	/* waits for more data */
	if (HugeBuf_Flush(header->header_size) == false) {
		spin_unlock_irqrestore(&dma_lock, flags);
		return false;
	}

	curband_size = header->data_bytescnt;
	spin_unlock_irqrestore(&dma_lock, flags);
	return true;
}

void HugeBuf_FlushCurBand(void)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&dma_lock, flags);
	fifo_out(dma_fifo, NULL, fifo_size2cnt(dma_fifo, curband_size));
	spin_unlock_irqrestore(&dma_lock, flags);
}

ssize_t HugeBuf_GetSize(void)
{
	ssize_t size;
	unsigned long flags = 0;

	spin_lock_irqsave(&dma_lock, flags);
	size = fifo_size2cnt(dma_fifo, fifo_cached(dma_fifo));
	spin_unlock_irqrestore(&dma_lock, flags);
	return size;
}

u8 *HugeBuf_GetRecAddr(void)
{
	return (u8 *)fifo_iaddr(dma_fifo);
}

void HugeBuf_PutSize(size_t size)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&dma_lock, flags);
	fifo_in(dma_fifo, NULL, fifo_size2cnt(dma_fifo, size));
	spin_unlock_irqrestore(&dma_lock, flags);
}

void HugBuf_InitReceiveSize(void)
{
	dma_fifo = fifo_init((void *)0x2000000, HEAD_SIZE, 1024 * 1024);
}

static void *dma_fill_fifo(struct fifo *fifo, size_t size)
{
	int cnt;
	void *addr;
	unsigned long flags = 0;

	cnt = fifo_size2cnt(fifo, size);

	spin_lock_irqsave(&dma_lock, flags);

	addr = fifo_iaddr(fifo);
	if (dma_receive(addr, size) != size) {
		addr = NULL;
		goto out;
	}
	fifo_in(fifo, NULL, cnt);

out:
	spin_unlock_irqrestore(&dma_lock, flags);
	return addr;
}

void HugeBuf_StartOneReceive(void)
{
	int cnt;
	void *addr;
	DataHeaderType *head;

	addr = dma_fill_fifo(dma_fifo, HEAD_SIZE);
	if (addr == NULL)
		return;

	head = (DataHeaderType *)addr;

	if (memcmp(&head->header_flag, "BYHX", sizeof(u32)))
		return;	/* XXX */

	cnt = fifo_size2cnt(dma_fifo, head->data_bytescnt);
	if (fifo_unused(dma_fifo) < cnt) {
		g_USBTransStoped = true;
		return;
	}

	dma_fill_fifo(dma_fifo, head->data_bytescnt);
}

void HugeBuf_CompleteOneReceive(void)
{
	return;
}

u16 *HugeBuf_GetCodeAddr(ssize_t len)
{
	if (fifo_cached(dma_fifo) < len)
		return 0;

	return (u16 *)fifo_oaddr(dma_fifo);
}

#if defined(HEAD_EPSON_GEN5)
u8 FPGA_SendData(u8 first, u32 PreFireNum)
#else
u8 FPGA_SendData(u8 first)
#endif
{
	int cnt;
	void *addr;
	int size = 19;
	unsigned long flags = 0;

	spin_lock_irqsave(&dma_lock, flags);

	addr = fifo_oaddr(dma_fifo);
	if (dma_send(addr, size) != size) {
		spin_unlock_irqrestore(&dma_lock, flags);
		return false;
	}

	cnt = fifo_size2cnt(dma_fifo, size);
	fifo_out(dma_fifo, NULL, cnt);

	spin_unlock_irqrestore(&dma_lock, flags);
	return true;
}


/* call this after OS initialized and this is only for OS0 */
int device_init(void)
{
	int rval;

	PIO_InitializeInterrupts(0);

	/* init i2c adapter and devices on i2c bus, like at24 */
	IIC_Init();

	rval = flash_init();
	if (rval) {
		pr_err("%s: failed to init flash\n", __func__);
		return rval;
	}

	return 0;
}

#endif /* INCOMPATIBLE_WITH_ATMEL */

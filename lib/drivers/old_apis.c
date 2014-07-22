#include <stdio.h>
#include <string.h>
#include <i2c.h>
#include <at24.h>
#include <flash.h>
#include <old_apis.h>
#include <cycserial.h>
#include <errno.h>
#include <malloc.h>
#include <delay.h>

#ifndef INCOMPATIBLE_WITH_ATMEL

static struct i2c_adapter *adap_zlg7290;
static struct i2c_adapter *adap_lcd;
static struct i2c_adapter *adap_max11614;

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

/* ------------------------------------------------------------------------
 * APIS for 24c32
 * ------------------------------------------------------------------------
 */
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
#if 1
#include <dwgpio.h>

static u8 writebuf[256];
extern void RESET_KB(void)
{
	return;
}

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
#endif

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

	adap_zlg7290 = i2c_get_adapter(0);
	adap_lcd = adap_zlg7290;

	if (at24_init())
		return;

	if (dwi2c_add_numbered_adapter(1)) {
		pr_err("%s: fails to add i2c adapter 1\n", __func__);
		return;
	}

	adap_max11614 = i2c_get_adapter(1);
}



#if 0
/* ------------------------------------------------------------------------
 * APIS for USB
 * ------------------------------------------------------------------------
 */




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
		return (u16 *)0;

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


#endif /* INCOMPATIBLE_WITH_ATMEL */
#endif /* INCOMPATIBLE_WITH_ATMEL */

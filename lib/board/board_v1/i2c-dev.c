#include <stdio.h>
#include <string.h>
#include <i2c.h>
#include <at24.h>
#include <old_apis.h>
#include <errno.h>
#include <dwgpio.h>
#include <flash.h>

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

	rval = flash_init(0, 0, 50000000, SPI_CPOL | SPI_CPHA);
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
#include <crypto.h>
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

	if (dwi2c_add_numbered_adapter(1)) {
		pr_err("%s: fails to add i2c adapter 1\n", __func__);
		return;
	}

	if (at24_init())
		return;

	adap_zlg7290 = i2c_get_adapter(1);
	adap_lcd = adap_zlg7290;
	adap_max11614 = adap_lcd;
}


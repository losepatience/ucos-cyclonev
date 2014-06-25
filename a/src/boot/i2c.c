#include <asm/arch/regs.h>
#include <asm/io.h>
#include "delay.h"
#include "i2c.h"

/* Absolutely safe for status update at 100 kHz I2C: */
#define I2C_WAIT			200
#define I2C_TIMEOUT			1000

#define I2C_IP_CLK			48000000
// #define I2C_INTERNAL_SAMPLING_CLK	12000000
#define I2C_INTERNAL_SAMPLING_CLK	24000000

/* I2C Status Register (I2C_STAT): */
#define I2C_STAT_XDR	(1 << 14)	/* TX Buffer draining */
#define I2C_STAT_RDR	(1 << 13)	/* RX Buffer draining */
#define I2C_STAT_BB	(1 << 12)	/* Bus busy */
#define I2C_STAT_ROVR	(1 << 11)	/* Receive overrun */
#define I2C_STAT_XUDF	(1 << 10)	/* Transmit underflow */
#define I2C_STAT_AAS	(1 << 9)	/* Address as slave */
#define I2C_STAT_AD0	(1 << 8)	/* Address zero */
#define I2C_STAT_XRDY	(1 << 4)	/* Transmit data ready */
#define I2C_STAT_RRDY	(1 << 3)	/* Receive data ready */
#define I2C_STAT_ARDY	(1 << 2)	/* Register access ready */
#define I2C_STAT_NACK	(1 << 1)	/* No ack interrupt enable */
#define I2C_STAT_AL	(1 << 0)	/* Arbitration lost int ena */

/* I2C Configuration Register (I2C_CON): */
#define I2C_CON_EN	(1 << 15)	/* I2C module enable */
#define I2C_CON_BE	(1 << 14)	/* Big endian mode */
#define I2C_CON_STB	(1 << 11)	/* Start byte mode(master) */
#define I2C_CON_MST	(1 << 10)	/* Master/slave mode */
#define I2C_CON_TRX	(1 << 9)	/* TX/RX mode(master only) */
#define I2C_CON_XA	(1 << 8)	/* Expand address */
#define I2C_CON_RM	(1 << 2)	/* Repeat mode(master only) */
#define I2C_CON_STP	(1 << 1)	/* Stop cond(master only) */
#define I2C_CON_STT	(1 << 0)	/* Start condition(master) */


#define I2C_SYSS_RDONE	(1 << 0)	/* reset monitoring */

struct __priv {
	void		*base;
	u16		flags;
};

static inline void __write_reg(struct __priv *priv, u16 val, u32 reg)
{
	__raw_writew(val, priv->base + reg);
}

static inline u16 __read_reg(struct __priv *priv, u32 reg)
{
	return __raw_readw(priv->base + reg);
}

/* waiting on bus busy */
static int __wait_for_bb(struct __priv *priv)
{
	int timeout = 1000;

	while (__read_reg(priv, I2CREG_IRQSTATUS_RAW) & I2C_STAT_BB) {

		udelay(1000);

		if (timeout-- == 0)
			return 1;
	}

	return 0;
}

static void setup_clk(struct __priv *priv, u32 sclk, u32 iclk, u32 speed)
{
	u32 psc, div, scll, sclh;

	/* calculate prescalar value */
	psc = (sclk / iclk) - 1;

	div = iclk / speed;
	div /= 2;

	scll = div - 7;
	sclh = div - 5;

	__write_reg(priv, psc, I2CREG_PSC);
	__write_reg(priv, scll, I2CREG_SCLL);
	__write_reg(priv, sclh, I2CREG_SCLH);
}

static void __i2c_reset(struct __priv *priv)
{
	u16 sysc;
	u16 reg;

	sysc = __read_reg(priv, I2CREG_SYSC);

	/* disable i2c controller before soft reset */
	reg = __read_reg(priv, I2CREG_CON);
	reg &= ~I2C_CON_EN;
	__write_reg(priv, reg, I2CREG_CON);

	/* soft reset */
	__write_reg(priv, 0x2, I2CREG_SYSC);

	/* For some reason we need to set the EN bit before the
	 * reset done bit gets set. */
	__write_reg(priv, I2C_CON_EN, I2CREG_CON);

	while (!(__read_reg(priv, I2CREG_SYSS) & I2C_SYSS_RDONE))
		udelay(1000);

	/* SYSC register is cleared by the reset; rewrite it */
	__write_reg(priv, sysc, I2CREG_SYSC);
}

static void omap_i2c_init(struct __priv *priv)
{
	__write_reg(priv, 0, I2CREG_CON);

	/* configure i2c bus speed to 100khz */
	setup_clk(priv, I2C_IP_CLK, I2C_INTERNAL_SAMPLING_CLK, 100000);

	/* take the i2c module out of reset: */
	__write_reg(priv, I2C_CON_EN, I2CREG_CON);
}

static s16 check_error(struct __priv *priv)
{
	u16 flags = I2C_STAT_ARDY | I2C_STAT_NACK | I2C_STAT_AL;
	int timeout = I2C_TIMEOUT;
	s16 stat;

	if (priv->flags & I2C_M_RD)
		flags |= I2C_STAT_ROVR | I2C_STAT_RRDY;
	else
		flags |= I2C_STAT_XUDF | I2C_STAT_XRDY;

	do {
		udelay(I2C_WAIT);
		stat = __read_reg(priv, I2CREG_IRQSTATUS_RAW);

	} while (!(stat & flags) && timeout--);


	if (priv->flags & I2C_M_RD)
		stat &= ~(I2C_STAT_XDR | I2C_STAT_XRDY);
	else
		stat &= ~(I2C_STAT_RDR | I2C_STAT_RRDY);

	/* We have an error */
	if (stat & (I2C_STAT_AL | I2C_STAT_ROVR | I2C_STAT_XUDF)) {
		__i2c_reset(priv);
		omap_i2c_init(priv);
		goto out;
	}

	if (stat  & I2C_STAT_NACK) {
		u16 reg = __read_reg(priv, I2CREG_CON);
		reg |= I2C_CON_STP;
		__write_reg(priv, reg, I2CREG_CON);
		goto out;
	}

	if (timeout <= 0) {
		/*
		 * If stat is still 0 here, probably the bus pads
		 * have not been configured for I2C, and/or pull-ups
		 * are missing.
		 */
		goto out;
	}

	return stat;

out:
	/* clean the STAT register */
	__write_reg(priv, 0xFFFF, I2CREG_STAT);
	return -1;
}

static int __xfer_msg(struct __priv *priv, struct i2c_msg *msg)
{
	u16 reg;
	int idx;

	__write_reg(priv, msg->addr, I2CREG_SA);
	__write_reg(priv, msg->len, I2CREG_CNT);

	/* clear the FIFO buffers */
	reg = __read_reg(priv, I2CREG_BUF);
	reg |= (1 << 14) | (1 << 6);
	__write_reg(priv, reg, I2CREG_BUF);

	reg = I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_STP;

	if (msg->flags & I2C_M_RD)
		priv->flags = I2C_M_RD;
	else
		reg |= I2C_CON_TRX;

	__write_reg(priv, reg, I2CREG_CON);

	for (idx = 0; idx < msg->len; idx++) {

		s16 stat = check_error(priv);
		if (stat < 0)
			return stat;

		if (stat & I2C_STAT_RRDY) {
			msg->buf[idx] = readb(priv->base + I2CREG_DATA);
			__write_reg(priv, I2C_STAT_RRDY, I2CREG_STAT);
		} else if (stat & I2C_STAT_XRDY) {
			writeb(msg->buf[idx], priv->base + I2CREG_DATA);
			__write_reg(priv, I2C_STAT_XRDY, I2CREG_STAT);
		}
	}

	return 0;
}

static int omap_i2c_xfer(struct i2c_adapter *adap,
			struct i2c_msg msgs[], int num)
{
	struct __priv *priv = (struct __priv *)adap->priv;
	int rval;
	int i;

	rval = __wait_for_bb(priv);
	if (rval < 0)
		return rval;

	for (i = 0; i < num; i++) {
		rval = __xfer_msg(priv, &msgs[i]);
		if (rval != 0)
			break;
	}

	__wait_for_bb(priv);

	return rval == 0 ? num : rval;
}


/* -------------------------------------------------------------
 * API
 * -------------------------------------------------------------
 */
static struct __priv omap_priv[I2C_BUS_MAX] = {
	[0] = {
		.base = (void *)I2C0_BASE,
	},
	[1] = {
		.base = (void *)I2C1_BASE,
	},

};

static struct i2c_adapter __i2c_adap[I2C_BUS_MAX] = {
	[0] = {
		.retries	= 2,
		.timeout	= 0,
		.priv		= &omap_priv[0],
		.xfer		= omap_i2c_xfer,
	},

	[1] = {
		.retries	= 2,
		.timeout	= 0,
		.priv		= &omap_priv[1],
		.xfer		= omap_i2c_xfer,
	},
};

struct i2c_adapter *i2c_get_adapter(int busnum)
{
	if (busnum > I2C_BUS_MAX - 1)
		return (void *)0;
	return &__i2c_adap[busnum];
}

inline void i2c_init(struct i2c_adapter *adap)
{
	return omap_i2c_init((struct __priv *)adap->priv);
}

int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	int rval;
	int retries = adap->retries;

	while (retries--) {
		rval = adap->xfer(adap, msgs, num);
		if (rval > 0)
			break;
	}

	return rval;
}


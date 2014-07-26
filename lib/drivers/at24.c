/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
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

/* Is there someone who would help me add multi-chip support? */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/types.h>
#include <platform.h>

#include <i2c.h>
#include <config.h>

struct __at24 {
	struct mutex		mutex;
	unsigned short		addr;
	unsigned short		flags;
	unsigned short		page_size;	/* for writes */
	int			bus;		/* i2c bus num */


	struct i2c_adapter	*adap;
	unsigned int		write_max;
	unsigned char		*writebuf;
};

static struct __at24 __24lc32 = {
	.addr		= IIC_EEPROM_ADDR,
	.flags		= AT24_FLAG_ADDR16,
	.page_size	= E2PROM_PAGESIZE,
	.bus		= 0
};

static unsigned io_limit = 128;

static inline struct __at24 *__get_at24(void)
{
	return &__24lc32;
}

static ssize_t __read(struct __at24 *at24, char *buf, loff_t offs, size_t cnt)
{
	struct i2c_msg msg[2];
	unsigned char msgbuf[2];
	int timeout = 10;
	int i = 0;

	memset(msg, 0, sizeof(msg));

	if (cnt > io_limit)
		cnt = io_limit;

	if (at24->flags & AT24_FLAG_ADDR16)
		msgbuf[i++] = offs >> 8;

	msgbuf[i++] = offs;

	msg[0].addr = at24->addr;
	msg[0].buf = msgbuf;
	msg[0].len = i;

	msg[1].addr = at24->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = (unsigned char *)buf;
	msg[1].len = cnt;

	do {
		if (2 == i2c_transfer(at24->adap, msg, 2))
			return cnt;

		msleep(1);
	} while (--timeout);

	return -ETIMEDOUT;
}

static ssize_t __write(struct __at24 *at24,
		const char *buf, loff_t offs, size_t cnt)
{
	struct i2c_msg msg;
	unsigned next_page;
	int timeout = 10;
	int i = 0;

	/* write_max is at most a page */
	if (cnt > at24->write_max)
		cnt = at24->write_max;

	/* Never roll over backwards, to the start of this page */
	next_page = roundup(offs + 1, at24->page_size);
	if (offs + cnt > next_page)
		cnt = next_page - offs;

	msg.addr = at24->addr;
	msg.flags = 0;

	msg.buf = at24->writebuf;
	if (at24->flags & AT24_FLAG_ADDR16)
		msg.buf[i++] = offs >> 8;

	msg.buf[i++] = offs;
	memcpy(&msg.buf[i], buf, cnt);
	msg.len = i + cnt;

	do {
		int tmp = i2c_transfer(at24->adap, &msg, 1);
		msleep(5);

		if (tmp == 1)
			return cnt;
	} while (--timeout);

	return -ETIMEDOUT;
}

ssize_t at24_read(char *buf, loff_t off, size_t cnt)
{
	struct __at24 *at24 = __get_at24();
	ssize_t rval = 0;

	mutex_lock(&at24->mutex);
	while (cnt) {
		ssize_t status = __read(at24, buf, off, cnt);
		if (status <= 0) {
			if (rval == 0)
				rval = status;
			break;
		}

		buf += status;
		off += status;
		cnt -= status;
		rval += status;
	}
	mutex_unlock(&at24->mutex);

	return rval;
}

ssize_t at24_write(const char *buf, loff_t off, size_t cnt)
{
	struct __at24 *at24 = __get_at24();
	ssize_t rval = 0;

	mutex_lock(&at24->mutex);
	while (cnt) {
		ssize_t status = __write(at24, buf, off, cnt);
		if (status <= 0) {
			if (rval == 0)
				rval = status;
			break;
		}

		buf += status;
		off += status;
		cnt -= status;
		rval += status;
	}
	mutex_unlock(&at24->mutex);

	return rval;
}

int at24_init(void)
{
	struct __at24 *at24 = __get_at24();

	mutex_init(&at24->mutex);

	at24->write_max = at24->page_size;
	if (at24->write_max > io_limit)
		at24->write_max = io_limit;

	at24->writebuf = malloc(io_limit + 2);
	if (at24->writebuf == NULL) {
		pr_err("%s: no memory\n", __func__);
		return -ENOMEM;
	}

	at24->adap = i2c_get_adapter(at24->bus);
	if (at24->adap == NULL) {
		pr_err("%s: adapter0 has not registered\n", __func__);
		free(at24->writebuf);
		return -ENODEV;
	}

	return 0;
}

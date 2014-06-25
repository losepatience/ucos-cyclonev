/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 25 17:03:16 CST 2014
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

#ifndef __BOOT_I2C_H__
#define __BOOT_I2C_H__

struct i2c_msg {
	u16		addr;	/* slave address */
	u16		flags;
#define I2C_M_RD	0x0001	/* read data, from slave to master */
	u16		len;	/* msg length */
	u8		*buf;	/* pointer to msg data */
};

struct i2c_adapter {
	int			retries;
	int			timeout;
	void			*priv;

	int (*xfer)(struct i2c_adapter *adap, struct i2c_msg msgs[], int num);
};

struct i2c_adapter *i2c_get_adapter(int busnum);
void i2c_init(struct i2c_adapter *adap);
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num);
#endif

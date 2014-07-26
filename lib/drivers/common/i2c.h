#ifndef __I2C_H__
#define __I2C_H__

/*
 * I2C adapter interface (master only).
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
#include <list.h>
#include <platform.h>

struct i2c_msg {
	unsigned short		addr;	/* slave address			*/
	unsigned short		flags;
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_STOP		0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
	unsigned short		len;	/* msg length */
	unsigned char		*buf;	/* pointer to msg data */
};

struct i2c_adapter {
	struct list_head	list;
	struct mutex		lock;
	int			busnum;
	int			retries;
	int			timeout;

	int 			(*xfer)(struct i2c_adapter *adap,
					struct i2c_msg msgs[], int num);
	void			*priv;
};

struct i2c_adapter *i2c_get_adapter(int busnum);
int i2c_register_adapter(struct i2c_adapter *adap, int busnum);
int i2c_is_numbered_adapter_registered(int busnum);
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num);

#endif


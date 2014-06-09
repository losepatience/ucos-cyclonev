/* @ . @ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 *
 * All rights reserved. No Part of this file may be reproduced,
 * stored in a retrieval system, or transmitted, in any form,
 * or by any means, electronic, mechanical, photocopying, recording,
 * or otherwise, without the prior consent of BYHX, Inc.
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <platform.h>

#include <gpio.h>
#include <ssl.h>
#include <w1_ds28e01.h>
#include <old_apis.h>

static unsigned char w1_read_bit(struct w1_master *master)
{
	int val;

	/*
	 *  \       /``````````````````\
	 * tF\_tRL_/....................\__
	 *
	 * the master generates a start signal by pull the bus down for tRL.
	 * then release the bus, and sample the bus within tMSR
	 */

	master->write_bit(master->data, 0);
	udelay(6);
	master->write_bit(master->data, 1);
	udelay(9);

	val = master->read_bit(master->data);

	udelay(55);

	return val & 0x1;
}

static void w1_write_bit(struct w1_master *master, u8 bit)
{
	/*
	 *  \        /``````````````````\
	 * tF\_tW1L_/                    \__
	 *  \                      /````\
	 * tF\________tW0L________/      \__
	 *
	 * XXX: the master pulls the bus down for tW1L or tW0L
	 */

	if (bit) {
		master->write_bit(master->data, 0);
		udelay(6);
		master->write_bit(master->data, 1);
		udelay(64);
	} else {
		master->write_bit(master->data, 0);
		udelay(60);
		master->write_bit(master->data, 1);
		udelay(10);
	}
}

int w1_reset_bus(struct w1_master *master)
{
	int val;

	/*
	 *  \               /```\XXXXXXXX/`````\
	 * tF\____tRSTL____/tPDH \XXXXXX/ tREC  \__
	 *                         tMSR
	 */
	master->write_bit(master->data, 0);
	udelay(480);

	/*
	 * after tRSTL, Vpup and slave control the bus.
	 * slave waits for tPDH and then transmits a Presence
	 * Pulse by pulling the line low for tPDL.
	 */
	master->write_bit(master->data, 1);
	udelay(70);

	val = master->read_bit(master->data) & 0x1;
	udelay(410);

	return val;
}


static u8 w1_read_8(struct w1_master *master)
{
	int i;
	u8 res = 0;

	if (master->read_byte)
		return master->read_byte(master->data);

	for (i = 0; i < 8; ++i)
		res |= (w1_read_bit(master) << i);

	return res;
}

static void w1_write_8(struct w1_master *master, u8 val)
{
	int i;

	if (master->write_byte)
		return master->write_byte(master->data, val);

	for (i = 0; i < 8; ++i)
		w1_write_bit(master, (val >> i) & 0x1);
}

static ssize_t w1_read_block(struct w1_master *master, u8 *buf, size_t len)
{
	int i;

	if (master->read_block)
		return master->read_block(master, buf, len);

	for (i = 0; i < len; ++i)
		buf[i] = w1_read_8(master);

	return len;
}

static ssize_t w1_write_block(struct w1_master *master,
				const u8 *buf, size_t len)
{
	int i;

	if (master->write_block)
		return master->write_block(master, buf, len);

	for (i = 0; i < len; ++i)
		w1_write_8(master, buf[i]);

	return len;
}

static inline int w1_reset_select_slave(struct w1_master *master)
{
	if (w1_reset_bus(master))
		return -EIO;

	w1_write_8(master, 0xcc);
	return 0;
}

/* ------------------------------------------------------------ */
/* gpio w1 adapter */
/* ------------------------------------------------------------ */
static inline void w1_gpio_write_bit(void *data, u8 val)
{
	unsigned pin = *(unsigned *)data;

	if (val)
		gpio_direction_input(pin);
	else
		gpio_direction_output(pin, 0);
}

static inline int w1_gpio_read_bit(void *data)
{
	return gpio_get_value(*(unsigned *)data);
}

int w1_master_init(struct w1_master *master)
{
	static unsigned pin = 32;	/* FIXME */
	master->data = &pin;
	master->read_bit = w1_gpio_read_bit;
	master->write_bit = w1_gpio_write_bit;
	return 0;
}

/* ---------------------------------------------------------------- */
/* things about w1 family 2f */
/* ---------------------------------------------------------------- */

static int w1_f2f_read_romid(struct w1_master *master, u8 *romid)
{
	if (w1_reset_bus(master))
		return -ENODEV;

	w1_write_8(master, W1_F2F_READ_ROMID);
	w1_read_block(master, romid, 8);

	return w1_crc8(romid, 8) ? -EAGAIN : 0;
}

static int __w1_f2f_read(struct w1_master *master, u8 *buf, u8 offs, int cnt)
{
	unsigned next_page;
	unsigned pad_num = 0;
	struct w1_ds28e01 *ds28e01 = master->slave;

	if (offs / ds28e01->sub_page_size)
		pad_num = offs - rounddown(offs, ds28e01->sub_page_size);

	offs	-= pad_num;
	cnt	+= pad_num;

	next_page = roundup(offs + 1, ds28e01->page_size);
	if (offs + cnt > next_page)
		cnt = next_page - offs;

	ds28e01->wr_buf[0] = W1_F2F_READ_EEPROM;
	ds28e01->wr_buf[1] = offs;
	ds28e01->wr_buf[2] = 0;

	/* read twice to compare the result */
	if (w1_reset_select_slave(master))
		return -EIO;

	w1_write_block(master, ds28e01->wr_buf, 3);
	w1_read_block(master, &ds28e01->wr_buf[3], cnt);

	/* the second time */
	if (w1_reset_select_slave(master))
		return -EIO;

	w1_write_block(master, ds28e01->wr_buf, 3);
	w1_read_block(master, ds28e01->rd_buf, cnt);

	if (memcmp(ds28e01->rd_buf, &ds28e01->wr_buf[3], cnt))
		return -EAGAIN;

	memcpy(buf, ds28e01->rd_buf + pad_num, cnt - pad_num);
	return cnt - pad_num;
}

static int w1_f2f_read(struct w1_master *master, u8 *buf, u8 offs, int cnt)
{
	int rval = 0;

	while (cnt) {
		int status = __w1_f2f_read(master, buf, offs, cnt);
		if (status < 0) {
			if (!rval)
				rval = status;
			break;
		}
		cnt -= status;
		offs += status;
		buf += status;
		rval += status;
	}
	return rval;
}

/* write 8 bytes to scratchpad */
static int w1_f2f_write_scratchpad(struct w1_master *master,
				   const u8 *buf, u8 offs)
{
	struct w1_ds28e01 *ds28e01 = master->slave;
	unsigned cnt = ds28e01->sub_page_size;

	/* write 2bytes addr & 8bytes data, then read back crc16 */
	if (w1_reset_select_slave(master))
		return -EIO;

	ds28e01->wr_buf[0] = W1_F2F_WRITE_SCRATCH;
	ds28e01->wr_buf[1] = offs;
	ds28e01->wr_buf[2] = 0;

	memcpy(&ds28e01->wr_buf[3], buf, cnt);

	w1_write_block(master, ds28e01->wr_buf, 3 + cnt);
	w1_read_block(master, &ds28e01->wr_buf[3 + cnt], 2);

	if ((crc16(0, ds28e01->wr_buf, 3 + cnt + 2) != 0xb001))
		return -EAGAIN;

	/* read back addr E/S and data to compare agaist */
	if (w1_reset_select_slave(master))
		return -EIO;

	w1_write_8(master, W1_F2F_READ_SCRATCH);
	w1_read_block(master, ds28e01->rd_buf, 3 + cnt);

	if ((ds28e01->rd_buf[0] != ds28e01->wr_buf[1])
			|| (ds28e01->rd_buf[1] != ds28e01->wr_buf[2])
			|| (ds28e01->rd_buf[2] != 0x5f)
			|| (memcmp(buf, &ds28e01->rd_buf[3], cnt) != 0))
		return -EAGAIN;

	return 0;
}

static int w1_f2f_write(struct w1_master *master, const u8 *buf, u8 offs)
{
	struct w1_ds28e01 *ds28e01 = master->slave;
	u8 *src = ds28e01->sha1_src;
	u32 sha1[5];
	int rval;

	if (offs < 0x80) {
		if (w1_f2f_read(master, &src[4], offs, 28) != 28)
			return -EAGAIN;
	} else if (offs == 0x88) {
		memcpy(&src[4], src, 4);	/* first 4 bytes of secret */
		memcpy(&src[8], &src[48], 4);	/* last 4 bytes of secret */
		if (w1_f2f_read(master, &src[12], offs, 20) != 20)
			return -EAGAIN;
	} else {
		return -EINVAL;
	}

	memcpy(&src[32], buf, 8);
	src[40] = offs >> 5;
	memset(&src[52], 0xff, 3);

	__sha1(sha1, src);

	if (w1_f2f_write_scratchpad(master, buf, offs))
		return -EAGAIN;

	if (w1_reset_select_slave(master))
		return -EIO;

	ds28e01->wr_buf[0] = W1_F2F_COPY_SCRATCH;
	ds28e01->wr_buf[1] = offs;
	ds28e01->wr_buf[2] = 0;
	ds28e01->wr_buf[3] = 0x5f;	/* E/S */
	w1_write_block(master, ds28e01->wr_buf, 4);
	mdelay(3);		/* for ds28e01 to generate SHA1 */

	w1_write_block(master, (u8 *)sha1, 20);
	mdelay(12);		/* for flushing scratchpad */

	rval = w1_read_8(master);

	return (rval ==  0xAA || rval ==  0x55) ? 0 : -EAGAIN;
}

/* loads the 64bits secret into secret region(offs) */
static int w1_f2f_load_secret(struct w1_master *master, const u8 *buf, u8 offs)
{
	struct w1_ds28e01 *ds28e01 = master->slave;
	int rval;

	rval = w1_f2f_write_scratchpad(master, buf, offs);
	if (rval)
		return rval;

	if (w1_reset_select_slave(master))
		return -EIO;

	ds28e01->wr_buf[0] = W1_F2F_LOAD_SECRET;
	ds28e01->wr_buf[1] = offs;
	ds28e01->wr_buf[2] = 0;
	ds28e01->wr_buf[3] = 0x5f;
	w1_write_block(master, ds28e01->wr_buf, 4);

	/* it is unecessary, here for logic */
	/* w1_write_bit(master, 1); */
	mdelay(10);	/* delay 10ms for copying scratchpad */

	rval = w1_read_8(master);

	return (rval ==  0xAA || rval ==  0x55) ? 0 : -EAGAIN;
}

static int w1_f2f_write_challenge(struct w1_master *master, int offs)
{
	struct w1_ds28e01 *ds28e01 = master->slave;

	if (w1_reset_select_slave(master))
		return -EIO;

	ds28e01->wr_buf[0] = W1_F2F_WRITE_CHALLENGE;
	ds28e01->wr_buf[1] = offs & 0xff;
	ds28e01->wr_buf[2] = 0;
	ds28e01->wr_buf[3] = 0xff;
	ds28e01->wr_buf[4] = 0xff;
	//memcpy(ds28e01->wr_buf, ds28e01->challenge, 5);	/* 5bytes */
	ds28e01->wr_buf[10] = 0xff;

	w1_write_block(master, ds28e01->wr_buf, 11);
	w1_read_block(master, &ds28e01->wr_buf[11], 2);

	return 0;
}

static int __authenticate(struct w1_master *master, u8 offs)
{
	struct w1_ds28e01 *ds28e01 = master->slave;
	u8 *src = ds28e01->sha1_src;
	u32 sha1[5];
	u8 buf[3 + 35];
	int rval;

	rval = w1_f2f_write_challenge(master, offs);
	if (rval)
		return rval;

	if (w1_reset_select_slave(master))
		return -EIO;

	buf[0] = W1_F2F_READ_AUTH_PAGE;
	buf[1] = offs;
	buf[2] = 0;
	w1_write_block(master, buf, 3);
	w1_read_block(master, &buf[3], 35);

	if (crc16(0, buf, 35 + 3) != 0xb001)
		return -EAGAIN;

	mdelay(3);      /* for SHA-1 algorithm */
	w1_read_block(master, buf, 22);
	if (crc16(0, buf, 22) != 0xb001)
		return -EAGAIN;

	memcpy(&src[4], &buf[3], 32);
	//memcpy(&src[36], Challenge, 2);
	memset(&src[38], 0xFF, 2);
	src[40] = offs >> 5 | 0x40;
	if (memcmp(buf, sha1, 20))
		return -EAGAIN;

	return 0;
}

int w1_f2f_init(void)
{
	u8 secret[8] = { 0x55, 0x4C, 0x50, 0x41, 0x58, 0x48, 0x59, 0x42 };
	struct w1_ds28e01 *ds28e01;
	struct w1_master *master;

	ds28e01 = (struct w1_ds28e01 *)calloc(1, sizeof(struct w1_ds28e01));
	if (!ds28e01)
		return -ENOMEM;

	master = (struct w1_master *)calloc(1, sizeof(struct w1_master));
	if (!master)
		return -ENOMEM;

	w1_master_init(master);

	master->slave = ds28e01;
	ds28e01->master = master;

	ds28e01->page_size = 32;
	ds28e01->sub_page_size = 8;

	/* fill the sha1 source buffer */
	memcpy(ds28e01->sha1_src, secret, 4);

	/* the last byte will be over-write soon */
	if (w1_f2f_read_romid(master, &ds28e01->sha1_src[41]))
		return -EAGAIN;

	memcpy(&ds28e01->sha1_src[48], &secret[4], 4);
	ds28e01->sha1_src[55] = 0x80;
	ds28e01->sha1_src[62] = 0x01;
	ds28e01->sha1_src[63] = 0xB8;

	return 0;
}

/* --------------------------------------------------------------------------
 * Old APIs
 * -------------------------------------------------------------------------- */

struct w1_master *__ds28e01;
#ifndef INCOMPATIBLE_WITH_ATMEL
u32 owBoardID;
u16 owManufacturerID;

u8 InitSecurityChip(void)
{
	struct w1_master *master = __ds28e01;
	u8 buf[32];

	/* read the whole content of page 1*/
	if (w1_f2f_read(master, buf, 0x88, 0x18) != 0x18)
		return False;

	if (buf[0] != 0x55 && buf[0] != 0xAA) {

		//if (w1_f2f_load_secret(master, ds28e01->secret, 0x80))
			return False;

		buf[0] = 0x55;
		if (w1_f2f_write(master, buf, 0x88))
			return False;
	}

	if (__authenticate(master, 0))
		return False;

	return True;
}

u8 WriteBoardAndManufacturerID(u32 Boardid, u16 Manufacturerid)
{
	struct w1_master *master = __ds28e01;
	u8 wrbuf[8];
	u8 rdbuf[32] = {0};

	/* read the whole content of page 1*/
	if (w1_f2f_read(master, rdbuf, 0x20, 0x20) != 0x20)
		return False;

	memcpy(wrbuf, rdbuf, 8);
	*(u32 *)wrbuf = Boardid;
	*(u16 *)(wrbuf + sizeof(u32)) = Manufacturerid;

	if (w1_f2f_write(master, wrbuf, 0x20))
		return False;

	return True;
}

u8 Init_OneWire(void)
{
	struct w1_master *master = __ds28e01;
	u8 buf[32];

#ifndef BYHX_WTITE_BOARD_TOOL

	/* read the whole content of page 1 */
	if (w1_f2f_read(master, buf, 0x20, 0x20) != 0x20)
		return False;

	owBoardID = *(u32 *)buf;
	owManufacturerID = *(u16 *)(buf + sizeof(u32));
#endif

	return True;
}

#endif /* INCOMPATIBLE_WITH_ATMEL */


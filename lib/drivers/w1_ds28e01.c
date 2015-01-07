/* @ . @ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 *
 * All rights reserved. No Part of this file may be reproduced,
 * stored in a retrieval system, or transmitted, in any form,
 * or by any means, electronic, mechanical, photocopying, recording,
 * or otherwise, without the prior consent of BYHX, Inc.
 */

#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <crypto.h>
#include <w1.h>
#include <gpio.h>
#include <stddef.h>

#define W1_F2F_READ			0xF0
#define W1_F2F_READ_AUTHPAGE		0xA5
#define W1_F2F_LOAD_SECRET		0x5A

#define W1_F2F_READ_SCRATCHPAD		0xAA
#define W1_F2F_WRITE_SCRATCHPAD		0x0F
#define W1_F2F_COPY_SCRATCHPAD		0x55

static int w1_gpio_pin = 70;

static inline unsigned char w1_gpio_read_bit(void)
{
	int pin = w1_gpio_pin;
	return gpio_get_value(pin);
}

static void w1_gpio_write_bit(u8 val)
{
	int pin = w1_gpio_pin;

	if (val)
		gpio_direction_input(pin);
	else
		gpio_direction_output(pin, 0);
}

static struct w1_master w1_gpio_master = {
	.read_bit = w1_gpio_read_bit,
	.write_bit = w1_gpio_write_bit
};

/* ---------------------------------------------------------------- */
/* things about w1 family 2f */
/* ---------------------------------------------------------------- */
static struct w1_f2f_pdata {
	u8	sha1_msg[64];
} __pdata;

static struct w1_slave *__w1_f2f_slave;

static inline struct w1_slave *w1_f2f_get_slave(void)
{
	return __w1_f2f_slave;
}

static int w1_f2f_read(struct w1_slave *sl, u8 *buf, int offs, int len)
{
	u8 cmd[3];
	u8 tmp[32];
	unsigned pad, next_page;

	if (w1_reset_slave(sl))
		return -EIO;

	pad = offs % 8;
	offs -=  pad;

	next_page = roundup(offs + 1, 32);
	if (offs + len > next_page)
		len = next_page - offs;

	cmd[0] = W1_F2F_READ;
	cmd[1] = offs;
	cmd[2] = 0;

	w1_write_block(sl, cmd, 3);
	w1_read_block(sl, buf, len);

	if (w1_reset_slave(sl))
		return -EIO;

	w1_write_block(sl, cmd, 3);
	w1_read_block(sl, tmp, len);

	if (memcmp(buf, tmp, len))
		return -EAGAIN;

	if (pad > 0) {
		len -= pad;
		memcpy(buf, tmp + pad, len);
	}

	return len;
}

/* write must be performed on 8-byte boundaries */
static int w1_f2f_write_scratchpad(struct w1_slave *sl,
		const u8 *buf, u8 offs, int check)
{
	/* 3-bytes cmd, 8-bytes data and 2-bytes crc16 */
	u8 cmd[3 + 8 + 2];

	/* 3-bytes addr, 8-bytes data and 2-bytes crc16 */
	u8 tmp[3 + 8 + 2];

	if (w1_reset_slave(sl))
		return -EIO;

	cmd[0] = W1_F2F_WRITE_SCRATCHPAD;
	cmd[1] = offs;
	cmd[2] = 0;

	memcpy(cmd + 3, buf, 8);

	w1_write_block(sl, cmd, 3 + 8);
	w1_read_block(sl, cmd + 3 + 8, 2);

	if ((crc16(0, cmd, 13) != 0xb001))
		return -EAGAIN;

	if (check) {
		/* read back addr, E/S and data to compare agaist */
		if (w1_reset_slave(sl))
			return -EIO;

		w1_write_8(sl, W1_F2F_READ_SCRATCHPAD);
		w1_read_block(sl, tmp, 13);

		/* compare addr and data agaist what we have writen */
		if (memcmp(tmp, cmd + 1, 2) || memcmp(tmp + 3, buf, 8))
			return -EAGAIN;

		/* check E/S */
		if (tmp[2] != 0x5F)
			return -EAGAIN;
	}

	return 0;
}

/* loads the 64bits secret into secret region(offs) */
static int w1_f2f_load_secret(struct w1_slave *sl, const u8 *secret, u8 offs)
{
	u8 cmd[4];
	u8 *msg;

	if (w1_f2f_write_scratchpad(sl, secret, offs, 1))
		return -EAGAIN;

	if (w1_reset_slave(sl))
		return -EIO;

	cmd[0] = W1_F2F_LOAD_SECRET;
	cmd[1] = offs;
	cmd[2] = 0;
	cmd[3] = 0x5F;
	w1_write_block(sl, cmd, 4);

	mdelay(10);			/* delay 10ms for copying scratchpad */
	if (w1_read_8(sl) != 0xAA)	/* 0xAA indicates success */
		return -EAGAIN;

	/* update secret */
	msg = ((struct w1_f2f_pdata *)sl->priv)->sha1_msg;
	memcpy(&msg[0], secret, 4);
	memcpy(&msg[48], &secret[4], 4);

	return 0;
}


/*
 * register page(0x88-0x9F):
 *  special functional registers, user data and manufactory data.
 *  once programmed to 0xaa or 0x55, most of them would be WP
 */
static int w1_f2f_write8(struct w1_slave *sl,
		const u8 *buf, u8 offs, u8 *page)
{
	u8 *msg;
	u8 cmd[4];
	u8 sha1[20];

	if (w1_f2f_write_scratchpad(sl, buf, offs, 1))
		return -EAGAIN;

	if (w1_reset_slave(sl))
		return -EIO;

	cmd[0] = W1_F2F_COPY_SCRATCHPAD;
	cmd[1] = offs;
	cmd[2] = 0;
	cmd[3] = 0x5F;			/* E/S */
	w1_write_block(sl, cmd, 4);
	mdelay(2);			/* for ds28e01 to generate SHA1 */

	msg = ((struct w1_f2f_pdata *)sl->priv)->sha1_msg;
	memcpy(&msg[4], page, 28);
	memcpy(&msg[32], buf, 8);
	msg[40] = (offs >> 5) & 0x7;
	memset(&msg[52], 0xff, 3);
	w1_sha1(sha1, msg);

	w1_write_block(sl, (u8 *)sha1, 20);
	mdelay(13);			/* for flushing scratchpad */

	gpio_direction_input(70);
	if (w1_read_8(sl) != 0xAA)	/* 0xAA indicates success */
		return -EAGAIN;

	return 0;
}

static int w1_f2f_auth(struct w1_slave *sl, u8 *chlge, int page)
{
	u8 offs;
	u8 *msg;
	u8 sha1[20];
	u8 cmd[3 + 35];
	struct w1_f2f_pdata *pdata;

	pdata = (struct w1_f2f_pdata *)sl->priv;
	msg = pdata->sha1_msg;
	offs = page << 5;

	/* write challenge to scratch-pad */
	if (w1_f2f_write_scratchpad(sl, chlge, offs, 0))
		return -EAGAIN;

	if (w1_reset_slave(sl))
		return -EIO;

	cmd[0] = W1_F2F_READ_AUTHPAGE;
	cmd[1] = offs;
	cmd[2] = 0;
	w1_write_block(sl, cmd, 3);

	/* read back 32-bytes data, 2-bytes CRC and 0xFF */
	w1_read_block(sl, &cmd[3], 35);

	if (crc16(0, cmd, 3 + 35) != 0xb001)
		return -EAGAIN;

	/* copy out the page data */
	memcpy(&msg[4], &cmd[3], 32);

	/* for SHA-1 algorithm */
	mdelay(3);

	w1_read_block(sl, cmd, 22);
	if (crc16(0, cmd, 22) != 0xb001)
		return -EAGAIN;

	/* take a warning */
	chlge += 2;
	memcpy(&msg[36], chlge, 2);
	memset(&msg[38], 0xFF, 2);
	msg[40] = page | 0x40;
	memcpy(&msg[52], &chlge[2], 3);
	w1_sha1(sha1, msg);
	
	if (memcmp(cmd, sha1, 20))
		return -EAGAIN;

	return 0;
}

struct w1_slave *w1_f2f_alloc(struct w1_master *master, u8 *secret)
{
	u8 *msg;
	struct w1_slave *sl;

	sl = (struct w1_slave *)malloc(sizeof(struct w1_slave));
	if (!sl)
		return NULL;
	memset(sl, 0, sizeof(struct w1_slave));
	sl->master = master;

	if (w1_read_rom(sl, sl->rom, 8)) {
		free(sl);
		return NULL;
	}

	msg = __pdata.sha1_msg;
	sl->priv = &__pdata;

	/* TODO */
	memcpy(&msg[0], secret, 4);
	memcpy(&msg[41], sl->rom, 7);
	memcpy(&msg[48], &secret[4], 4);
	msg[55] = 0x80;
	memset(&msg[56], 0, 6);
	msg[62] = 0x01;
	msg[63] = 0xB8;

	__w1_f2f_slave = sl;

	return sl;
}

/* --------------------------------------------------------------------------
 * Old APIs
 * -------------------------------------------------------------------------- */
unsigned int owBoardID;
unsigned short owManufacturerID;
static u8 ripstar_secret[8] = {
	0x55, 0x4C, 0x50, 0x41, 0x58, 0x48, 0x59, 0x42
};

u8 InitSecurityChip(void)
{
	u8 buf[8];
	u8 page[32];
	struct w1_slave *sl = w1_f2f_get_slave();

	u8 chlge[8] = {
		0xFF, 0xFF, 0x12, 0x24, 0x4D, 0x2A, 0x73, 0xFF
	};

	if (w1_f2f_read(sl, page + 8, 0x88, 24) != 24)
		return false;

	if (page[8] != 0x55 && page[8] != 0xAA) {

		if (w1_f2f_load_secret(sl, ripstar_secret, 0x80))
			return false;

		memcpy(page, ripstar_secret, 8);
		memcpy(buf, page + 8, 8);
		buf[0] = 0x55;
		if (w1_f2f_write8(sl, buf, 0x88, page))
			return false;
	} else if (w1_f2f_auth(sl, chlge, 1)) {
		return false;
	}

	return true;
}

u8 WriteBoardAndManufacturerID(unsigned int bid, unsigned short mid)
{
	struct w1_slave *sl = w1_f2f_get_slave();
	u8 tmp[32];
	u8 buf[8];

	if (w1_f2f_read(sl, tmp, 0x20, 32) != 32)
		return false;

	memcpy(buf, &bid, sizeof(bid));
	memcpy(buf + sizeof(bid), &mid, sizeof(mid));
	buf[6] = 0;
	buf[7] = 0;

	if (w1_f2f_write8(sl, buf, 0x20, tmp))
		return false;

	return true;
}

u8 Init_OneWire(void)
{
	struct w1_slave *sl;
	u8 tmp[32];

#ifndef BYHX_WTITE_BOARD_TOOL
	sl = w1_f2f_alloc(&w1_gpio_master, ripstar_secret);
	if (!sl)
		return false;

	if (w1_f2f_read(sl, tmp, 0x20, 32) != 32)
		return false;

	owBoardID = *((unsigned int *)tmp);
	owManufacturerID = *((unsigned short *)(tmp + sizeof(int)));
#endif
	return true;
}


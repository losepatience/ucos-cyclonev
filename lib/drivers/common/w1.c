/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jul 30 11:11:38 CST 2014
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

#include <w1.h>
#include <delay.h>
#include <crypto.h>

static unsigned char w1_read_bit(struct w1_master *master)
{
	unsigned char val;

	/*
	 *  \       /``````````````````\
	 * tF\_tRL_/....................\__
	 *
	 * the master generates a start signal by pull the master down for tRL.
	 * then release the master, and sample the master within tMSR
	 */

	/* sample timing is critical here */
	local_irq_save(master->irqflags);
	master->write_bit(0);
	udelay(6);
	master->write_bit(1);
	udelay(9);

	val = master->read_bit();
	local_irq_restore(master->irqflags);

	udelay(55);

	return val & 0x1;
}

static void w1_write_bit(struct w1_master *master, unsigned char bit)
{
	/*
	 *  \        /``````````````````\
	 * tF\_tW1L_/                    \__
	 *  \                      /````\
	 * tF\________tW0L________/      \__
	 *
	 * XXX: the master pulls the master down for tW1L or tW0L
	 */

	local_irq_save(master->irqflags);
	if (bit) {
		master->write_bit(0);
		udelay(6);
		master->write_bit(1);
		udelay(64);
	} else {
		master->write_bit(0);
		udelay(60);
		master->write_bit(1);
		udelay(10);
	}
	local_irq_restore(master->irqflags);
}

int w1_reset(struct w1_slave *sl)
{
	unsigned char val;
	struct w1_master *master = sl->master;

	/*
	 *  \               /```\XXXXXXXX/`````\
	 * tF\____tRSTL____/tPDH \XXXXXX/ tREC  \__
	 *                         tMSR
	 */
	master->write_bit(0);
	udelay(480);

	/*
	 * after tRSTL, Vpup and slave control the master.
	 * slave waits for tPDH and then transmits a Presence
	 * Pulse by pulling the line low for tPDL.
	 */
	master->write_bit(1);
	udelay(70);

	val = master->read_bit() & 0x1;
	udelay(410);

	return val;
}

unsigned char w1_read_8(struct w1_slave *sl)
{
	int i;
	unsigned char ret = 0;
	struct w1_master *master = sl->master;

	if (master->read_byte)
		return master->read_byte();

	for (i = 0; i < 8; ++i)
		ret |= (w1_read_bit(master) << i);

	return ret;
}

void w1_write_8(struct w1_slave *sl, unsigned char val)
{
	int i;
	struct w1_master *master = sl->master;

	if (master->write_byte)
		return master->write_byte(val);

	for (i = 0; i < 8; ++i)
		w1_write_bit(master, (val >> i) & 0x1);
}

int w1_read_block(struct w1_slave *sl, unsigned char *buf, int len)
{
	int i;
	struct w1_master *master = sl->master;

	if (master->read_block)
		return master->read_block(buf, len);

	for (i = 0; i < len; ++i)
		buf[i] = w1_read_8(sl);

	return len;
}

void w1_write_block(struct w1_slave *sl, const unsigned char *buf, int len)
{
	int i;
	struct w1_master *master = sl->master;

	if (master->write_block)
		return master->write_block(buf, len);

	for (i = 0; i < len; ++i)
		w1_write_8(sl, buf[i]);
}

int w1_reset_slave(struct w1_slave *sl)
{
	if (w1_reset(sl))
		return -1;

	w1_write_8(sl, W1_SKIP_ROM);
	return 0;
}

int w1_read_rom(struct w1_slave *sl, unsigned char *buf, int len)
{
	if (w1_reset(sl))
		return -1;

	w1_write_8(sl, W1_READ_ROM);
	w1_read_block(sl, buf, len);
	if (w1_crc8(buf, len))
		return -1;

	return 0;
}

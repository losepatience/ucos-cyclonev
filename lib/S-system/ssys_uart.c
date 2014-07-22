/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jul  8 13:15:00 CST 2014
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

#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <errno.h>

#include <fifo.h>
#include <ssl.h>
#include <platform.h>
#include <cycserial.h>
#include <serial_core.h>

#include <miscellance.h>
#ifndef MAX_SAPACKET_LEN
#  define MAX_SAPACKET_LEN	(32)
#endif

#define UART_CHNUM	4

/*
 * SA protocol:
 *
 * SA is short for simple-abc. this name comes from its simpleness
 *  1. packet is the xfer basic unit. there are 2 kinds of packet.
 *     normal packet and ACK packet, as show in the table below.
 *     .-------.-------.-------.-------.-------.
 *     |   0   |   1   |   2   |   3   | 4-31  |
 *     .-------+-------+-------+-------+-------.
 *     |   st  |toggle |chksum |length | data  |
 *     .-------+-------+-------+-------+-------.
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
	int		rx_toggle;
	int		tx_toggle;
	bool		txACK;

	spinlock_t	lock;
};

static struct SA_channel *SA_chans;

static u8 SA_gen_chksum(u8 *data, int len, int flag, u8 chksum)
{
	if (flag == CHKMOD_SUM) {
		int i;

		for (i = 0; i <	len; i++)
			chksum += data[i];
	} else {
		chksum = crc8(w1_crc8_table, data, len, chksum);
	}

	return chksum;
}

static bool SA_is_valid_packet(int num, void *data, int len, u8 chksum)
{
	u8 tmp;

	*((u8 *)data + 2) = 0;	/* XXX ignore the chksum byte */

	if (SA_chans[num].chkmod == CHKMOD_ANY) {
		tmp = SA_gen_chksum(data, len, CHKMOD_SUM, 0);
		if (tmp != chksum)
			tmp = SA_gen_chksum(data, len, CHKMOD_CRC, 0);
	} else {
		tmp = SA_gen_chksum(data, len, SA_chans[num].chkmod, 0);
	}

	return chksum == tmp;
}

/* TODO: UART_GetCMD only support callback for now */
static void SA_callback(void *arg)
{
	static u8 cmd[MAX_SAPACKET_LEN] = { 0xaa };
	static int idx;
	int num = *(int *)arg;
	unsigned char ch;

	uart_read(num, &ch, 1);

	if (fifo_unused(SA_chans[num].rxfifo) < MAX_SAPACKET_LEN)
		return;

	if (idx == 0 && ch != 0xaa)
		return;

	if (idx == 1) {
		if (ch == !SA_chans[num].rx_toggle) {
			cmd[idx++] = ch;
		} else {
			idx = 0;

			if (ch == 0x55)	/* this is an ACK */
				SA_chans[num].txACK = true;
		}

	} else if (idx == 3 && (ch > MAX_SAPACKET_LEN || ch < 5)) {
		idx = 0;
	} else if (idx > 3 && idx == cmd[3]) {

		const u8 ack[] = { 0xaa, 0x55 };	

		cmd[idx] = ch;
		idx = 0;

		/* cmd[2] is the check sum of the received packet */
		if (!SA_is_valid_packet(num, cmd, cmd[3], cmd[2]))
			return;

		SA_chans[num].rx_toggle = !SA_chans[num].rx_toggle;

		cmd[3] -= 3;
		fifo_in(SA_chans[num].rxfifo, &cmd[3], cmd[3]);
		uart_write(num, ack, 2);	/* ACK */
	} else {
		cmd[idx++] = ch;
	}
}

void UART_SetCheckModel(u8 num, u8 model)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&SA_chans[num].lock, flags);
	SA_chans[num].chkmod = model;
	spin_unlock_irqrestore(&SA_chans[num].lock, flags);
}

/* virtual uart device. data received would be stored in txfifo */
u8 UART_MotionGetCMD(u8 *data)
{
	unsigned long flags = 0;
	struct SA_channel *chan;
	struct fifo *fifo;
	int len;
	bool ret = false;

	chan = &SA_chans[UART_MOTION_CHANNEL];
	fifo = chan->txfifo;

	spin_lock_irqsave(&chan->lock, flags);

	if (fifo_cached(fifo) == 0)
		goto err_out;

	len = *((int *)fifo_oaddr(fifo));
	if (len > fifo_cached(fifo))
		goto err_out;

	fifo_out(fifo, data, len);

	ret = true;

err_out:
	spin_unlock_irqrestore(&chan->lock, flags);
	return ret;
}

static u8 UART_SendMotionCMD(u8 *data)
{
	unsigned long flags = 0;
	struct SA_channel *chan;
	struct fifo *fifo;
	u8 len;
	u8 err;

	chan = &SA_chans[UART_MOTION_CHANNEL];
	fifo = chan->txfifo;
	len = data[0];

	spin_lock_irqsave(&chan->lock, flags);
	if (fifo_unused(fifo) < len) {
		spin_unlock_irqrestore(&chan->lock, flags);
		return false;
	}

	fifo_in(fifo, data, len);
	//OSFlagPost(mix_FLAG_GRP, MOTION_SEND_CMD, OS_FLAG_SET, &err);

	spin_unlock_irqrestore(&chan->lock, flags);
	return true;
}

u8 UART_MotionReportCMD(u8 *data)
{
	unsigned long flags = 0;
	struct SA_channel *chan;
	struct fifo *fifo;
	u8 len;
	u8 err;

	chan = &SA_chans[UART_MOTION_CHANNEL];
	fifo = chan->rxfifo;
	len = data[0];

	spin_lock_irqsave(&chan->lock, flags);
	if (fifo_unused(fifo) < len) {
		spin_unlock_irqrestore(&chan->lock, flags);
		return false;
	}

	fifo_in(fifo, data, len);
	//OSFlagPost(mix_FLAG_GRP, MOTION_REPORT_CMD, OS_FLAG_SET, &err);
	spin_unlock_irqrestore(&chan->lock, flags);

	return true;
}

u8 UART_GetCMD(u8 num, u8 *data)
{
	unsigned long flags = 0;
	struct SA_channel *chan;
	struct fifo *fifo;
	bool ret = false;
	u8 len;

	chan = &SA_chans[num];
	fifo = chan->rxfifo;

	spin_lock_irqsave(&chan->lock, flags);

	if (fifo_cached(fifo) == 0)
		goto out;

	len = *((u8 *)fifo_oaddr(fifo));
	if (len > fifo_cached(fifo))
		goto out;

	fifo_out(fifo, data, len);
	ret = true;
out:
	spin_unlock_irqrestore(&chan->lock, flags);
	return ret;
}

u8 UART_SendCMD(u8 num, u8 *data)
{
	unsigned long flags = 0;
	struct SA_channel *chan;
	u8 buf[MAX_SAPACKET_LEN];
	bool ret = false;

	chan = &SA_chans[num];

	/* motion channel */
	if (UART_MOTION_CHANNEL == num)
		return UART_SendMotionCMD(data);

	chan = &SA_chans[num];

	buf[0] = 0xAA;
	buf[1] = chan->tx_toggle;
	buf[2] = 0;
	memcpy(&buf[3], data, data[0]);
	buf[3] = data[0] + 3;	/* len */
	buf[2] = SA_gen_chksum(buf, buf[3], chan->chkmod, 0);

	spin_lock_irqsave(&chan->lock, flags);
	if (!chan->txACK)
		goto out;

	if (uart_write(num, buf, buf[3]) == buf[3]) {
		chan->txACK = false;
		chan->tx_toggle = !chan->tx_toggle;
		ret = true;
	}
out:
	spin_unlock_irqrestore(&chan->lock, flags);
	return ret;
}

void UART_Init(u8 flag)
{
	int i = 0;
	void *txbuf;

	/* there is a virtual channel for motion contorl */
	SA_chans = calloc(UART_CHNUM + 1, sizeof(struct SA_channel));
	if (SA_chans == NULL) {
		pr_err("%s: no memory!", __func__);
		return;
	}

	txbuf = malloc(512);
	if (txbuf == NULL) {
		free(SA_chans);
		pr_err("%s: no memory!", __func__);
		return;
	}

	do {
		SA_chans[i].chkmod = 0;
		SA_chans[i].rx_toggle = 0;
		SA_chans[i].tx_toggle = 1;
		SA_chans[i].txACK = true;
		SA_chans[i].rxfifo = fifo_init(SA_chans[i].rxbuf, 1, 512);
	} while (++i < UART_CHNUM);


	cycserial_init(SA_callback);

	/* this is motion channel */
	SA_chans[UART_CHNUM].txfifo = fifo_init(txbuf, 1, 512);
}


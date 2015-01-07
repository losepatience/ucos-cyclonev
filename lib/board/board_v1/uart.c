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
#include <crypto.h>
#include <platform.h>
#include <ripstar_uart.h>

#include <miscellance.h>
#ifndef MAX_SAPACKET_LEN
#  define MAX_SAPACKET_LEN	(32)
#endif

#define UART_CHNUM		(4 + 1)

int uart_waittime[4];	/* not used, just for complitable */

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
	unsigned long		irqflags;

	unsigned int		fifosize;

#define CHKMOD_ANY		0
#define CHKMOD_SUM		1
#define CHKMOD_CRC		2
	int			chkmod;
	int			rx_toggle;
	int			tx_toggle;
	bool			txACK;
	struct fifo		*rxfifo;
	struct fifo		*txfifo;
	int			idx;
	u8			cmd[MAX_SAPACKET_LEN];
};

static struct SA_channel *SA_chans[UART_CHNUM];

static struct SA_channel *to_SA_channel(int num)
{
	return SA_chans[num];
}

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
	struct SA_channel *chan = to_SA_channel(num);
	u8 tmp;

	*((u8 *)data + 2) = 0;	/* XXX ignore the chksum byte */

	if (chan->chkmod == CHKMOD_ANY) {
		tmp = SA_gen_chksum(data, len, CHKMOD_SUM, 0);
		if (tmp != chksum)
			tmp = SA_gen_chksum(data, len, CHKMOD_CRC, 0);
	} else {
		tmp = SA_gen_chksum(data, len, chan->chkmod, 0);
	}

	return chksum == tmp;
}

/* TODO: UART_GetCMD only support callback for now */
static void SA_callback(void *arg)
{
	int num = *(int *)arg;
	struct SA_channel *chan = to_SA_channel(num);
	unsigned char ch;

	uart_read(num, &ch, 1);

	if (fifo_unused(chan->rxfifo) < MAX_SAPACKET_LEN)
		return;

	if (chan->idx == 0 && ch != 0xaa)
		return;

	if (chan->idx == 1) {
		if (ch == !chan->rx_toggle) {
			chan->cmd[chan->idx++] = ch;
		} else {
			chan->idx = 0;

			if (ch == 0x55)	/* this is an ACK */
				chan->txACK = true;
		}

	} else if (chan->idx == 3 && (ch > MAX_SAPACKET_LEN || ch < 5)) {
		chan->idx = 0;
	} else if (chan->idx > 3 && chan->idx == chan->cmd[3]) {

		const u8 ack[] = { 0xaa, 0x55 };

		chan->cmd[chan->idx] = ch;
		chan->idx = 0;

		/* chan->cmd[2] is the check sum of the received packet */
		if (!SA_is_valid_packet(num, chan->cmd,
					chan->cmd[3], chan->cmd[2]))
			return;

		chan->rx_toggle = !chan->rx_toggle;

		chan->cmd[3] -= 3;
		fifo_in(chan->rxfifo, &chan->cmd[3], chan->cmd[3]);
		uart_write(num, ack, 2);	/* ACK */
	} else {
		chan->cmd[chan->idx++] = ch;
	}
}

static void __UART_SetCheckModel(u8 num, u8 model)
{
	struct SA_channel *chan = to_SA_channel(num);

	local_irq_save(chan->irqflags);
	chan->chkmod = model;
	local_irq_restore(chan->irqflags);
}

void UART_SetCheckModel(u8 num, u8 model)
{
	if (num == UART_HEAD_CHANNEL) {
		int i;

		for (i = 0; i < 4; i++)
			__UART_SetCheckModel(i, model);
	} else {
		__UART_SetCheckModel(num, model);
	}
}

/* virtual uart device. data received would be stored in txfifo */
u8 UART_MotionGetCMD(u8 *data)
{
	struct SA_channel *chan = to_SA_channel(UART_MOTION_CHANNEL);
	struct fifo *fifo = chan->txfifo;
	bool ret = false;
	int len;


	local_irq_save(chan->irqflags);

	if (fifo_cached(fifo) == 0)
		goto err_out;

	len = *((u8 *)fifo_oaddr(fifo));
	if (len > fifo_cached(fifo))
		goto err_out;

	fifo_out(fifo, data, len);

	ret = true;

err_out:
	local_irq_restore(chan->irqflags);
	return ret;
}

static u8 UART_SendMotionCMD(u8 *data)
{
	struct SA_channel *chan = to_SA_channel(UART_MOTION_CHANNEL);
	struct fifo *fifo = chan->txfifo;
	u8 len = data[0];
	u8 err;

	local_irq_save(chan->irqflags);
	if (fifo_unused(fifo) < len) {
		local_irq_restore(chan->irqflags);
		return false;
	}

	fifo_in(fifo, data, len);
	OSFlagPost(mix_FLAG_GRP, MOTION_SEND_CMD, OS_FLAG_SET, &err);

	local_irq_restore(chan->irqflags);
	return true;
}

u8 UART_MotionReportCMD(u8 *data)
{
	struct SA_channel *chan = to_SA_channel(UART_MOTION_CHANNEL);
	struct fifo *fifo = chan->rxfifo;
	u8 len = data[0];
	u8 err;


	local_irq_save(chan->irqflags);
	if (fifo_unused(fifo) < len) {
		local_irq_restore(chan->irqflags);
		return false;
	}

	fifo_in(fifo, data, len);
	OSFlagPost(mix_FLAG_GRP, MOTION_REPORT_CMD, OS_FLAG_SET, &err);
	local_irq_restore(chan->irqflags);

	return true;
}

u8 __UART_GetCMD(u8 num, u8 *data)
{
	struct SA_channel *chan = to_SA_channel(num);
	struct fifo *fifo = chan->rxfifo;
	bool ret = false;
	u8 len;

	local_irq_save(chan->irqflags);

	if (fifo_cached(fifo) == 0)
		goto out;

	len = *((u8 *)fifo_oaddr(fifo));
	if (len > fifo_cached(fifo))
		goto out;

	fifo_out(fifo, data, len);
	ret = true;
out:
	local_irq_restore(chan->irqflags);
	return ret;
}

u8 UART_GetCMD(u8 num, u8 *data)
{
	if (num == UART_HEAD_CHANNEL) {
		int i;

		for (i = 0; i < 4; i++)
			if (__UART_GetCMD(i, data)) {
				data[data[0]++] = i + 1;
				return true;
			}
	} else {
		return __UART_GetCMD(num, data);
	}

	return false;
}

u8 __UART_SendCMD(u8 num, u8 *data)
{
	struct SA_channel *chan;
	u8 buf[MAX_SAPACKET_LEN];
	bool ret;

	/* motion channel */
	if (UART_MOTION_CHANNEL == num)
		return UART_SendMotionCMD(data);

	chan = to_SA_channel(num);
	ret = false;

	buf[0] = 0xAA;
	buf[1] = chan->tx_toggle;
	buf[2] = 0;
	memcpy(&buf[3], data, data[0]);
	buf[3] = data[0] + 3;	/* len */
	buf[2] = SA_gen_chksum(buf, buf[3], chan->chkmod, 0);

	local_irq_save(chan->irqflags);
	if (!chan->txACK)
		goto out;

	if (uart_write(num, buf, buf[3]) == buf[3]) {
		chan->txACK = false;
		chan->tx_toggle = !chan->tx_toggle;
		ret = true;
	}
out:
	local_irq_restore(chan->irqflags);
	return ret;
}


u8 UART_SendCMD(u8 num, u8 *data)
{
	if (num == UART_HEAD_CHANNEL) {
		int i;

		for (i = 0; i < 4; i++) {
			if (!__UART_SendCMD(i, data))
				return false;
		}
	} else {
		return __UART_SendCMD(num, data);
	}

	return true;
}

static struct SA_channel *SA_add_channel(int num)
{
	struct SA_channel *chan;
	void *buf;

	chan = malloc(sizeof(struct SA_channel));
	if (chan == NULL) {
		pr_err("%s: no memory!\n", __func__);
		return NULL;
	}
	memset(chan, 0, sizeof(struct SA_channel));

	chan->fifosize = 512;
	buf = malloc(chan->fifosize);
	if (buf == NULL) {
		free(chan);
		pr_err("%s: no memory for fifo!\n", __func__);
		return NULL;
	}

	chan->chkmod = 0;
	chan->rx_toggle = 0;
	chan->tx_toggle = 1;
	chan->txACK = true;
	chan->rxfifo = fifo_init(buf, 1, chan->fifosize);

	SA_chans[num] = chan;

	return chan;
}

void UART_Init(u8 flag)
{
	static unsigned char SA_txbuf[512];
	struct SA_channel *chan;
	int num;

	for (num = 0; num < UART_CHNUM; num++) {
		chan = SA_add_channel(num);
		if (chan == NULL) {
			pr_err("%s: failed to add SA channel\n", __func__);
			return;
		}

		if (num == UART_CHNUM - 1) {	/* motion channel */
			chan->txfifo = fifo_init(SA_txbuf, 1, sizeof(SA_txbuf));
			SA_chans[num] = chan;
		} else {
			cyc_uart_port_add(num);
			uart_register_rxcb(num, SA_callback);
		}
	}
}


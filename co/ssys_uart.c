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
#include <delay.h>

#include <fifo.h>
#include <ssl.h>
#include <platform.h>
#include <fpga_uart.h>

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

	struct mutex	mutex;
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

static int SA_find_packet_st(int num)
{
	u8 ch;

	while (fpga_uart_read(num, &ch, 1) == 1)
		if (ch == 0xaa)
			return 0;

	return -EAGAIN;
}

static int SA_recv_packet(int num)
{
	u8 cmd[MAX_SAPACKET_LEN] = { 0xaa };
	u8 ack[2] = { 0xaa, 0x55 };

	if (SA_find_packet_st(num)) {
		pr_debug("%s: no packet st has been found!\n", __func__);
		return -EAGAIN;
	}

	if (fpga_uart_read(num, &cmd[1], 1) != 1)
		return -EIO;

	/* this is a ACK from headboard */
	if (cmd[1] == 0x55) {
		SA_chans[num].txACK = true;
		return 0;
	}

	if (fifo_unused(SA_chans[num].rxfifo) < MAX_SAPACKET_LEN) {
		pr_debug("%s: rxfifo is full!\n", __func__);
		return -EAGAIN;
	}

	/* this packet has already received */
	if (cmd[1] == SA_chans[num].rx_toggle)
		return -EAGAIN;

	if (fpga_uart_read(num, &cmd[2], 2) != 2)
		return -EIO;

	/* cmd[3] is the length of the received packet */
	if (fpga_uart_read(num, &cmd[4], cmd[3]) != cmd[3])
		return -EAGAIN;

	/* cmd[2] is the check sum of the received packet */
 	if (!SA_is_valid_packet(num, cmd, cmd[3], cmd[2]))
		return -EAGAIN;

	SA_chans[num].rx_toggle = !SA_chans[num].rx_toggle;

	fifo_in(SA_chans[num].rxfifo, &cmd[3], cmd[3] - 3);
	fpga_uart_write(num, ack, 2);	/* ACK */

	return 0;
}

static bool SA_check_ACK(int num)
{
	int retries = 10;

	do {
		SA_recv_packet(num);
		if (SA_chans[num].txACK) {
			SA_chans[num].txACK = false;
			return true;
		}

	} while (--retries);

	return false;
}

void UART_SetCheckModel(u8 num, u8 model)
{
	mutex_lock(&SA_chans[num].mutex);
	SA_chans[num].chkmod = model;
	mutex_unlock(&SA_chans[num].mutex);
}

/* virtual uart device. data received would be stored in txfifo */
u8 UART_MotionGetCMD(u8 *data)
{
	int num = UART_MOTION_CHANNEL;

	mutex_lock(&SA_chans[num].mutex);

	if (fifo_cached(SA_chans[num].txfifo) < 1) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_out(SA_chans[num].txfifo, data, 1);
	fifo_out(SA_chans[num].txfifo, data + 1, *data - 1);

	mutex_unlock(&SA_chans[num].mutex);
	return true;
}

static u8 UART_SendMotionCMD(u8 *data)
{
       u8 err;
       u8 len;
       int num = UART_MOTION_CHANNEL;

       len = data[0];

       mutex_lock(&SA_chans[num].mutex);
       if (fifo_unused(SA_chans[num].txfifo) < len) {
               mutex_unlock(&SA_chans[num].mutex);
               return false;
       }

       fifo_in(SA_chans[num].txfifo, data, len);
       //OSFlagPost(mix_FLAG_GRP, MOTION_SEND_CMD, OS_FLAG_SET, &err);

       mutex_unlock(&SA_chans[num].mutex);
       return true;
}

u8 UART_MotionReportCMD(u8 *data)
{
	u8 err;
	u8 len;
	int num = UART_MOTION_CHANNEL;

	len = data[0];

	mutex_lock(&SA_chans[num].mutex);
	if (fifo_unused(SA_chans[num].rxfifo) < len) {
		mutex_unlock(&SA_chans[num].mutex);
		return false;
	}

	fifo_in(SA_chans[num].rxfifo, data, len);
	//OSFlagPost(mix_FLAG_GRP, MOTION_REPORT_CMD, OS_FLAG_SET, &err);
	mutex_unlock(&SA_chans[num].mutex);

	return true;
}

u8 UART_GetCMD(u8 num, u8 *data)
{
	mutex_lock(&SA_chans[num].mutex);
	while (SA_recv_packet(num))
		msleep(1);
	mutex_unlock(&SA_chans[num].mutex);

	fifo_out(SA_chans[num].rxfifo, data, 1);
	fifo_out(SA_chans[num].rxfifo, data + 1, data[0] - 1);
	return true;
}

u8 UART_SendCMD(u8 num, u8 *data)
{
	u8 tmp;
	u8 buf[4];
	u8 *src = data + 1;
	u8 len = data[0] - 1;

	/* motion channel */
	if (UART_MOTION_CHANNEL == num)
		return UART_SendMotionCMD(data);

	buf[0] = 0xAA;
	buf[1] = SA_chans[num].tx_toggle;
	buf[2] = 0;
	buf[3] = data[0] + 3;	/* len */

	tmp = SA_gen_chksum(buf, 4, SA_chans[num].chkmod, 0);
	tmp = SA_gen_chksum(src, len, SA_chans[num].chkmod, tmp);
	buf[2] = tmp;

	mutex_lock(&SA_chans[num].mutex);

	do {
		fpga_uart_write(num, buf, 4);
		fpga_uart_write(num, src, len);
		msleep(1);
	} while (SA_check_ACK(num));

	SA_chans[num].tx_toggle = !SA_chans[num].tx_toggle;
	mutex_unlock(&SA_chans[num].mutex);
	return true;
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
		pr_err("%s: no memory!", __func__);
		return;
	}

	do {
		mutex_init(&SA_chans[i].mutex);
		SA_chans[i].chkmod = 0;
		SA_chans[i].rx_toggle = 0;
		SA_chans[i].tx_toggle = 0;
		SA_chans[i].txACK = false;
		SA_chans[i].rxfifo = fifo_init(SA_chans[i].rxbuf, 1, 512);
		fpga_uart_init(i);
	} while (++i < UART_CHNUM);

	/* this is motion channel */
	SA_chans[UART_CHNUM].txfifo = fifo_init(txbuf, 1, 512);
}


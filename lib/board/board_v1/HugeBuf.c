/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jul 23 10:41:48 CST 2014
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

#include <circ.h>
#include <status.h>
#include <stddef.h>
#include <hugebuf.h>
#include <usb.h>
#include <ezusb.h>
#include <platform.h>

#include <control.h>
#include <miscellance.h>
#include <fpga.h>
#include <asm/io.h>

#define __report	status_ReportStatus
#define HEADSIZE	1024	/* heads is 1024bytes aligned */

extern volatile INT8U USBDMA_manualStop; /*not used*/
extern INT16U *CurHugeBufDataAddr;

static circ_t __circ;

#ifndef CCCC
volatile INT32U TickAfterLastComm;
extern INT32U cur_step_distance;

int DoFlash_EPSON_MICOLOR(void)
{
	return 0;
}

void CONTROL_OPEN_SUCKER(void)
{
	return;
}
void CONTROL_CLOSE_SUCKER(void)
{
	return;
}

INT32U BOARD_DDRAM_SIZE;
INT8U FPGA_RESET(void)
{
	return 1;
}

#define BULK_BASE	0xff200200
#define BULK_BUSY	1 << 1

#define DMA_BASE	0xff200280
#define DMA_BUSY	1 << 1

int wait_for_fpga_dma_idle(void)
{
	while (1) {
		if (!(readl((void *)DMA_BASE) & DMA_BUSY))
			break;

		msleep(1);
	}

	return true;
}

int wait_for_usb_dma_idle(void)
{
	while (1) {
		if (!(readl((void *)BULK_BASE) & BULK_BUSY))
			break;

		msleep(1);
	}

	return true;
}
#endif

circ_t *__to_circ(void)
{
	return &__circ;
}

void HugeBuf_Reset(void)
{
	circ_t *circ = __to_circ();
	unsigned long flags = 0;

	local_irq_save(flags);
	fifo_reset(circ->fifo);
	local_irq_restore(flags);
}

u8 HugeBuf_Flush(ssize_t size)
{
	circ_t *circ = __to_circ();
	struct fifo *fifo = circ->fifo;
	unsigned long flags = 0;
	bool ret;

	local_irq_save(flags);

	if (size > fifo_cached(fifo)) {
		ret = false;
	} else {
		fifo_out(fifo, NULL, size);
		ret = true;
	}

	local_irq_restore(flags);
	return ret;
}

u8 HugeBuf_GetInfHead(DataHeaderType *header)
{
	circ_t *circ = __to_circ();
	struct fifo *fifo = circ->fifo;
	unsigned long flags = 0;
	void *addr;
	int size;

	if (fifo_cached(fifo) < circ->sizeof_header)
		return false;

	addr = fifo_oaddr(fifo);
	memcpy(header, addr, sizeof(DataHeaderType));

	if (memcmp(&header->header_flag, "XHYB", 4)) {
		__report(STATUS_FTA_INTERNAL_WRONGHEADER, STATUS_SET);
		while (1)
			OSTimeDly(1);

		return false;
	}

	size = circ->sizeof_header;
	if (header->header_size != size) {
		__report(STATUS_FTA_INTERNAL_WRONGHEADERSIZE, STATUS_SET);
		return false;
	}

	local_irq_save(flags);

	HugeBuf_Flush(size);

	CurHugeBufDataAddr = fifo_oaddr(fifo);
	size = header->data_bytescnt;
	circ->nextheader = fifo_oaddr_plus(fifo, size);
	circ->sizeof_band = size;
	local_irq_restore(flags);
	return true;
}

void HugeBuf_FlushCurBand(void)
{
	circ_t *circ = __to_circ();
	struct fifo *fifo = circ->fifo;
	unsigned long flags = 0;
	int cnt;

	wait_for_fpga_dma_idle();

	local_irq_save(flags);
	cnt = circ->nextheader - fifo_oaddr(fifo);
	fifo_out(fifo, NULL, cnt);
	local_irq_restore(flags);
}

ssize_t HugeBuf_GetSize(void)
{
	circ_t *circ = __to_circ();
	unsigned long flags = 0;
	int cnt;

	local_irq_save(flags);
	cnt = fifo_cached(circ->fifo);
	local_irq_restore(flags);

	return (ssize_t)cnt;
}

u8 *HugeBuf_GetRecAddr(void)
{
	circ_t *circ = __to_circ();
	unsigned long flags = 0;
	u8 *addr;

	local_irq_save(flags);
	addr = (u8 *)fifo_iaddr(circ->fifo);
	local_irq_restore(flags);

	return addr;
}

void HugeBuf_PutSize(size_t size)
{
	circ_t *circ = __to_circ();
	unsigned long flags = 0;

	local_irq_save(flags);
	fifo_in(circ->fifo, NULL, size);
	local_irq_restore(flags);
}

void HugBuf_InitReceiveSize(void)
{
	circ_t *circ = __to_circ();

	circ->xinfo.remainning = 0;
	circ->xinfo.first = true;
	circ->xinfo.cnt = HEADSIZE;
	circ->sizeof_header = HEADSIZE;
}

void HugBuf_Init(void)
{
	circ_t *circ = __to_circ();
	unsigned long start, size;

	start = __OS_BASE + __OS_SIZE;
	size = PHYS_SDRAM_SIZE - start;

	HugBuf_InitReceiveSize();
	circ->fifo = fifo_init((void *)start, 1, size);
}

static void __callback(void *arg, u8 stat, u32 xmit, u32 remaining)
{
	if (!stat)
		USBDMA_EP2_DoneInterrupt();
}

/* fill the fifo until it is full */
void HugeBuf_StartOneReceive(void)
{
	circ_t *circ = __to_circ();
	struct fifo *fifo = circ->fifo;
	__rx_info_t *info = &circ->xinfo;
	unsigned long flags = 0;

	if (USBDMA_manualStop) {
		g_USBTransStoped = true;
		return;
	}

	wait_for_usb_dma_idle();

	local_irq_save(flags);

	g_USBTransStoped = false;
	if (fifo_unused(fifo) >= info->cnt) {
		void *addr = fifo_iaddr(fifo);

		USBD_Read(USB_OUT_EP, addr, info->cnt, __callback, 0);
	} else {
		g_USBTransStoped = true;
	}
	local_irq_restore(flags);
}

void HugeBuf_CompleteOneReceive(void)
{
	circ_t *circ = __to_circ();
	struct fifo *fifo = circ->fifo;
	__rx_info_t *info = &circ->xinfo;

	if (info->first) {
		DataHeaderType * head;

		head = (DataHeaderType *)fifo_iaddr(fifo);

		if (!memcmp(&head->header_flag, "XHYB", 4))
			info->remainning = head->data_bytescnt;
		else
			info->remainning = 0;

		if (info->remainning != 0)
			info->first = false;

	} else {
		info->remainning -= info->cnt;
		if (info->remainning == 0)
			info->first = true;
	}

	fifo_in(fifo, NULL, info->cnt);

	if (info->first) {
		info->cnt = circ->sizeof_header;
	} else {
		info->cnt = min(info->remainning, 0x10000 - circ->sizeof_header);
		info->cnt = min(info->cnt, fifo_unused_nowrap(fifo));
	}
	return;
}

u16 *HugeBuf_GetCodeAddr(ssize_t len)
{
	circ_t *circ = __to_circ();

	if (fifo_cached(circ->fifo) < len)
		return 0;

	return fifo_oaddr(circ->fifo);
}

void HugeBuf_CancelJob(void)
{
	unsigned long flags = 0;
	u8 buf[10];
	u8 err;

	buf[0] = 2;
	buf[1] = UART_DSP_STOP_PRINT;

	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
		OSTimeDly(100);

	FPGADMA_stop();
	USBD_AbortDataRead(USB_OUT_EP);
	OSTimeDly(1);

	local_irq_save(flags);

	USBDMA_manualStop = false;
	HugeBuf_Reset();
	HugBuf_InitReceiveSize();
	HugeBuf_StartOneReceive();
	local_irq_restore(flags);

	status_ReportStatus(STATUS_NO_CANCEL, STATUS_SET);


	g_yEncoderInf.bandIndex = -1;
	g_yEncoderInf.encoderPos = 0;
	g_yEncoderInf.bandY = 0;
	g_yEncoderInf.nextBandY = 0;
	g_yEncoderInf.bandResY = 0;
	g_yEncoderInf.fStepTotal = 0;

	g_adjustInf.passStepValue = 0;
	g_adjustInf.passAdvance = 0;
	g_adjustInf.feedBackY = 0;
	g_adjustInf.bidirectionValue = 0;

	cur_step_distance = 0;

	/* Go home */
	move_dir = 2;
	move_distance = 0;
	move_type = 0;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
	OSTimeDly(2);

	/* Waiting moving flag cleared */
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING | CMD_START_MOVE,
			OS_FLAG_WAIT_CLR_ALL, 0, &err);

#ifdef MANUFACTURER_DYSS
	if ((LCDMenuConfig_EX.b_Sundry & DYSS_Y_LIMIT) != 0) {
		move_dir = 3;	/* Go home */
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSTimeDly(2);

		/* Waiting moving flag cleared */
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING,
				OS_FLAG_WAIT_CLR_ANY, 0, &err);
	}
#endif


	if ((rFPGA_STATUS & 0x7) == 4)
		rFPGA_COMMAND = rFPGA_COMMAND_END_SSHAKE;

	DisableFPGAPosiIT(0);

	if (factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin
		|| factoryData.HeadType == HeadNo_Ricoh_Gen4_G4
		|| factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L
		|| factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin) {
		buf[0] = 2;
		buf[1] = UART_HEAD_EPSON_JOBEND;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
			OSTimeDly(10);
	}

	g_shakehandPassed = false;

	OSTimeDly(10);
}

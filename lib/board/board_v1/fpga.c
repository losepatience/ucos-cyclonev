/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jul 17 11:35:45 CST 2014
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
#include <job.h>
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

#define BULK_BASE	0xff200280

struct __dma {
	void			*base;
	struct completion	wait;
} __dma;

static volatile INT8U FPGADMA_manualStop = false;
//nozzle_mask_t gl_nozzle_mask;
volatile INT8U USBDMA_manualStop = false;

struct __dma *to_dma()
{
	return &__dma;
}

unsigned char FPGA_SendData(unsigned char first);

/* this is irq from fpga. */
static void fpga_notify_interrupt(void *arg)
{
	/* rFPGA_STATUS is a read-clear register */
	unsigned int status = rFPGA_STATUS;

	if (status & rFPGA_STATUS_TOO_MUCH)
		status_ReportStatus(STATUS_WAR_FPGA_ULTRADATA, STATUS_SET);
	else if (status & rFPGA_STATUS_TOO_LESS)
		status_ReportStatus(STATUS_WAR_FPGA_LESSDATA, STATUS_SET);
	else if (status & rFPGA_STATUS_ERR_CMD)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGCOMMAND, STATUS_SET);
	else if (status & rFPGA_STATUS_ERR_STAT)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGSTATUS, STATUS_SET);
	else
		status_ReportStatus(STATUS_ERR_MOTION_SPEED_BAD, STATUS_SET);

}

static void dma_interrupt(void *arg);

u8 fpga_init(void)
{
	struct __dma *dma = to_dma();
	dma->base = BULK_BASE;
	request_irq(74, dma_interrupt, NULL);

	return true;
}

u8 fpga_cfg(void)
{
	u32 fpga_cfg = 0;

	/*
	 * data source: USB
	 * X coor source: raster
	 */
	//REG_WRITE(REG_FPGA_CONFIG, fpga_cfg);

	return true;
}

static void dma_start(int en)
{
	u32 reg;
	unsigned long flags;
	struct __dma *dma = to_dma();

	local_irq_save(flags);
	reg = readl(dma->base + 0x18);
	if (en)
		reg |= 1 << 4;
	else
		reg &= ~(1 << 4);
	writel(reg, dma->base + 0x18);
	local_irq_restore(flags);
}

void FPGADMA_stop(void)
{
	struct __dma *dma = to_dma();
	unsigned long flags;
	u32 reg;

	FPGADMA_manualStop = true;

	local_irq_save(flags);
	reg = readl(dma->base + 0x18);
	reg |= 1 << 6;
	reg &= ~(1 << 4);
	writel(reg, dma->base + 0x18);
	local_irq_restore(flags);
}

static void __send(const void *buf, int len)
{
	struct __dma *dma = to_dma();

	writel(buf, dma->base + 0x8);
	writel(len / 4, dma->base + 0xC);
	writel(0x9c, dma->base + 0x18);
}

static void dma_interrupt(void *arg)
{
	int len;
	void *addr;
	struct fifo *fifo = __to_circ()->fifo;

	addr = fifo_oaddr(fifo);
	len = min(1024 * 16, fifo_cached_nowrap(fifo));

	if (len == 0) {
		dma_start(0);
		return;
	}

	__send(addr, len);
}

unsigned char FPGA_SendData(unsigned char first)
{
	wait_for_fpga_dma_idle();

	if (first) {
		FPGADMA_manualStop = false;
		dma_start(1);
	} else {
		return false;
	}

	return true;
}

void FPGA_START_RICOH_AUTO_C(void )
{
	rFPGA_COMMAND = rFPGA_COMMAND_BGN_AUTO_C_1;
}

void FPGA_START_FLASH_RICOH(unsigned char DataMask)
{
	unsigned char flash_level = 0;
	if (0 <= cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval
		&&cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval <= 5)
		flash_level = cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval;
	else
		flash_level = 0;
#ifdef MANUFACTURER_DYSS
		flash_level = 1;
#endif

	if(DataMask != 0)
		rFPGA_COMMAND = rFPGA_COMMAND_BEGIN_FLASH
			| 0xFF | (flash_level << 8);
	else
		rFPGA_COMMAND = rFPGA_COMMAND_BEGIN_FLASH;
}

void FPGA_STOP_FLASH(void)
{

	rFPGA_COMMAND = rFPGA_COMMAND_END_FLASH;
}


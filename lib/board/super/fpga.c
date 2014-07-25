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

#include <fifo.h>
#include <job.h>
#include <status.h>
#include <stddef.h>
#include <hugebuf.h>
#include <usb.h>
#include <ezusb.h>
#include <platform.h>

#include <uv.h>
#include <control.h>
#include <miscellance.h>
#include <fpga.h>

typedef struct __rx_info {
	bool		first;
	int		remainning;
	int		cnt;
} __rx_info_t;

typedef struct circ {
	spinlock_t	lock;
	struct fifo	*fifo;

	int		sizeof_header;	/* const */
	int		sizeof_band;
	void		*nextheader;	/* FlushCurBand */
	int		band_left;

	__rx_info_t	xinfo;
} circ_t;


static volatile INT8U FPGADMA_manualStop = false;


volatile INT8U USBDMA_manualStop = false;
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


void FPGADMA_stop(void)
{
	FPGADMA_manualStop = true;
	//11 DMAD_AbortTransfer(FPGA_DMA_CHANNEL_NUM);
}


unsigned char FPGA_SendData(unsigned char first) 
{
	unsigned char ret = true;
	unsigned short cur_FPGADMA_StepSize;

	circ_t *circ = __to_circ();
	unsigned long flags = 0;

	wait_for_fpga_dma_idle();
	
	spin_lock_irqsave(&circ->lock, flags);
	
	if (first) {
		FPGADMA_manualStop = false;
	} else {

		int cnt = FPGADMA_TRANS_COUNT * 2;
		fifo_out(circ->fifo, NULL, cnt);
	}

	if (circ->xinfo.remainning > 0 && !FPGADMA_manualStop) {
		if (first == true)
		{
		//	AT91C_BASE_AIC->AIC_ICCR = (1<<AT91C_ID_FIQ);
		}
		//IRQ_EnableIT(AT91C_ID_FIQ);
	} else {
		ret = false;	
	}
	spin_unlock_irqrestore(&circ->lock, flags);
	return ret;
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


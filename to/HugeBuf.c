#include "os_includes.h"
#include "frmwrk_cfg.h"
#include "HugeBuf.h"
#include "FPGA.h"
#include "consol.h"
#include "status.h"
#include "job.h"
#include "global.h"
#include "miscellance.h"
#include "flash.h"
#include "board_memories.h"

#include "stdlib.h"

#ifdef UV_PRINTER
#include "uv.h"
#endif

#include "dmad/dmad.h"
#include "pio/pio_it.h"

INT32U BOARD_DDRAM_SIZE;

extern INT32U FPGA_FileLen;
extern INT32U FPGA_File;
extern INT32U cur_step_distance;
/******************************************************************************
*	Process DMA transfer EP2 FIFO => SDRAM									  *
*						 SDRAM	  => FPGA FIFO								  *
*	For test purpose	 SDRAM	  => EP6 FIFO, must set FLAGB first  		  *	
*	All functions must run in one task								          * 		
******************************************************************************/
static INT16U *HugeBufReceiveAddr;
static INT16U *HugeBufSendAddr;
static INT16U *HugeBufNextHeader = 0;
static INT16U *HugeBufLastHeader;
static INT32U HugeBufLastBandSize = 0;
extern INT16U *CurHugeBufDataAddr;
static volatile INT8U FPGADMA_manualStop = False;
volatile INT8U USBDMA_manualStop = False;

static volatile INT32S HugeBufSize = 0; //Left in buffer, unit:bytes, 512 bytes aligned

static unsigned int USBDMA_SendCount; //Not use, for SX2 IN transfer

//USB
#define USBDMA_TRANS_COUNT 		512
#define USBDMA_BYTES_PER_TRANS 	sizeof(INT16U)

#define FPGADMA_TRANS_DOUBLE_COUNT 	1024
#define USBDMA_BytesCount_Min (USBDMA_TRANS_COUNT*USBDMA_BYTES_PER_TRANS)
#define USBDMA_WordsCount_Min (USBDMA_TRANS_COUNT*USBDMA_BYTES_PER_TRANS/sizeof(INT16U))
static INT16U USBDMA_BytesCount = USBDMA_BytesCount_Min;
static INT16U USBDMA_WordsCount = USBDMA_WordsCount_Min;
static INT8U USBDMA_IsFirstTransfer = True;
static INT32U USBDMA_PackageSize = 0;

static const INT16U FPGADMA_BytesCount = FPGADMA_TRANS_COUNT*FPGADMA_BYTES_PER_TRANS;
static const INT16U FPGADMA_WordsCount = FPGADMA_TRANS_COUNT*FPGADMA_BYTES_PER_TRANS/sizeof(INT16U);

static const INT16U FPGADMA_Double_BytesCount = FPGADMA_TRANS_DOUBLE_COUNT*FPGADMA_BYTES_PER_TRANS;
static const INT16U FPGADMA_Double_WordsCount = FPGADMA_TRANS_DOUBLE_COUNT*FPGADMA_BYTES_PER_TRANS/sizeof(INT16U);


#ifdef ADOPT_COMPRESS_DATA
#error "A+ doesn't support compress data"
#endif

static INT16U cur_FPGADMA_BytesCount = 0;
//static INT16U cur_FPGADMA_WordsCount = 0;
static INT16U cur_FPGADMA_TransCount = 0;

#ifdef HEAD_EPSON_GEN5
static INT8U bStartPreFill = 0;
static INT8U bStartPostFill = 0;
static INT32U bPreFilledSize = 0;
static INT32S bPreFillLength = 0;
static INT32U bPostFilledSize = 0;
static INT32U bPostFillLength = 0;
static INT16U * FillBuffer;
#endif

unsigned int tick_1=0, tick_2 = 0, tick_3 =0;

#if 0
void FPGADMA_stop()
{
	FPGADMA_manualStop = True;
	DMAD_AbortTransfer(FPGA_DMA_CHANNEL_NUM);
}
#endif

void HugeBuf_CancelJob(void) //Need rewrite this function
{
	OS_CPU_SR cpu_sr;
	INT8U err;	
	INT8U buf[10];
	
#ifdef UV_PRINTER
	uv_PrintCancelJob();
#endif

#ifdef FULGENCY_FUN
		UV_CTR_PRT_START_CLEAR();
		UV_CTR_PRT_FINISH_SET();
#endif

#ifdef MANUFACTURER_DYSS
	if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_AUTO)
		ClosePlateFan();
	buf[0] = 2; //Length									
	buf[1] = UART_DSP_STOP_PRINT; 
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);	
#endif
	
	//FPGADMA_stop();	
	USBD_AbortDataRead(USB_OUT_EP);	
	OSTimeDly(1);
	
	OS_ENTER_CRITICAL();	
	
	USBDMA_manualStop = False;
	HugeBuf_Reset(); 	
	HugBuf_InitReceiveSize();
	HugeBuf_StartOneReceive();  	
	OS_EXIT_CRITICAL(); 	
	
	status_ReportStatus(STATUS_NO_CANCEL, STATUS_SET);
	
	
	//#ifdef Y_ENCODER_ENABLE					
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
	
	//#endif				
#ifdef EPSON_FLASH_IDLE
	move_dir = 2;
	move_distance = curPos.x - cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
	move_type = 0;
#if defined (EPSON_CLEAN_UPDOWN)
	postCapping_flash = True;
#endif
	cleanPara.flash = True;
	
#else
	move_dir = 2; //Go home
	move_distance = 0;
	move_type = 0;
#if defined (EPSON_CLEAN_UPDOWN)
	RegPostCapping(); //waiting action completed
#endif
	
#endif
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
	OSTimeDly(2);		
	
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving flag cleared, 

#ifdef MANUFACTURER_DYSS
	if((LCDMenuConfig_EX.b_Sundry & DYSS_Y_LIMIT) != 0)
	{
		move_dir = 3; //Go home
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSTimeDly(2);		
		
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared, 
	}
#endif

#if defined(FULGENCY_FUN)
	Y_GOHOME_Dirty = Y_GOHOME_IDLE;
#endif

#if defined(HEAD_RICOH_G4)
	if((rFPGA_STATUS & 0x7) == 4)
	{
		rFPGA_COMMAND = rFPGA_COMMAND_END_SSHAKE;
	}
	DisableFPGAPosiIT(0);	
#endif
	
#if defined( HEAD_EPSON_GEN5)
	if (factoryData.HeadType == HeadNo_Epson_Gen5)
	{
		buf[0] = 2;
		buf[1] = UART_HEAD_EPSON_JOBEND;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
			OSTimeDly(10);				
	}
#elif defined(HEAD_RICOH_G4)
	if (factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin ||factoryData.HeadType == HeadNo_Ricoh_Gen4_G4||
		factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L || factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin)
	{
		buf[0] = 2;
		buf[1] = UART_HEAD_EPSON_JOBEND;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
			OSTimeDly(10);				
	}
#endif
	
	g_shakehandPassed = False;
	HugeBufLastBandSize = 0;
	
	OSTimeDly(10);		

}


//=====================================================================================
//	FPGA FUNCTIONS:
//=====================================================================================

//Max frequency : 66 MHz
//The configuration data is copied to the HUGE buffer, so do not transfer SX2 data before
//FPGA configured

const Pin FPGAnConfigPin = PIN_FPGA_nCONFIG;
const Pin FPGADclkPin = PIN_FPGA_DCLK;
const Pin FPGAData0Pin = PIN_FPGA_DATA0;
const Pin FPGAnStatusPin = PIN_FPGA_nSTATUS;
const Pin FPGAConfDonePin = PIN_FPGA_CONF_DONE;

static Pin Pin_nIRQ_FPGA;
static Pin Pin_FPGA_nXDREQ;
//static const Pin Pin_FPGA_nXDACK = PIN_FPGA_nXDACK;
Pin Pin_FPGA_nHalfEmpty;

extern void * __FPGABin_section_begin();
extern void * __FPGABin_section_end();

///cheney: PIO interrupt is an input changed interrupt.
//  it can't set as Negative edge triggered.
void FPGANotify_Interrupt() 
{
	INT16U status = rFPGA_STATUS; //This status will be cleared after read, so can not read 2 times
	
#if defined(HEAD_RICOH_G4)
	if (status & rFPGA_STATUS_TOO_MUCH)
	{
		status_ReportStatus(STATUS_WAR_FPGA_ULTRADATA, STATUS_SET);
	}
	else if (status & rFPGA_STATUS_TOO_LESS)
		status_ReportStatus(STATUS_WAR_FPGA_LESSDATA, STATUS_SET);
	else if (status & rFPGA_STATUS_ERR_CMD)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGCOMMAND, STATUS_SET);
	else if (status & rFPGA_STATUS_ERR_STAT)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGSTATUS, STATUS_SET);	
	else
		status_ReportStatus(STATUS_ERR_MOTION_SPEED_BAD, STATUS_SET);
#else
#ifndef EPSON_BOTTOM_BOARD_V3
	//Chengliang: 2012/8/30, TATE report this error when printing, even when idle, so ignore this error.
	//if (status&0xF0)
	//    status_ReportStatus(STATUS_ERR_MOTION_SPEED_BAD, STATUS_SET);
	//else 
	if(status&0x2)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGSTATUS, STATUS_SET);	
	else if (status&0x8)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGCOMMAND, STATUS_SET);
	else if (status&0x4)
		status_ReportStatus(STATUS_WAR_FPGA_ULTRADATA, STATUS_SET);
	else if (status&0x1)
		status_ReportStatus(STATUS_WAR_FPGA_LESSDATA, STATUS_SET);
#else
	if (status == 2)
	{
		status_ReportStatus(STATUS_WAR_FPGA_ULTRADATA, STATUS_SET);
	}
	else if (status == 1)
		status_ReportStatus(STATUS_WAR_FPGA_LESSDATA, STATUS_SET);
	else if (status == 3)
		status_ReportStatus(STATUS_ERR_FPGA_WRONGCOMMAND, STATUS_SET);
	else if (status == 4)
		status_ReportStatus(STATUS_ERR_MOTION_SPEED_BAD, STATUS_SET);
	else
		status_ReportStatus(STATUS_ERR_FPGA_WRONGSTATUS, STATUS_SET);	
#endif
#endif		
	//ClearPending(BIT_EINT2);
	//PIO interrupt is cleared auto when read PIO_ISR.
	
#ifndef HEAD_RICOH_G4
	CONSOL_Printf("FPGA err1: Status %d, FireNum %d, ReadNumL %x, H %x, WriteNumL %x, H %x \r\n", 
				  status, rFPGA_DBG_FireNum, rFPGA_DBG_ReadNum_L, 
				  rFPGA_DBG_ReadNum_H, rFPGA_DBG_WriteNum_L, rFPGA_DBG_WriteNum_H);
#endif
}

void FPGADMA_Done_Interrupt(void) 
{
	/*
	INT16U tmp[256];
	INT16U i;
	for (i = 0; i<256; i++)
	tmp[i] = rFPGA_FIFODAT;*/
	INT8U ret = True;
	
	//PIO_Set(&Pin_FPGA_nXDACK);
	
	g_FPGADMA_sendPkg++;
	debug_int1++;
#if defined(HEAD_EPSON_GEN5)
	ret = FPGA_SendData(False, 0);	
#else
	ret = FPGA_SendData(False);	
#endif

	if ((!USBDMA_manualStop)&&(g_USBTransStoped))
	{
		HugeBuf_StartOneReceive();
	}
}

INT8U FPGA_RESET()	
{
	INT16U i, times = 0,err_count = 0, err_count1 = 0;
	volatile INT16U data, fpga_data;	
	Pin nRstPin = PIN_FPGA_nRST;
	
#if defined(HEAD_EPSON_GEN5) //Epson gen5, 8.31M/16.625M, support FAST_BUS
	//Bit 15~13, default 000. Bit 13: X使用的编码器 1/0 = 伺服/光栅; Bit14: Y 编码器, 目前无用
	//Bit 12~8,  Specific use. Refer to FPGA program
	//Bit 7~0,  并转串芯片的工作频率 = FPGA工作频率(48M)/(2*(F+1)). Default = 0x001, 既12M
	
#ifdef EPSON_4H
#ifdef MOTION_Y_DSP
	//if DSP get the right encoder, it will report "光栅出错" for MOTION_Y_DSP case. so, i map a null encoder signal to avoid this error. 
	data = 0x7 | ((headboardInfo.headBoardType & 0x6) << 8) | (0x3 << 13)|(1<<8); 
#else
	if (printer.xEncoder)
		data = 0x7 | ((headboardInfo.headBoardType & 0x6) << 8) | (0x0 << 13)|(1<<8); 
	else	
		data = 0x7 | ((headboardInfo.headBoardType & 0x6) << 8) | (0x1 << 13)|(1<<8); 
#endif
#ifndef SUPPORT_MOTOR_CONTROL
	data = (0x3<<11) | data;
#else
	data = ((~(0x1<<12)) & data) | (0x1<<11);
#endif
	
#else
#ifdef MOTION_Y_DSP
	//if DSP get the right encoder, it will report "光栅出错" for MOTION_Y_DSP case. so, i map a null encoder signal to avoid this error. 
	data = 0x3 | ((headboardInfo.headBoardType & 0x7) << 8) | (0x3 << 13); 
#else
	if (printer.xEncoder)
		data = 0x3 | ((headboardInfo.headBoardType & 0x7) << 8) | (0x0 << 13); 
	else	
		data = 0x3 | ((headboardInfo.headBoardType & 0x7) << 8) | (0x1 << 13); 
#endif
	
#ifndef SUPPORT_MOTOR_CONTROL
	data = (0x1<<12) | data;
#endif
#endif
#elif defined(HEAD_RICOH_G4) //ricoh gen4, 8.31M
	//bit15. read only. config locked flag. Fpga config register only write  one time, then this register is protected and bit 15 is true.
	//bit14. read only. FPGA enable flag. this version is always true.
	//bit13. same as old design. map liner encoder/servo feedback as DSP's X input encoder signal.
	//bit12~8. print data channel map. based on head board type. for ricoh fpga, it is not used and default value is 0.
	//bit7~0. lvds freq's divider. default is 7(about 8.3Mhz). lvds_freq = FPGA_freq/2*(F+1).  
	//#ifndef LIUBO_TEST  
#if defined(RICOH_G4_16H)
	if(headboardInfo.headBoardType != HEAD_BOARD_TYPE_RICOH_GEN4_DA_16H && headboardInfo.headBoardType != HEAD_BOARD_TYPE_RICOH_GEN4_PULSE_16H)
	{
		if (printer.xEncoder)
			data = 0x3 | ((0x2 & 0x1F) << 8) | (0x0 << 13); 
		else	
			data = 0x3 | ((0x2 & 0x1F) << 8) | (0x1 << 13); 
	}
	else
	{
		if (printer.xEncoder)
			data = 0x3 | ((0x2 & 0x1F) << 8) | (0x0 << 13); 
		else
			data = 0x3 | ((0x2 & 0x1F) << 8) | (0x1 << 13); 
	}
#elif defined(RICOH_G5_4H)
	if (printer.xEncoder)
		data = 0x5 | ((0x1 & 0x1F) << 8) | (0x0 << 13); 
	else	
		data = 0x5 | ((0x1 & 0x1F) << 8) | (0x1 << 13); 
#else
	if(headboardInfo.headBoardType != HEAD_BOARD_TYPE_RICOH_GEN4_DA_16H && headboardInfo.headBoardType != HEAD_BOARD_TYPE_RICOH_GEN4_PULSE_16H)
	{
		//#ifndef MOTION_Y_DSP
		if (printer.xEncoder)
			data = 0x7 | ((0x1 & 0x1F) << 8) | (0x0 << 13); 
		else	
			//#endif        
			data = 0x7 | ((0x1 & 0x1F) << 8) | (0x1 << 13); 
	}
	else
	{
		//#ifndef MOTION_Y_DSP  
		if (printer.xEncoder)
			data = 0x7 | ((0x2 & 0x1F) << 8) | (0x0 << 13); 
		else
			//#endif        
			data = 0x7 | ((0x2 & 0x1F) << 8) | (0x1 << 13); 
	}
#endif
	//#else
	//     data = 0x7 | ((0x1 & 0x1F) << 8) | (0x1 << 13);  
	//#endif
#else
#error "only support epson and ricoh"
#endif
	
#if defined(HEAD_EPSON_GEN5)
	//  if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_EPSON_GEN5_2HEAD && factoryData.HeadType == HeadNo_Epson_Gen5 && EPSON_HEADNUM == 1)
	//    data |= 0x8000; 
#endif
	
	do 
	{
		PIO_Clear(&nRstPin);
		for (i=0;i<400;i++);
		PIO_Set(&nRstPin);
		
		OSTimeDly(100);
		
		rFPGA_PINCONF = data;
		
		OSTimeDly(1);
		
#if defined(HEAD_RICOH_G4)    
		//rFPGA_PINCONF = 0;
#else
		rFPGA_PINCONF = 0;
#endif        
		
		OSTimeDly(1);
		
#if defined(HEAD_RICOH_G4)    
		if ((data | rFPGA_PINCONF_LOCKED | rFPGA_PINCONF_ENBALE)== rFPGA_PINCONF)
#else
			if (data == rFPGA_PINCONF)
#endif        
				break;
		OSTimeDly(10);
		times++;	
	}while (times < 20);
	//}while (times < 60000);
	if (times >= 20)
	{
		status_ReportStatus(STATUS_FTA_INTERNAL_WRONGPINCONF, STATUS_SET);
		return False;
	}
	else
		return True;
}
#if defined(HEAD_EPSON_GEN5)  
INT8U FPGA_RESETAGAIN()
{
	INT16U i, times = 0,err_count = 0, err_count1 = 0;
	volatile INT16U data, fpga_data;	
	Pin nRstPin = PIN_FPGA_nRST;
	
#ifdef MOTION_Y_DSP
	//if DSP get the right encoder, it will report "光栅出错" for MOTION_Y_DSP case. so, i map a null encoder signal to avoid this error. 
	data = 0x7 | ((headboardInfo.headBoardType & 0x7) << 8) | (0x3 << 13); 
#else
	if (printer.xEncoder)
		data = 0x7 | ((headboardInfo.headBoardType & 0x7) << 8) | (0x0 << 13); 
	else	
		data = 0x7| ((headboardInfo.headBoardType & 0x7) << 8) | (0x1 << 13); 
#endif
	
#ifndef SUPPORT_MOTOR_CONTROL
	data = (0x3<<11) | data;
#else
	data = ((~(0x1<<12)) & data) | (0x1<<11);
#endif
	do 
	{
		PIO_Clear(&nRstPin);
		for (i=0;i<400;i++);
		PIO_Set(&nRstPin);
		
		OSTimeDly(100);
		
		rFPGA_PINCONF = data;
		
		OSTimeDly(1);
		
#if defined(HEAD_RICOH_G4)    
		//rFPGA_PINCONF = 0;
#else
		rFPGA_PINCONF = 0;
#endif        
		
		OSTimeDly(1);
		
		if (data == rFPGA_PINCONF)      
			break;
		OSTimeDly(10);
		times++;	
	}while (times < 20);
	if (times >= 20)
	{
		status_ReportStatus(STATUS_FTA_INTERNAL_WRONGPINCONF, STATUS_SET);
		return False;
	}
	else
		return True;
}
#endif

#ifdef HEAD_EPSON_GEN5	
#define FPGA_FLASH_DELAY_TIME 45
void FPGA_START_FLASH(INT16U interval_us, INT8U HeadMask)
{
	INT16U mask;
	if(interval_us < 75)
		interval_us = 75;
	interval_us -= FPGA_FLASH_DELAY_TIME;
	if(interval_us > 0x3FF)
		interval_us = 0x3FF;
#ifndef EPSON_BOTTOM_BOARD_V3		
	mask = (HeadMask & 0xF)<<8;
	rFPGA_COMMAND = (0x1000 | mask | ((interval_us/4)&0xFF));
#else
	mask = (HeadMask & 0x3)<<10;
	rFPGA_COMMAND = (0x1000 | mask | (interval_us&0x3FF));
#endif	
}

void FPGA_START_FLASH_ALLWIN(INT16U interval_us, INT8U HeadMask)
{
	INT16U mask;
	if(interval_us > FPGA_FLASH_DELAY_TIME)		
		interval_us -= FPGA_FLASH_DELAY_TIME;
	
	if(interval_us < 100)
		interval_us = 100;
	else if(interval_us >1000)
		interval_us = 1000;
	
#ifndef EPSON_BOTTOM_BOARD_V3		
	mask = (HeadMask & 0xF)<<8;
	rFPGA_COMMAND = (rFPGA_COMMAND_BEGIN_FLASH | mask | ((interval_us/4)&0xFF));
#else
	mask = (HeadMask & 0x3)<<10;
	rFPGA_COMMAND = (rFPGA_COMMAND_BEGIN_FLASH | mask | (interval_us&0x3FF));
#endif	
}

void FPGA_STOP_FLASH()
{
	rFPGA_COMMAND = 0x0110;
}

void FPGA_STOP_FLASH_Safe()
{
	rFPGA_COMMAND = 0x0110;
	OSTimeDly(10);
	FPGA_START_FLASH(200, 0);
	OSTimeDly(10);
	FPGA_STOP_FLASH();
}
#elif defined(HEAD_RICOH_G4)

#ifndef CONVERSION_BOARD //fpga资源不够 命令被删除
void FPGA_START_RICOH_AUTO_C()
{
	rFPGA_COMMAND = rFPGA_COMMAND_BGN_AUTO_C_1;
}
#endif

void FPGA_START_FLASH_RICOH(INT8U DataMask)
{
	INT8U flash_level = 0;
	if(0 <= cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval
	   &&cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval <= 5)
		flash_level = cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval; 
	else
		flash_level = 0;
#ifdef CONVERSION_BOARD 	
	rFPGA_COMMAND = rFPGA_COMMAND_FLASH_HIGH_BIT | 0xFF;
#endif
	if(DataMask != 0)
		rFPGA_COMMAND = rFPGA_COMMAND_BEGIN_FLASH | 0xFF |(flash_level << 8);
	else
		rFPGA_COMMAND = rFPGA_COMMAND_BEGIN_FLASH;
}

void FPGA_STOP_FLASH()
{
	rFPGA_COMMAND = rFPGA_COMMAND_END_FLASH;
}

#endif

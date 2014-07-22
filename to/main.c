/**************************************************************************
 *                                                                         *
 *   PROJECT     : ARM port for uC/OS-II                                   *
 *                                                                         *
 *   MODULE      : EX1.c                                                   *
 *                                                                         *
 *   AUTHOR      : Michael Anburaj                                         *
 *                 URL  : http://geocities.com/michaelanburaj/             *
 *                 EMAIL: michaelanburaj@hotmail.com                       *
 *                                                                         *
 *   PROCESSOR   : Any                                                     *
 *                                                                         *
 *   TOOL-CHAIN  : Any                                                     *
 *                                                                         *
 *   DESCRIPTION :                                                         *
 *   This is a sample code (Ex1) to test uC/OS-II.                         *
 *                                                                         *
 **************************************************************************/

/*
 *********************************************************************************************************
 *                                                uC/OS-II
 *                                          The Real-Time Kernel
 *
 *                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
 *                                           All Rights Reserved
 *
 *                                               EXAMPLE #1
 *********************************************************************************************************
 */
#include "intrinsics.h"
#include "os_includes.h"
#include "consol.h"
#include "flash.h"
#include "tasks.h"
#include "FPGA.h"
#include "miscellance.h"
#include "status.h"
#include "hugebuf.h"
#include "keylcd.h"
#include "key.h"
#include "global.h"
#include "control.h"
#ifdef UV_PRINTER
#include "uv.h"
#endif
#include "../LCD_GUI/resources.h"
#include "frmwrk.h"
extern volatile INT8U nextband_autoClean;
#if EPSON_CLEAN_UPDOWN
#include "math.h"            
#endif
#include <board_memories.h>

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#include "calibration.h"
#include "display.h"
#include "../LCD_GUI/Menu_GUI.h"
#include "CommPipe.h"
#include "usb.h"
#endif
#if defined( SUPPORT_MOTOR_CONTROL)|| defined( SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#include "motion.h"
#endif

#include "onewire.h"

//#define TEST_INK
//#define DEBUG_HB_RESET

//#define DEBUG_NO_HEADBOARD True

//////////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_MOTION
#define DEBUG_NO_HEADBOARD True
#define DEBUG_NO_DSP True
#define DUMP_DSPCOM 1
#else
//	#define DEBUG_NO_HEADBOARD True
//	#define DEBUG_NO_DSP True
//	#define DUMP_HEADCOM 1
//	#define DUMP_DSPCOM 1
#endif

#ifdef VENDOR_PASSWORD_TOOL
#define DEBUG_NO_HEADBOARD True
#endif

//#define DEBUG_NO_HEADBOARD True

#ifdef EPSON_4H
#define HEAD_ERROR_REPORT_MAX_NUM 8
#endif
//////////////////////////////////////////////////////////////////////////////////////

/* ********************************************************************* */
/* Global definitions */

/* ********************************************************************* */
/* File local definitions */
#pragma data_alignment=8
OS_STK  TaskStartStk[START_TASK_STK_SIZE] FAST_DATA_SECTION;
OS_STK 	TaskUsbStk[USB_TASK_STK_SIZE] FAST_DATA_SECTION;      /* USB Tasks stack */
OS_STK	TaskStatusStk[STATUS_TASK_STK_SIZE] FAST_DATA_SECTION;    
OS_STK  TaskParserStk[PARSER_TASK_STK_SIZE] FAST_DATA_SECTION;
OS_STK  TaskControlStk[CONTROL_TASK_STK_SIZE] FAST_DATA_SECTION;
OS_STK  TaskCleanStk[CLEAN_TASK_STK_SIZE] FAST_DATA_SECTION;
OS_STK  TaskKeyboardStk[KEYBOARD_TASK_STK_SIZE] FAST_DATA_SECTION;
OS_STK  TaskUpdateStk[UPDATE_TASK_STK_SIZE] FAST_DATA_SECTION;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
OS_STK  TaskCaliStk[CALI_TASK_STK_SIZE] FAST_DATA_SECTION;
#endif
#if (defined(SUPPORT_MOTOR_CONTROL))|| defined (SUPPORT_MOTOR_CONTROL_ONLY_STEP)
OS_STK  TaskMotionStk[MOTION_TASK_STK_SIZE] FAST_DATA_SECTION;
#endif    
#pragma data_alignment=4

char     TaskData[NO_TASKS];                    /* Parameters to pass to each task */

volatile INT8S kb_installed = False;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
static INT8S calibration_ready = False;
INT8U MediaMeasured = False;
#endif
volatile INT8U valueFlag_FactoryData_Not_Init = 0;
volatile INT8U valueFlag_BYHXData_Not_Init = 0;
extern volatile INT8U valFlag_GetPos;  //Value Flag, define in SX2.c
INT8U Already_Reset_FPGA = FALSE;
INT8U global_findPrintHeadInCategoryList(INT8U category, INT8U headType);
INT8U vol_CalHeadVoltage(INT8U headNum);
INT8U vol_MapHeadNumber(INT8U hd_HeadNum, INT8U isTamparature, INT8U isHeater, INT8U isHeaterThermistorTemp, INT8U isPulseWidth);
void headboardSwitchL40M14(void);
volatile ElapseTimeType elapsedTime;
volatile INT8U motionInitS2 = False;

INT8U SetMeasureResult(	INT32S paper_X_NearEdgePos, INT32S paper_X_FarEdgePos);
void Initial_Dis();
void UpdateHeadParamName();

INT8U eepromWrtFailTimes = 0;

#ifdef VENDOR_PASSWORD_TOOL
extern INT8U desKey[];
#endif

#ifdef WAVE_SET_FUN
INT16U DATA_LEN = 0;
INT8U TRANSFER_MODE = 0;
INT8U WAVE_BUF[600] = {0};
INT8U WAVE_NAME_BUF[4 * 8 * 8] = {0};		//一共4个vsd，每个vsd包含8个波形，即8个名字，每个名字最多占8个字节（有效7个字节 + 0位）共256个字节
INT8U WAVE_MAP_TABLE[MAX_HEAD_DRIVE_NUM * 2] = {0};			//长度为头板通道个数 * 2
#endif

#if defined(FUNC_WAVE_MAP)&&!defined(WAVE_SET_FUN)
INT16U DATA_LEN = 0;
INT8U TRANSFER_MODE = 0;
INT8U WAVE_NAME_BUF[4 * 8 * 8] = {0};		//一共4个vsd，每个vsd包含8个波形，即8个名字，每个名字最多占8个字节（有效7个字节 + 0位）共256个字节
/*波形映射表结构：WAVE_MAP_TABLE
 *是一个短整型数组，长度为头板通道个数，按顺序存储通道对应的波形          
 *短整型共16位，
 *0~3位表示VSD1对应波形序号
 *4~7位表示VSD2对应波形序号
 *8~11位表示VSD3对应波形序号
 *12~15位表示VSD4对应波形序号
 */
INT8U WAVE_MAP_TABLE[MAX_HEAD_DRIVE_NUM * 2] = {0};			//长度为头板通道个数 * 2 
#endif

extern const Pin LVDSLockPin;
extern struct EPR_MOTION_PARAM motionParam;
INT32S j; //only for testing FPGA coor system
extern INT8U OpenPlateFanAlways();
#if defined(ALLWIN_MEDIA_KEY)
const Pin CLEAN_STATUS_LED = PIN_HEADBOARD_OUT;
#elif defined(MANUFACTURER_TATE_EPSON_UV)&&defined(RIPSTAR_FLAT_EX)
INT8U IS_STOP = False;
const Pin TATE_FLAT_STOP_MOVE = PIN_TATE_FLAT_STOP_MOVE;
#elif defined(HUMAN_Y_STATUS_OUTPUT)
const Pin Y_STATUS_OUTPUT = PIN_HEADBOARD_OUT;
const Pin Y_STATUS_INPUT = PIN_DSP_UV_CONTROL_2;
//const Pin X_STATUS_INPUT = PIN_DSP_UV_CONTROL_1;
INT8U Y_MOVE_STOP = False;
void HUMAN_Y_STATUS_UP_Interrupt(const Pin * pin)
{
	if(PIO_Get(&Y_STATUS_INPUT))
	{
		PIO_Set(&Y_STATUS_OUTPUT);
	}
	else
	{
		Y_MOVE_STOP = True;
	}
}
#if 0
void HUMAN_Y_STATUS_DOWN_Interrupt(const Pin * pin)
{
	if(PIO_Get(&X_STATUS_INPUT))
	{
		Y_MOVE_START = True;
	}
}
#endif
#endif
#if defined(HEAD_RICOH_G4)
INT8U Hardmap_to_PM_map[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={0} ;

INT8U Hardmap_to_PM_map_8C1G2G[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={6,7,14,15,4,5,12,13,2,3,10,11,0,1,8,9,} ;
INT8U Hardmap_to_PM_map_8C1G2G_NEG[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={6,7,14,15,4,5,12,13,2,3,10,11,0,1,8,9,} ;

INT8U Hardmap_to_PM_map_3H_6C4C1G[6] ={4,5,2,3,0,1} ;
INT8U Hardmap_to_PM_map_3H_G5[] ={2,1,0};

INT8U Hardmap_to_PM_map_4H_G5_4C[] ={0,2,3,1};
INT8U Hardmap_to_PM_map_4H_G5_8C[] ={0,1,3,2};
extern INT8U RICOH_ChannelMap[];
void Init_To_Pm_Map( void )
{
	if (headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H)
	{
		if(factoryData.m_fHeadXColorSpace < 0)
			memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_8C1G2G_NEG, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
		else
			memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_8C1G2G, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
	}
	else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H)
	{
		memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_3H_6C4C1G, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
		memcpy(EpsonChannelMap,  RICOH_ChannelMap,MAX_EPSON_HEAD_CHANNEL);
	}
	else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H)
	{
		memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_3H_G5, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
		memcpy(EpsonChannelMap,  RICOH_ChannelMap,MAX_EPSON_HEAD_CHANNEL);
	}
	else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H)
	{
		if(COLOR_NUMBER_CALIBRATION > 4)
		{
			memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_4H_G5_8C, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
			memcpy(EpsonChannelMap,  Hardmap_to_PM_map_4H_G5_8C,MAX_EPSON_HEAD_CHANNEL);
		}
		else
		{
			memcpy(Hardmap_to_PM_map, Hardmap_to_PM_map_4H_G5_4C, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);
			memcpy(EpsonChannelMap,  Hardmap_to_PM_map_4H_G5_4C,MAX_EPSON_HEAD_CHANNEL);
		}
	}
}
#endif
#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP 
INT8U MOTION_ZC_INIT_OK = False;
#endif
#if defined(HEAD_RICOH_G4)||defined(EPSON_FLASH_IDLE) 
INT8U flash_idle_on = False;
INT8U flash_idle_send_jobstart = False;
INT8U flash_wait_50ms = 0;
void False_Idle_Off(void)
{
	INT8U buf[3]; 
	//cleanPara.flash = False;
#ifdef EPSON_FLASH_IDLE
	if(flash_idle_on&&((rFPGA_COMMAND&0xF000) == rFPGA_COMMAND_BEGIN_FLASH_IDLE)||(!flash_idle_on&&flash_idle_send_jobstart))
#else
		if((flash_idle_on&&(rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_FLASH | 0xFF))||(!flash_idle_on&&flash_idle_send_jobstart))
#endif
		{		
			if(flash_idle_on)
			{
				FPGA_STOP_FLASH_Safe();
			}
			if(flash_idle_send_jobstart)
			{
				buf[0] = 2;
				buf[1] = UART_HEAD_EPSON_JOBEND;
				while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
					OSTimeDly(10);
				flash_wait_50ms = 0;
			}
			else
			{
				OSTimeDly(1);
#ifdef HEAD_RICOH_G4		   
#ifndef CLOSE_SSHAKE    		   
				SetSafeCmd(rFPGA_COMMAND_BGN_SSHAKE);
#endif
#endif
			}
			flash_idle_on = False;
			flash_idle_send_jobstart = False;
		}  
}
#endif	

#ifdef EPSON_FLASH_IDLE
void EPSON_FLASH_IDLE_STAR(INT16U interval_ms, INT8U HeadMask)
{
	INT16U mask;

	if(interval_ms < 4)
		interval_ms = 4;
	else if(interval_ms >1000)
		interval_ms = 1000;

#ifndef EPSON_BOTTOM_BOARD_V3		
	mask = (HeadMask & 0xF)<<8;
	rFPGA_COMMAND = (rFPGA_COMMAND_BEGIN_FLASH_IDLE | mask | ((interval_ms/4)&0xFF));
#else
	mask = (HeadMask & 0x3)<<10;
	rFPGA_COMMAND = (rFPGA_COMMAND_BEGIN_FLASH_IDLE | mask | (interval_ms&0x3FF));
#endif	
}

#endif

#ifdef FULGENCY_FUN
INT8U Y_GOHOME_Dirty = Y_GOHOME_IDLE;
INT8U Head_Pump_Dirty = True;
INT8U Z_SAFE_Dirty = False;
INT8U FIND_ORIGIN_Dirty = False;
void FULGENCY_PRT_Y_GOHOME(void)
{
	INT8U buf[10] = {0},err;
	OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,&err);
	if (err == OS_NO_ERR &&Y_GOHOME_Dirty == Y_GOHOME_CMD)
	{
		Y_GOHOME_Dirty = Y_GOHOME_WAITE;
		status_ReportStatus(STATUS_MOVING, STATUS_SET);
		buf[0] = 8; //Length									
		buf[1] = UART_PAPERFEED_CMD; 
		buf[2] = 0;
		buf[3] = 3;
		*((__packed INT32S *)&buf[4]) = curPos.y;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(1);
	}
}
#endif
#if defined(EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)

#define Move_Deviation 10
extern INT8U Backhaul_Flag;
extern INT8U postCapping;

/***************************************************************************
 * judge the printer nozzle come back, if come back return True, else return 
 * False.
 ***************************************************************************/
static INT8U Judge_Printer_nozzle_come_back(void)
{
	if (absv(curPos.x) < Move_Deviation)
	{
		return True;
	}
	return False;
}    

/***************************************************************************
 * send second backhaul command in low speed, this function can be call when  
 * printer nozzle not come back. if the printer nozzle not come back, this function 
 * can be called second time, if the printer nozzle not come back at the second 
 * time, this function not be called even the printer nozzler not come back.
 ***************************************************************************/
INT8U Send_Second_Backhaul(void)
{
	INT8U buf[7];

	buf[0] = 7;                          //Command Length									
	buf[1] = UART_MOVETO_CMD;            //uart command
	*((__packed INT32S *)&buf[2]) = 0;   //printer move distance
	buf[6] = 7;                          //printer move Speed

	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
	{
		OSTimeDly(10);
	}

	return True;
}

#endif
void TaskStart (void *data)
{
	OS_CPU_SR cpu_sr;

	//char key;
	//INT8U keys[20];
	INT8U err;
	INT8U UartCMD[32]; 
	INT8U motionInitS1 = False;
	INT8U headInitS1 = False;
	INT8U headInitS2 = False;

	INT32U cur_elapsedTime;
	INT32U old_elapsedTime;


	INT8U usbInit = False;
	INT8U FPGAInit = False;
	INT32U i;

	INT8U securityChipInit = False;
	//		INT32U volatile ttPos = 0; //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	INT8U buf[64];

	INT16S retryCnt = 50;

	INT16U coreLEDFlashDly = 0;
	INT8U len;
	INT16U tmpu;
	INT16S tmps;
	INT16U addr;
#ifdef ALLWIN_MEDIA_KEY
	INT8U no_clean = False;
#endif		

#ifdef BYHX_TEST_BOARD
	CONSOL_Printf("Enter Start Task\n");
#endif		
	OS_CSP_TickInit();	/* Initialize the Tick interrupt */
	OSStatInit();		/* Initialize uC/OS's statistics */

	Global_Init();

	device_init();	/*XXX*/

	BELL_OFF();


	/*FIXME*/
	securityChipInit = Init_OneWire();

#ifdef UV_PRINTER		
	uv_Init();
#endif

	mix_FLAG_GRP = OSFlagCreate(0x0000, &err); //USB Communication Flags      	
	mix_FLAG_GRP_2 = OSFlagCreate(0x0000, &err); //USB Communication Flags      

	status_Init();  
#if defined(MICOLOR_AUTOFUCTION) &&  defined(EPSON_BOTTOM_BOARD_V3)
	OpenPlateFanAlways();
#endif
	kb_installed = KB_Init();
#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
	LCD_Init();
#endif
	Printer_Init();
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	calibration_ready = InitCalibration(); //Initial_Dis() need InitCalibration()
#endif
#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
	Initial_Dis();
	//OSTaskCreate(Keyboard_Task, (void *)0, (void *)&TaskKeyboardStk[KEYBOARD_TASK_STK_SIZE - 1], KEYBOARD_TASK_PRIO);
	(void)OSTaskCreateExt(Keyboard_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskKeyboardStk[KEYBOARD_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			KEYBOARD_TASK_PRIO,                            
			KEYBOARD_TASK_ID,
			&TaskKeyboardStk[0],                         /* Set Bottom-Of-Stack                  */
			KEYBOARD_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(KEYBOARD_TASK_PRIO, (INT8U *)"Keyboard", &err);
#endif

	//OSTaskCreate(status_Task, (void *)0, (void *)&TaskStatusStk[STATUS_TASK_STK_SIZE - 1], STATUS_TASK_PRIO);
	(void)OSTaskCreateExt(status_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskStatusStk[STATUS_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			STATUS_TASK_PRIO,                            
			STATUS_TASK_ID,
			&TaskStatusStk[0],                         /* Set Bottom-Of-Stack                  */
			STATUS_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(STATUS_TASK_PRIO, (INT8U *)"Status", &err);

#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1) 
	Heater_Init();
#endif
	//OSTaskCreate(Control_Task, (void *)0, (void *)&TaskControlStk[CONTROL_TASK_STK_SIZE - 1], CONTROL_TASK_PRIO);
#ifndef DEBUG_NO_HEADBOARD
	(void)OSTaskCreateExt(Control_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskControlStk[CONTROL_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			CONTROL_TASK_PRIO,                            
			CONTROL_TASK_ID,
			&TaskControlStk[0],                         /* Set Bottom-Of-Stack                  */
			CONTROL_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(CONTROL_TASK_PRIO, (INT8U *)"Control", &err);
#endif       

#if ((!defined(BYHX_WTITE_BOARD_TOOL))&&(!defined(VENDOR_PASSWORD_TOOL)))		
#if 0
	FPGAInit = FPGA_Cfg();
	if (FPGAInit)
	{
		FPGAInit = FPGA_Init(); 			
	}		
#endif
#endif

	usbInit = USB_Init();
	if (usbInit)
	{
		//OSTaskCreate(USB_Task, (void *)0, (void *)&TaskUsbStk[USB_TASK_STK_SIZE - 1], USB_TASK_PRIO);
		(void)OSTaskCreateExt(USB_Task,
				(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
				&TaskUsbStk[USB_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
				USB_TASK_PRIO,                            
				USB_TASK_ID,
				&TaskUsbStk[0],                         /* Set Bottom-Of-Stack                  */
				USB_TASK_STK_SIZE,
				(void *)0,                                 /* No TCB extension                     */
				OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
		OSTaskNameSet(USB_TASK_PRIO, (INT8U *)"USB", &err);
	}
	//OSTimeDly(1000);

	if(!securityChipInit)    
		status_ReportStatus(STATUS_SVC_READBOARDID_FAIL, STATUS_SET);        

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	if( usbInit)
		InitCommPipe(3); //total 3 channel: Headboard, DSP, and ARM itself.

#if defined( EPSON_LCD)
	if (kb_installed)
		UI_DisplayLED(E_LI_POWERON, E_LS_FLASH);
#endif

#endif

#if DEBUG_NO_HEADBOARD
	UART_Init(False);
#elif defined(EPSON_BOTTOM_BOARD_V3)	
	UART_Init(True);  //Initial UART 1 and UART2
#else
	retryCnt = 50; 
	while (retryCnt--)
	{
		if (PIO_Get(&LVDSLockPin) == 1) //LVDS 124 is OK
		{
			UART_Init(True);  //Initial UART 1 and UART2
			break;
		}
		else
		{
			UART_Init(False);  //Only init UART2
			OSTimeDly(1);
		}
	}

	if (retryCnt <= 0)
		status_ReportStatus(STATUS_FTA_HEADTOMAINROAD, STATUS_SET);        
#endif        

	// Cheney: move to KB_Init
	//#ifndef DEBUG_NO_HEADBOARD  
	//        BIT_CLR(rEINTMASK, BIT_EINT_EINT16); //Enable LVDS lock signal interrupt
	//#endif        

#ifdef BYHX_WTITE_BOARD_TOOL
	//cheney: because the following e2prom action is very slow, 
	//it will make valueFlag_BYHXData_Not_Init/valueFlag_FactoryData_Not_Init is invalid before check action. 
	//it will cause that PM get the error factory data. 
	valueFlag_BYHXData_Not_Init = 1;
	valueFlag_FactoryData_Not_Init = 1;
	{
		INT8U * BPP =  (INT8U *)(AT91C_DDR2 + 0x200000);
		INT8U * BPP2 = (INT8U *)(AT91C_DDR2 + 0x300000);

		INT8U * PP;
		INT32U total = 0;
		INT32U EEPROM_PAGE_LEN = E2PROM_PAGESIZE, EEPROM_len;
		EEPROM_len = 32*1024/EEPROM_PAGE_LEN;

		OSSemPend(IICSem, 0, &err);

		PP = BPP;

		for ( i = 0; i < EEPROM_len * EEPROM_PAGE_LEN; i++)
		{
			BPP[i] = (INT8U)i;
			BPP2[i] = 0;
		}

		total = 0;
		for ( i = 0; i < EEPROM_len; i++)
		{
			len = EEPROM_PAGE_LEN;
			if (IIC_WriteEEPRom(total, PP, &len) == False)
			{
				total += len;
				break;
			}

			PP += EEPROM_PAGE_LEN;
			total += EEPROM_PAGE_LEN;
		}

		PP = BPP2;

		total = 0;
		for ( i = 0; i < EEPROM_len; i++)
		{
			len = EEPROM_PAGE_LEN;
			if (IIC_ReadEEPRom(total, PP, &len) == False)
			{
				total += len;
				break;
			}
			PP += EEPROM_PAGE_LEN;
			total += EEPROM_PAGE_LEN;
		}

		for ( i = 0; i < EEPROM_len * EEPROM_PAGE_LEN; i++)
		{
			if (BPP[i] != BPP2[i])
			{
				status_ReportStatus(STATUS_SVC_EEPROM_CHK, STATUS_SET);	
				CONSOL_Printf("CHECK EEPROM Failed 1\r\n");
				break;
			}
		}

		PP = BPP;

		for ( i = 0; i < EEPROM_len * EEPROM_PAGE_LEN; i++)
		{
			BPP[i] = 0xFF;
			BPP2[i] = 0;
		}

		total = 0;
		for ( i = 0; i < EEPROM_len; i++)
		{
			len = EEPROM_PAGE_LEN;
			if (IIC_WriteEEPRom(total, PP, &len) == False)
			{
				total += len;
				break;
			}
			PP += EEPROM_PAGE_LEN;
			total += EEPROM_PAGE_LEN;
		}

		PP = BPP2;

		total = 0;
		for ( i = 0; i < EEPROM_len; i++)
		{
			len = EEPROM_PAGE_LEN;
			if (IIC_ReadEEPRom(total, PP, &len) == False)
			{
				total += len;
				break;
			}
			PP += EEPROM_PAGE_LEN;
			total += EEPROM_PAGE_LEN;
		}

		for ( i = 0; i < EEPROM_len * EEPROM_PAGE_LEN; i++)
		{
			if (BPP2[i] != 0xFF)
			{
				status_ReportStatus(STATUS_SVC_EEPROM_CHK, STATUS_SET);	
				CONSOL_Printf("CHECK EEPROM Failed 2\r\n");
				break;
			}				
		}
		OSSemPost(IICSem);
		//		CONSOL_Printf("CHECK EEPROM Finished\r\n");
	}		
#endif

	//BYHX初始化检测
#ifdef CHECK_BYHX_DATA
	len = EPR_BYHX_AUTHORITY_FLAG_SIZE;
	OSSemPend(IICSem, 0, &err);
	if (IIC_ReadEEPRom(EPR_BYHX_AUTHORITY_FLAG_OFFSET, buf, &len) == False)
	{
		OSSemPost(IICSem);
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
		//			while (1)	OSTimeDly(100);	
	}
	OSSemPost(IICSem);
	if (memcmp(buf, byhxAuthorityStr, EPR_BYHX_AUTHORITY_FLAG_SIZE)) //!=
	{
		valueFlag_BYHXData_Not_Init = 1;
		status_ReportStatus(STATUS_SVC_BYHX_DATA, STATUS_SET);
		//			while (1)	OSTimeDly(100);	
	}
#else
	valueFlag_BYHXData_Not_Init = 1;
#endif		

	//FACTORY初始化检测
	len = EPR_FACTORY_AUTHORITY_FLAG_SIZE;
	OSSemPend(IICSem, 0, &err);
	if (IIC_ReadEEPRom(EPR_FACTORY_AUTHORITY_FLAG_OFFSET, buf, &len) == False)
	{
		OSSemPost(IICSem);
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
		//			while (1)	OSTimeDly(100);	
	}		
	OSSemPost(IICSem);

	//		for (i = 0 ; i < EPR_FACTORY_AUTHORITY_FLAG_SIZE; i++)
	//		{
	//			CONSOL_Printf(":%x", buf[i]);
	//		}
	if (memcmp(buf, factoryAuthorityStr,EPR_FACTORY_AUTHORITY_FLAG_SIZE)) //!=
	{
		valueFlag_FactoryData_Not_Init = 1;
		status_ReportStatus(STATUS_FTA_FACTORY_DATA, STATUS_SET);
		//			while (1)	OSTimeDly(100);	
	} 

#ifndef BYHX_WTITE_BOARD_TOOL		
	//Limited time initial
	elapsedTime.dirty = False;
	len = EPR_LIMIT_TIME_SIZE;
	OSSemPend(IICSem, 0, &err);
	if (IIC_ReadEEPRom(EPR_LIMIT_TIME_OFFSET, (INT8U *)&elapsedTime, &len) == False)
	{
		OSSemPost(IICSem);
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
	}		
	OSSemPost(IICSem);

	if (elapsedTime.flag != 0)
	{
		elapsedTime.flag = 0;
		elapsedTime.time_elapse = 0;
		elapsedTime.dirty = True;
	}

	//used ink volume initial
	usedInkVolume.dirty = False;
	i = 0;
	len = sizeof(usedInkVolume);
	while (i < len)
	{
		INT8U length;
		if (len - i > 64)
			length = 64;
		else
			length = len - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_USED_INK_VOLUME_OFFSET+i, ((INT8U *)&usedInkVolume)+i, &length) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += length;			
	}

	if (i < len)
	{
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
	}		

	if (usedInkVolume.flag != 0x12345678)
	{
		memset((void*)&usedInkVolume, 0, sizeof(usedInkVolume));
		usedInkVolume.flag = 0x12345678;
		usedInkVolume.dirty = True;
		usedInkVolume.DotVolume = GetDotVolume(factoryData.HeadType);
		usedInkVolume.CurrentTotalInk = 0;

		len = sizeof(usedInkVolume);
		OSSemPend(IICSem, 0, &err);
		if (IIC_WriteEEPRom(EPR_USED_INK_VOLUME_OFFSET, (INT8U *)&usedInkVolume, &len) == False)
			eepromWrtFailTimes = 1;
		else
			eepromWrtFailTimes = 0;
		OSSemPost(IICSem);
		if (eepromWrtFailTimes > 0)	
			status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
	}
	else
	{	
		usedInkVolume.DotVolume = GetDotVolume(factoryData.HeadType);
		usedInkVolume.CurrentTotalInk = 0;
		for(i= 0; i< MAX_COLOR_NUMBER; i++)
		{
			usedInkVolume.CurrentTotalInk += usedInkVolume.InkVolume[i][1];
		}
	}
#endif		

#ifndef BYHX_WTITE_BOARD_TOOL								 
	//Check password
	if (!security_CheckTimePassword())
		status_ReportStatus(STATUS_WAR_TIME_PASSWORD_WRONGINPUT, STATUS_CLR);
	if (!security_CheckOptionPassword())
	{
		status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_WRONGINPUT, STATUS_CLR);
	}
	else
	{
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		if(password.m_bHaveOptionPwd)
		{
			if( LCDMenuConfig.LanguageIndex != password.m_bLanguage)
				LCDMenuConfig.LanguageIndex = password.m_bLanguage;
		}
#endif
	}
	if (!security_CheckInkPassword())
		status_ReportStatus(STATUS_WAR_INK_PASSWORD_WRONGINPUT, STATUS_CLR);
#endif

#if defined(SUPPORT_MOTOR_CONTROL)|| defined (SUPPORT_MOTOR_CONTROL_ONLY_STEP)
	if(FPGAInit)
	{   
		if(Motion_Init())
		{
			//OSTaskCreate(Motion_Task, (void *)0, (void *)&TaskMotionStk[MOTION_TASK_STK_SIZE-1], MOTION_TASK_PRIO);
			(void)OSTaskCreateExt(Motion_Task,
					(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
					&TaskMotionStk[MOTION_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
					MOTION_TASK_PRIO,                            
					MOTION_TASK_ID,
					&TaskMotionStk[0],                         /* Set Bottom-Of-Stack                  */
					MOTION_TASK_STK_SIZE,
					(void *)0,                                 /* No TCB extension                     */
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
			OSTaskNameSet(MOTION_TASK_PRIO, (INT8U *)"Motion", &err);
		}
		else
			status_ReportStatus(STATUS_SVC_ARM_MOTION, STATUS_SET);
	}
#endif    

	//Start parsing task here
	//OSTaskCreate(Parser_Task, (void *)0, (void *)&TaskParserStk[PARSER_TASK_STK_SIZE-1], PARSER_TASK_PRIO);
	(void)OSTaskCreateExt(Parser_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskParserStk[PARSER_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			PARSER_TASK_PRIO,                            
			PARSER_TASK_ID,
			&TaskParserStk[0],                         /* Set Bottom-Of-Stack                  */
			PARSER_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(PARSER_TASK_PRIO, (INT8U *)"Parser", &err);
	//OSTaskCreate(Clean_Task, (void *)0, (void *)&TaskCleanStk[CLEAN_TASK_STK_SIZE-1], CLEAN_TASK_PRIO);
	(void)OSTaskCreateExt(Clean_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskCleanStk[CLEAN_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			CLEAN_TASK_PRIO,                            
			CLEAN_TASK_ID,
			&TaskCleanStk[0],                         /* Set Bottom-Of-Stack                  */
			CLEAN_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(CLEAN_TASK_PRIO, (INT8U *)"Clean", &err);

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	if(calibration_ready)
	{
		//OSTaskCreate(Cali_Task, (void *)0, (void *)&TaskCaliStk[CALI_TASK_STK_SIZE-1], CALI_TASK_PRIO);
		(void)OSTaskCreateExt(Cali_Task,
				(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
				&TaskCaliStk[CALI_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
				CALI_TASK_PRIO,                            
				CALI_TASK_ID,
				&TaskCaliStk[0],                         /* Set Bottom-Of-Stack                  */
				CALI_TASK_STK_SIZE,
				(void *)0,                                 /* No TCB extension                     */
				OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
		OSTaskNameSet(CALI_TASK_PRIO, (INT8U *)"Calibration", &err);
	}
	AdjustPlatSpace();
#endif

#if DEBUG_NO_DSP
	motionInitS1 = True;
#endif        

#if DEBUG_NO_HEADBOARD
	headInitS1 = True;
	headboardInfo.headCategory = HEAD_CATEGORY_RICOH;
	headboardInfo.headBoardType = HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H;    		    				
#if ((!defined(BYHX_WTITE_BOARD_TOOL))&&(!defined(VENDOR_PASSWORD_TOOL)))
	FPGA_RESET();
#endif		
#endif     
#if defined(DYSS_UV_CLEAN)
	DYSS_CLEAN_TURN_MOTOR_INIT();
	DYSS_CLEAN_MOVEING_MOTOR_INIT();
#endif				  
	//Waiting Head board and Motion initial first stage

	retryCnt = 1000;

	while (retryCnt--)
	{
		if (headInitS1 && (!motionInitS1)&&UART_GetCMD(UART_MOTION_CHANNEL, UartCMD)) //Motion stage 1 initial OK
		{        		
			if ((UartCMD[0] == 6 || (UartCMD[0] == 7 && UartCMD[6] == 2)) & (UartCMD[1] == UART_INIT_STAGE1_CMD)) //Command motion start stage 2 initial
			{
				if(UartCMD[0] == 7 && UartCMD[6] == 2)
					UART_SetCheckModel(UART_MOTION_CHANNEL, 2);
				else
					UART_SetCheckModel(UART_MOTION_CHANNEL, 1);
				if ((!valueFlag_FactoryData_Not_Init)&&(!valueFlag_BYHXData_Not_Init))
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_SET_FACTORY_DATA_CMD;                          
					UartCMD[2] = factoryData.HeadType;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, UartCMD))
						OSTimeDly(10);

					UartCMD[0] = 4;
					UartCMD[1] = UART_BEGIN_STAGE2_CMD;
					UartCMD[2] = printer.xEncoder; //!0 使用光栅， 0 使用司服编码        			
					UartCMD[3] = factoryData.paper_w_type;

					while (!UART_SendCMD(UART_MOTION_CHANNEL, UartCMD))
						OSTimeDly(10);
				}        					
				motionInitS1 = True;
			}        		
		}

		if ((!headInitS1)&&UART_GetCMD(UART_HEAD_CHANNEL, UartCMD)) //Head board stage 1 initial OK 	
		{
			switch (UartCMD[1])
			{
			case UART_INIT_STAGE1_CMD: //Length is 6 is the old version headboard, length is 8 is the new version

				if(UartCMD[0] == 10)
					Head_Board_Num = UartCMD[--UartCMD[0]];
				else
					Head_Board_Num = 1;

				if ((UartCMD[0] == 6) ||(UartCMD[0] == 8) || (UartCMD[0] == 9 && UartCMD[8] == 2))//Command headboard start stage 1 initial
				{   
					if(UartCMD[0] == 9 && UartCMD[8] == 2)
						UART_SetCheckModel(UART_HEAD_CHANNEL, 2);
					else
						UART_SetCheckModel(UART_HEAD_CHANNEL, 1);
					if (UartCMD[0] == 8 || UartCMD[0] == 9)
					{
						headboardInfo.headCategory = UartCMD[6];
						headboardInfo.headBoardType = UartCMD[7];
					}
					else
					{
						headboardInfo.headCategory = HEAD_CATEGORY_RICOH;
						headboardInfo.headBoardType = HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H;    		    				
					}    		    			

					headInitS1 = True;
#if ((!defined(BYHX_WTITE_BOARD_TOOL))&&(!defined(VENDOR_PASSWORD_TOOL)))        					
					FPGA_RESET();
#endif 
#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
					UpdateHeadParamName();
#endif


				}
				break;
			}
		}
		//if(!headInitS1)
		//	OSTimeDly(10);
		if (motionInitS1&&headInitS1)
			break;
		OSTimeDly(10);
	}


	if (headInitS1 && (!motionInitS1))
		status_ReportStatus(STATUS_FTA_DSPINITS1, STATUS_SET); 

	if (!headInitS1)
		status_ReportStatus(STATUS_FTA_HEADINITS1, STATUS_SET);
	else
	{
		if ((!valueFlag_FactoryData_Not_Init)&&(!valueFlag_BYHXData_Not_Init))
		{
			{
				UartCMD[0] = 2;
				UartCMD[1] = UART_BEGIN_STAGE2_CMD;
#ifdef HEAD_EPSON_GEN7
				UartCMD[0] = 3;
				UartCMD[2] = 2;
#elif defined(HEAD_EPSON_GEN5S)
				UartCMD[0] = 3;
				UartCMD[2] = 3;

#endif
			}

#ifdef CONVERSION_BOARD
			UartCMD[3] = UartCMD[0];
			UartCMD[0] = 4;
#endif				
			if (FPGAInit)
			{
				while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD)) 
					OSTimeDly(1);
			}	        	
		}
	}        			
#if DEBUG_NO_DSP
	status_ReportStatus(STATUS_INI_DSP, STATUS_CLR); 
	motionInitS2 = True;
#endif	    
#if DEBUG_NO_HEADBOARD
	status_ReportStatus(STATUS_INI_HEADBOARD, STATUS_CLR); 
	headInitS2 = True;
#endif 

#if (defined(EPSON_BOTTOM_BOARD_V2) || defined(EPSON_BOTTOM_BOARD_V2_1) || defined(EPSON_BOTTOM_BOARD_V3))
	retryCnt = 3;

#if defined( IIC_Key_Board_LCD)
	while(!kb_installed)
	{   //For EPSON_BOTTOM_BOARD_V2, we found that the signal is very drity when ZLG keyboard chip reset.
		// so we try it again this time after all device is ready.
		kb_installed = KB_Init_Again();
		if(!kb_installed && retryCnt)
		{
			OSTimeDly(100);
			retryCnt --;
			if(retryCnt ==0)
				break;
		}
	}
#endif
#if defined( EPSON_LCD)
	if (kb_installed)
		UI_DisplayLED(E_LI_POWERON, E_LS_ON);
#endif

#endif  

#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
	LCD_SetMainInfoEx(E_IT_PrintOrigin, (void *)&LCDMenuConfig.OriginX);
	LCD_SetMainInfoEx(E_IT_MediaWidth, (void *)&LCDMenuConfig.MediaWidth);
#ifdef RIPSTAR_FLAT_EX		
	LCD_SetMainInfoEx(E_IT_PrintYOrigin, (void *)&LCDMenuConfig_EX.OriginY);
	LCD_SetMainInfoEx(E_IT_MediaLength, (void *)&LCDMenuConfig_EX.MediaL);
	LCD_SetMainInfoEx(E_IT_PrintZOrigin, (void *)&LCDMenuConfig_EX.OriginZ);
#endif
#endif
	status_ReportStatus(STATUS_INI_ARM, STATUS_CLR); //Initializing complete

	if (headInitS2)
	{
		headboardSwitchL40M14();
		for (i=0;i<MAX_VOLTAGENUMBER;i++)
			g_curHeadVoltage[i] = vol_CalHeadVoltage(i);
	}

	if (!epromDataRead)
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);

#ifndef VENDOR_PASSWORD_TOOL			
	//Step1 initializing finished.
	if (!global_findPrintHeadInCategoryList(headboardInfo.headCategory, factoryData.HeadType))
	{
		status_ReportStatus(STATUS_FTA_PRINTHEAD_NOTMATCH, STATUS_SET);
	}
#endif		

	//old_elapsedTime = OSTimeGet()/10;  //ticks, ms, for test
	old_elapsedTime = OSTimeGet()/1000;  //ticks, ms


	//#ifdef SDRAM_SIZE_128M
	//  if (!FRMWRK_vRamTest((INT8U *)"SDRAM Test:", 0x74000000, 0x74100000))
	//    if (!FRMWRK_vRamTest((INT8U *)"SDRAM Test:", 0x74000000, 0x74100000))
	//      status_ReportStatus(STATUS_SVC_SDRAM_CHECK, STATUS_SET);
	//#endif        	        	
	if(BOARD_DDRAM_SIZE < 64 * 1024 *1024)
		status_ReportStatus(STATUS_SVC_SDRAM_CHECK, STATUS_SET);
	if(IsNeed128M() && BOARD_DDRAM_SIZE != 128 * 1024 *1024)
		status_ReportStatus(STATUS_SVC_SDRAM_NOMATCH, STATUS_SET);

#ifdef INK_PASSWORD
	CheckInkVolume();
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	if(!calibration_ready)
		status_ReportStatus(STATUS_FTA_CALIBRATION_DATA, STATUS_SET);
#endif

#if defined(HEAD_RICOH_G4)
	Init_To_Pm_Map();
#endif
#if defined(HEAD_RICOH_G4)&&defined(SUPPORT_HEAD_HEATER)		
#ifdef CONVERSION_BOARD
	INT8U j = 0;
	UartCMD[0] = 2+MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION + 1;
	UartCMD[1] = UART_HEAD_SET_HEATER_ENABLE;
	for(i = 0;i < MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION;i++)
		UartCMD[2+i] = 1;
	for(j = 1;j <= Head_Board_Num;j++)
	{
		UartCMD[UartCMD[0] - 1] = j;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD)) 
			OSTimeDly(1);
		//OSTimeDly(500);
	}
#else
	UartCMD[0] = 2+MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION;
	UartCMD[1] = UART_HEAD_SET_HEATER_ENABLE;
	for(i = 0;i < MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION;i++)
		UartCMD[2+i] = 0;

	for(i = 0;i < MAX_EPSON_HEAD_CHANNEL;i++)
		UartCMD[2+ EpsonChannelMap[i]] = 1;
	for(i = 0;i < COLOR_NUMBER_CALIBRATION;i++)
		UartCMD[2+COLOR_NUMBER_CALIBRATION+i] = 1;

	while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD)) 
		OSTimeDly(1);
#endif	
#endif			

#if defined(HUMAN_Y_STATUS_OUTPUT)
	PIO_ConfigureIt(&Y_STATUS_INPUT, HUMAN_Y_STATUS_UP_Interrupt);
	PIO_EnableIt(&Y_STATUS_INPUT);
#if 0		
	PIO_ConfigureIt(&X_STATUS_INPUT, HUMAN_Y_STATUS_DOWN_Interrupt);
	PIO_EnableIt(&X_STATUS_INPUT);		
#endif		
#endif						
	while (1)
	{			
		INT8U sleep = 50;

		if (coreLEDFlashDly ++ > 20)
		{
			coreLEDFlashDly = 0;
			LED_APP_RUN();
		}

		cur_elapsedTime = OSTimeGet()/1000;  //ticks, ms

		if (cur_elapsedTime < old_elapsedTime)
			old_elapsedTime = cur_elapsedTime;

		if (cur_elapsedTime - old_elapsedTime >= TIME_LIMIT_EPR_WRT_UNIT)
		{
			elapsedTime.dirty = True;
			elapsedTime.time_elapse += cur_elapsedTime - old_elapsedTime;  //Seconds
			old_elapsedTime = cur_elapsedTime;								
		}	

		if (elapsedTime.dirty)
		{
			elapsedTime.dirty = False;
			len = EPR_LIMIT_TIME_SIZE;
			OSSemPend(IICSem, 0, &err);
			if (IIC_WriteEEPRom(EPR_LIMIT_TIME_OFFSET, (INT8U *)&elapsedTime, &len) == False)
				eepromWrtFailTimes++;
			else
				eepromWrtFailTimes = 0;
			OSSemPost(IICSem);
			if (eepromWrtFailTimes > 10)	
				status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
		}

		if ( (password.m_bHaveTimePwd) && (password.m_bTimeLimited) )
		{
			if (password.m_bTimeLimited < elapsedTime.time_elapse/3600)
			{
				status_ReportStatus(STATUS_FTA_LIMITEDTIME_RUNOUT,	STATUS_SET); //Time Limited run out
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_LAST,	STATUS_CLR); 
			}
			else if (password.m_bTimeLimited < (elapsedTime.time_elapse/3600 + TIME_LIMIT_WAR_LAST))
			{
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_SECOND,	STATUS_CLR); 
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_LAST,	STATUS_SET); 
			}
			else if (password.m_bTimeLimited < (elapsedTime.time_elapse/3600 + TIME_LIMIT_WAR_SECOND))
			{
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_FIRST,	STATUS_CLR);
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_SECOND,	STATUS_SET); 
			}else if (password.m_bTimeLimited < (elapsedTime.time_elapse/3600 +  TIME_LIMIT_WAR_FIRST))
				status_ReportStatus(STATUS_WAR_LIMITEDTIME_FIRST,	STATUS_SET);
			//CONSOL_Printf("%d hours passed!\r\n", elapsedTime.time_elapse/3600);
			//CONSOL_Printf("Password limited time = %d\r\n", password.m_bTimeLimited);	
		}
#if 0
		static INT8U i = 0;
		if(i==0)
		{
			PUMP_OPEN(255);
			i = 1;	
		}
		else	
		{
			PUMP_OPEN(0);
			i = 0;
		}
		OSTimeDly(1500);
#endif 

		if (UART_GetCMD(UART_MOTION_CHANNEL, UartCMD))
		{
#if DUMP_DSPCOM			
			CONSOL_Printf("Motion Command Received:");
			for (i = 0; i < UartCMD[0]; i ++)
			{
				CONSOL_Printf(" %x", UartCMD[i]);
			}
			CONSOL_Printf("\r\n");      
#endif        		  		

			sleep = 30;
			switch (UartCMD[1])
			{				
			case UART_INIT_STAGE2_CMD:
#if ( defined(FPGA_MAINTAIN_COOR) && ! (defined(SUPPORT_MOTOR_CONTROL)))
				OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5
				i = ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL);
				i |= ER_CoorCtrl_RESET_X;
				SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, i);
				i &= ~ER_CoorCtrl_RESET_X;
				SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, i);
#else
				tmpu = rFPGA_RICOH_COORCTRL_L;
				tmpu |= ER_CoorCtrl_RESET_X;
				rFPGA_RICOH_COORCTRL_L = tmpu;
				tmpu &= ~ER_CoorCtrl_RESET_X;
				rFPGA_RICOH_COORCTRL_L = tmpu;
#endif                
				OS_EXIT_CRITICAL();
#endif                
#ifdef MANUFACTURER_DYSS		
				bDyssMeasureDirty = True;
#endif		
#if defined (EPSON_CLEAN_UPDOWN)

#ifdef FULGENCY_FUN
				buf[0] = 6; //Length			
				buf[1] = UART_DSP_Z_SAFE_POSI; 
				*((__packed INT32U *)&buf[2]) = Z_SAFE_POSITION; //move_distance means target position
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(10);	
				buf[0] = 8; //Length			
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;  
				*((__packed INT32S *)&buf[4]) = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos; //move_distance means target position
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(10);	
				status_ReportStatus(STATUS_CAPPED, STATUS_SET);
				cleanPara.flash = True;
#else
				buf[0] = 8; //Length			
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;  
				*((__packed INT32S *)&buf[4]) = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos; //move_distance means target position
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(10);	
				status_ReportStatus(STATUS_CAPPED, STATUS_SET);
#endif

#else
#if defined( MICOLOR_AUTOFUCTION) && defined (EPSON_BOTTOM_BOARD_V3) 
				if(LCDMenuConfig.AutoPowerOnClean && !IsHomeWhenPowerOn())
					QuickClean_EPSON_MICOLOR();
#elif defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				*((__packed INT32S *)&buf[4]) = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance; 
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(10);						
#elif defined(EPSON_CLEAN_INTEGRATE_3)
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				*((__packed INT32S *)&buf[4]) = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance + cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance; 
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(10);
				status_ReportStatus(STATUS_CAPPED, STATUS_SET);
#endif
				status_ReportStatus(STATUS_INI_DSP, STATUS_CLR);
#endif
				//						CONSOL_Printf("Motion initial step 2 OK.\r\n");
				motionInitS2 = True;
				break;
#ifdef	VENDOR_PASSWORD_TOOL
			case UART_DSP_RPT_DESKEY:
				for (i = 0; i<8;i++)
					desKey[i] = UartCMD[2+i];
				break;
#endif						
			case UART_DSP_RPT_STOP: //Motion stop. Parameters: 4 bytes 当前位置 X: INT32S 类型: 4 字节, Little Endian
#if 0
				CONSOL_Printf("DSP fire number: %d-%d\r\n", g_DSP_FireNumber, debug_int2);
				g_DSP_FireNumber = 0;
#endif					
#ifdef ALLWIN_MEDIA_KEY   
				PIO_Clear(&CLEAN_STATUS_LED);
#endif
				//						if (UartCMD[0] != 6)
				//							CONSOL_Printf("Motion COM command error. Command: %x; Len: %x\r\n", UartCMD[1], UartCMD[0]);
				//						else
				{
					curPos.x = *(__packed INT32S *)(&UartCMD[2]);
					curPos.y = *(__packed INT32S *)(&UartCMD[6]);
					curPos.z = *(__packed INT32S *)(&UartCMD[10]);
					curPos.f = *(__packed INT32S *)(&UartCMD[14]);

#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP
					curPos.z = curPos.z_bak;
					curPos.f = curPos.f_bak;
#endif

#ifdef FULGENCY_FUN
					if(Y_GOHOME_Dirty == Y_GOHOME_WAITE)
					{
						UV_CTR_PRT_Y_HOME_SET();
						((INT16U*)buf)[0] = EP6_CMD_T_PRT_UNLOAD_FINISH;
						PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, buf, 2, False);
						Y_GOHOME_Dirty ++;
					}
#endif

#if defined(EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
					if (Backhaul_Flag)
					{
						Backhaul_Flag = False;        //close the Backhaul Flag

						if (!Judge_Printer_nozzle_come_back()&&postCapping)
						{
							Send_Second_Backhaul();
							break;
						}
					}
#endif
					//Debug							
					//							if (curPos.y - ttPos > 4000)
					//								status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG1, STATUS_SET);
					//							ttPos = curPos.y;
#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER)                         
					INT8U err2 = 0;
					OSFlagAccept(mix_FLAG_GRP_2, DYSS_UV_LIGHT_CAL, OS_FLAG_WAIT_CLR_ALL, &err2);
#endif
					OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER)                         
					if (err == OS_NO_ERR && err2 == OS_NO_ERR )
#else
						if (err == OS_NO_ERR)
#endif
						{								
							OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL, &err); //20081103
							if (err == OS_NO_ERR)
							{	
								OS_CPU_SR  cpu_sr;	
								//must report movement completed. Else, the following CMD_START_MOVE can't trig the movement.
								status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR);	
								OS_ENTER_CRITICAL();
								status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
								//Note: sometimes, before set CMD_START_MOVE, Parser task can print the next band. so i change STATUS_NO_PRINT_BITS, add CMD_PAUSE. 
								// 20111029: Add CMD_PAUSE is error. Sometimes, such as, when no moving and want to pause, 
								//  but Parser waiting STATUS_NO_PRINT_BITS and sleep. it will can not wake up.  
								//  because, to wakeup parser task , we must clear CMD_PAUSE. but main status is not pause and PM doesn't handle resume cmd.
								//  But if not move, CMD_PAUSE can't trig STATUS_PAUSE. 
								//  the best ways is not add CMD_PAUSE into STATUS_NO_PRINT_BITS. And it is disable interrupt between clear STATUS_MOVING and set CMD_START_MOVE.

								//The following 4 lines are added on 9/24/2008, after pause, go home.
#ifdef UV_PRINTER								
								uv_PrintPause();
#endif	

#ifdef EPSON_FLASH_IDLE
								move_dir = 2;
								move_distance = curPos.x - cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
								move_type = 0;
								postCapping_flash = True;
								cleanPara.flash = True;

#else
								move_dir = 2; //Go home
								move_distance = 0;
								move_type = 0;
#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
								RegPostCapping(); //waiting action completed
#endif

#endif

								status_ReportStatus(CMD_START_MOVE, STATUS_SET);
								//The above 4 lines are added on 9/24/2008, after pause, go home.
								status_ReportStatus(STATUS_PAUSE, STATUS_SET);

								OS_EXIT_CRITICAL();	
							}
							else
							{
#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
								if(!CapPostHandleAfterMoveCompleted())
#endif                
								{
									status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR);	
									status_ReportStatus(STATUS_MOVING, STATUS_CLR);							
								}
							}
						}
						else
#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
							if(!CapPostHandleAfterMoveCompleted())
#endif                
							{
								status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR);	
								status_ReportStatus(STATUS_MOVING, STATUS_CLR);							
							}
				}
#if defined (EPSON_CLEAN_UPDOWN)
				OSFlagAccept(status_FLAG_GRP, STATUS_INI, OS_FLAG_WAIT_SET_ALL, &err);
				if (err == OS_NO_ERR)
					status_ReportStatus(STATUS_INI_DSP, STATUS_CLR);
#elif defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
				OSFlagAccept(status_FLAG_GRP, STATUS_INI, OS_FLAG_WAIT_SET_ALL, &err);
				if (err == OS_NO_ERR)
					status_ReportStatus(STATUS_INI_DSP, STATUS_CLR);
#endif                
				break;
			case UART_DSP_RPT_POSITION: //Motion report position
				if (UartCMD[0] != 6)
					CONSOL_Printf("Motion COM command error. Command: %x; Len: %x\r\n", UartCMD[1], UartCMD[0]);
				else
				{	
					//							CONSOL_Printf("Clear Flag\r\n");						
					curPos.x = *(__packed INT32S *)(&UartCMD[2]);
					valFlag_GetPos = 0;
					//OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_GETPOS, OS_FLAG_CLR, &err);
				}
				break;

			case UART_DSP_RPT_DEBUG_INFO: //Motion report position

				for (i = 0; i < UartCMD[0]; i++)
					dspDebugInfo[i] = UartCMD[i];

				break;

			case UART_DSP_RPT_PRIVATE_DATA: //Motion report position

				for (i = 0; i < UartCMD[0]; i++)
					dspPrivateData[i] = UartCMD[i];
				break;					

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			case UART_DSP_RPT_PIPE_DATA:
				PushCommPipeData(COMMPIPE_DSP_CHANNEL_ID, UartCMD, UartCMD[0], False);
				break;  
#endif

#ifdef EPSON_BOTTOM_BOARD_V2
			case UART_RPT_DSP_NEEDLE:
				if(UartCMD[2] == 1)
					Needle_Open();
				else
					Needle_Close();
				break;  
#endif

#if defined(DYSS_UV_CLEAN)
			case UART_RPT_DSP_HIGH_VALUE:
				status_ReportStatus(STATUS_MOVING, STATUS_CLR); 
				DYSS_MEASURE_HIGH_VALUE = (float)(*((__packed INT16U*)&UartCMD[3]))/Z_BASE_RES;
				MEASURE_HIGH_SUCCSE = UartCMD[2];
				((INT16U*)UartCMD)[0] = EP6_CMD_T_MEDIA_HIGH;
				PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
				break;
			case UART_RPT_DSP_MEASURE_INIT_OVER:
				status_ReportStatus(STATUS_MOVING, STATUS_CLR); 
				((INT16U*)UartCMD)[0] = EP6_CMD_T_MEDIA_INIT_OVER;
				PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
				break; 	
			case UART_RPT_DSP_MEASURE_TOUCH:
				{
					INT8U err;
					OSSemPost(MenuSem);
					UIAction_PauseResumePrint((void *)1);
					OSSemPend(MenuSem, 0, &err);
					break;		
				}
#endif
			case UART_UPDATE_DSP_INFO:
				if (UartCMD[0] == 4)
				{							
					OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_CLR, &err);
					if ((UartCMD[2] != 0)&&(UartCMD[3] >= 0xA0))
						status_ReportStatus(STATUS_UPDATING + UartCMD[3],STATUS_SET);
				}
				break;
			case UART_DSP_RPT_STATUS:
				if ((UartCMD[0] == 8)&&(UartCMD[3] >= 0xD0))
				{
					switch (UartCMD[2])
					{									
					case 1:
						status_ReportStatus(STATUS_SVC+UartCMD[3], STATUS_SET);
						break;
					case 2:
						status_ReportStatus(STATUS_FTA+UartCMD[3], STATUS_SET);
						break;
					case 3:
						status_ReportStatus(STATUS_ERR+UartCMD[3], STATUS_SET);
						break;
					case 4:									
						status_ReportStatus(STATUS_WAR+UartCMD[3], UartCMD[4]);
						break;
					case 5:
						status_ReportStatus(STATUS_ERR_AUTO+UartCMD[3], UartCMD[4]);
						break;
					}
				}
				break;							
			case UART_RPT_VERSION:
				fwInfo.mt_version = (UartCMD[5]<<24)|UartCMD[4]<<16|UartCMD[3]<<8|UartCMD[2];
				memcpy(fwInfo.mtfw_date, &UartCMD[6], 12);
				break;
			case UART_DSP_RPT_PHYSICAL_SIZE:
				//						printerPhysicalSize.x = *(__packed INT32S *)(&UartCMD[2]);
				printerPhysicalSize.x = *(__packed INT32S *)(&UartCMD[2]) - printer.accSpace - printer.platSpace - PRINTER_SAFE_SPACE;
				printerPhysicalSize.y = *(__packed INT32S *)(&UartCMD[6]);
				printerPhysicalSize.z = *(__packed INT32S *)(&UartCMD[10]);
				printerPhysicalSize.f = *(__packed INT32S *)(&UartCMD[14]);
				break;

			case UART_DSP_RPT_MEDIA:
				mediaVolNum = (INT8S)UartCMD[2];
				if (mediaVolNum > 0)
				{
					if (mediaVolNum > MAX_PAPER_VOLUME)
						mediaVolNum = MAX_PAPER_VOLUME;
					i = UartCMD[3]; //Index
					if (i < mediaVolNum)
					{
						mediaEdge[i].paper_X_NearEdgePos = *((__packed INT32S *)&UartCMD[4]);
						mediaEdge[i].paper_X_FarEdgePos  = *((__packed INT32S *)&UartCMD[8]);
						mediaEdge[i].paper_Y_NearEdgePos = *((__packed INT32S *)&UartCMD[12]);
						mediaEdge[i].paper_Y_FarEdgePos  = *((__packed INT32S *)&UartCMD[16]);
					}
					//#if defined(MICOLOR_AUTOFUCTION)||defined(ALLWIN_EPSON_SAME) ||defined(MANUFACTURER_BEMAJET)||defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_SAISHUN_WATER_EPSON)||defined(MANUFACTURER_RIPSTAR_TEST_UV)                                                
					{
						SetMeasureResult(mediaEdge[i].paper_X_NearEdgePos, mediaEdge[i].paper_X_FarEdgePos);
					}
					//#endif
				}
#ifndef SUPPORT_MOTOR_CONTROL	
				else
					status_ReportStatus(STATUS_WAR+UartCMD[2], STATUS_SET);
#endif
#if defined (EPSON_CLEAN_UPDOWN)
				//status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR);	
				//status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
				//OSFlagPend(status_FLAG_GRP,CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); 
				Backhaul_Flag = True;
				RegPostCapping();
#endif                
				break;
			case UART_DSP_REQ_READ_EEPROM:
				addr = (UartCMD[2] << 8)|UartCMD[3];
				len  = UartCMD[4];
				if ((len > 23) || (addr + len >= EPR_DSP_DISK_SIZE))
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_RPT_DSP_READ_ERR;
					UartCMD[2] = 1;
					UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);
				}
				else
				{
					OSSemPend(IICSem, 0, &err);
					if (IIC_ReadEEPRom(EPR_DSP_DISK_OFFSET + addr, &UartCMD[5], &len))
					{
						UartCMD[0] = len + 5;
						UartCMD[1] = UART_RPT_DSP_EEPROM_CONTENT;
						UartCMD[4] = len;
						UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);
#if defined(FULGENCY_FUN)
						UartCMD[0] = 5;
						UartCMD[1] = UART_RPT_DSP_READ_ERR;
						UartCMD[2] = 0;
						UartCMD[3] = addr >> 8;
						UartCMD[4] = addr;
#else
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_READ_ERR;
						UartCMD[2] = 0;
#endif
						UART_SendCMD(UART_MOTION_CHANNEL, UartCMD); 						
					}else
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_READ_ERR;
						UartCMD[2] = 2; //Read failed.
						UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);
					}
					OSSemPost(IICSem);
				}			
				break;
			case UART_DSP_WRITE_EEPROM:
				addr = (UartCMD[2] << 8)|UartCMD[3];
				len  = UartCMD[4];
				if ((len > 23)|| (addr + len >= EPR_DSP_DISK_SIZE))
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
					UartCMD[2] = 1;
					UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);
				}
				else
				{ //
					OSSemPend(IICSem, 0, &err);
					if (IIC_WriteEEPRom(EPR_DSP_DISK_OFFSET+addr, &UartCMD[5], &len))
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
						UartCMD[2] = 0;
						UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);	
					}else
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
						UartCMD[2] = 2; //Write.
						UART_SendCMD(UART_MOTION_CHANNEL, UartCMD);
					}
					OSSemPost(IICSem);
				}			
				break;
				//#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#if 0
			case UART_RPT_DSP_PASS_START:
				UartCMD[0] = 6;
				UartCMD[1] = UART_HEAD_EPSON_PASSSTART;
				UartCMD[2] = (INT8U)FireCount ;
				UartCMD[3] = (INT8U)(FireCount>>8);
				UartCMD[4] = (INT8U)FireCountEx;
				UartCMD[5] = (INT8U)(FireCountEx>>8);
				UART_SendCMD(UART_HEAD_CHANNEL, UartCMD);
				break;
#endif						
#if defined (EPSON_CLEAN_UPDOWN)
			case UART_RPT_DSP_SWIPE_COMPLETED:
				status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
				break;
#endif
#ifdef ALLWIN_EPSON_SAME
			case UART_DSP_PRINT_PAUSE:		//UartCMD[2]:单次为0，多次为1		UartCMD[3]:暂停为1，不暂停为0,
				{
					INT8U err;
					if(UartCMD[2] == 0)			//单次：暂停可以恢复，如果DSP检测到信号，发送暂停，但可以通过继续打印来恢复打印操作。
					{
						if(UartCMD[3] == 0)		//	继续
						{
							OSSemPost(MenuSem);
							Dspcmd_PauseResumePrint((void *)4);		//单次：参数为4
							OSSemPend(MenuSem, 0, &err);
						}
						if(UartCMD[3] == 1)		//	暂停
						{
							OSSemPost(MenuSem);
							Dspcmd_PauseResumePrint((void *)5);		//单次：参数为5
							OSSemPend(MenuSem, 0, &err);
						}
					}
					else if(UartCMD[2] == 1)	//多次：暂停只有在信号发生变化的时候才可以恢复，即不可通过PM或CLD控制恢复
					{
						if(UartCMD[3] == 0)		//	继续
						{
							OSSemPost(MenuSem);
							Dspcmd_PauseResumePrint((void *)2);		//多次：继续：打印参数 为2
							OSSemPend(MenuSem, 0, &err);
						}
						if(UartCMD[3] == 1)		//	暂停
						{
							OSSemPost(MenuSem);
							Dspcmd_PauseResumePrint((void *)3);		//多次：暂停：打印参数 为3
							OSSemPend(MenuSem, 0, &err);
						}
					}
					break;
				}
#endif
			}				
		}

		if (UART_GetCMD(UART_HEAD_CHANNEL, UartCMD))
		{
#if DUMP_HEADCOM
			INT8U key;			
			CONSOL_Printf("Head Board Command Received:");
			for (key = 0; key < UartCMD[0]; key ++)
			{
				CONSOL_Printf(" %x", UartCMD[key]);
			}
			CONSOL_Printf("\r\n");      
#endif        		  		

			switch (UartCMD[1])
			{
#if EPSON_BOTTOM_BOARD_V3
			case UART_RESISTANCE_OFFSET:
				memcpy((void *)HeaterInfo.ResistanceOffset, UartCMD+2, sizeof(HeaterInfo.ResistanceOffset));
				break;
#endif
			case UART_INIT_STAGE2_CMD:
				status_ReportStatus(STATUS_INI_HEADBOARD, STATUS_CLR); 
				headInitS2 = True;
				break;

#ifdef WAVE_SET_FUN
			case UART_HEAD_WAVE_TRANSFER:
				if(UartCMD[2] != UartCMD[3])
				{
					memcpy((void*)&(WAVE_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);
					DATA_LEN += WAVE_DATA_MAX_LEN;
				}
				else
				{
					memcpy((void*)&(WAVE_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);
					DATA_LEN += (UartCMD[0] - 4);
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_READ_READY;
					PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
				}
				break;
			case UART_HEAD_WAVE_READY:
				((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_PRINT_READY;
				PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);	  	
				break;
			case UART_HEAD_WAVE_NAME_TRANSFER:
				if(UartCMD[2] != UartCMD[3])
				{
					memcpy((void*)&(WAVE_NAME_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);
					DATA_LEN += WAVE_DATA_MAX_LEN;
				}
				else
				{
					memcpy((void*)&(WAVE_NAME_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);
					DATA_LEN += (UartCMD[0] - 4);
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_NAME_READY ;
					PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
				}
				break;
			case UART_HEAD_WAVE_CHANNEL_TRANSFER:
				if(UartCMD[2] != UartCMD[3])
				{
					memcpy((void*)&(WAVE_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);
					memcpy((void*)&(WAVE_MAP_TABLE[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);	//备份波形映射表
					DATA_LEN += WAVE_DATA_MAX_LEN;
				}
				else
				{
					memcpy((void*)&(WAVE_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);
					memcpy((void*)&(WAVE_MAP_TABLE[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);	//备份波形映射表
					DATA_LEN += (UartCMD[0] - 4);
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_CHANNEL_FINISH;
					PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
				}
				break;
#endif
#if defined(FUNC_WAVE_MAP)&& !defined(WAVE_SET_FUN)
			case UART_HEAD_WAVE_NAME_TRANSFER:
				if(UartCMD[2] != UartCMD[3])
					memcpy((void*)&(WAVE_NAME_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);
				else
					memcpy((void*)&(WAVE_NAME_BUF[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);
				break;

			case UART_HEAD_WAVE_CHANNEL_TRANSFER:
				if(UartCMD[2] != UartCMD[3])
					memcpy((void*)&(WAVE_MAP_TABLE[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), WAVE_DATA_MAX_LEN);	//备份波形映射表
				else
					memcpy((void*)&(WAVE_MAP_TABLE[(UartCMD[3]-1)*WAVE_DATA_MAX_LEN]), (void*)&(UartCMD[4]), UartCMD[0] - 4);	//备份波形映射表
				break;
#endif

#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
			case UART_HEAD_WAVE_SET_FINISH:
				OSFlagPost(mix_FLAG_GRP_2,HEAD_WAVE_SET_FINISH,OS_FLAG_SET,&err);
				break;
#endif	

			case UART_HEAD_PUMP_BITS:
#ifdef CONVERSION_BOARD
				if(UartCMD[UartCMD[0] - 1] == 1)
					UartCMD[0]--;
				else
					break;
#endif
#if defined(EPSON_4H)&&defined(HEAD_LEVEL_SENSOR)
				if (headboardInfo.headBoardType ==HEAD_BOARD_TYPE_EPSON_GEN5_4HEAD&&UartCMD[0] == 3)											
				{				
#ifdef MANUFACTURER_HUMAN_EPSON					
					Ink_level_map(UartCMD[2]);
#else
					INK_LEVEL_MASK = UartCMD[2];
#endif

				}
#elif (defined(HEAD_RICOH_G4))&&(defined(HEAD_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR_NO_PUMP))						
				if ((headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H ||
							headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H||
							headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H||
							headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H)&&UartCMD[0] == 4)											
				{
					UartCMD[2] &= ~UartCMD[3];
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)||defined(CONVERSION_BOARD)||defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_DOCAN_UV)||defined(MANUFACTURER_SAIBO_ROLL_UV)||defined(ORIC_FUNCTION)
					INK_LEVEL_MASK = UartCMD[2];
#elif defined(HEAD_LEVEL_SENSOR_NO_PUMP)
					Ink_level_map(UartCMD[2]);
#else
					Ink_level_map(UartCMD[2]);
#endif
				}
#endif

				break;
			case UART_INIT_STAGE1_CMD:
				if (headInitS2)
#ifdef CONVERSION_BOARD						
					status_ReportStatus(STATUS_FTA_HEADBOARD_RESET|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
#else
				status_ReportStatus(STATUS_FTA_HEADBOARD_RESET, STATUS_SET); 
#endif
				break;
			case UART_HEAD_TEMEPRATUE_CMD:
			case UART_HEAD_TEMEPRATUE_EX_CMD:
#if defined(HEAD_RICOH_G4)
				//#warning "not implement temperature control for ricoh"

				if(UartCMD[1] == UART_HEAD_TEMEPRATUE_CMD)
				{
#ifdef CONVERSION_BOARD	


					if(UartCMD[UartCMD[0]-1] == 0 ||UartCMD[UartCMD[0]-1] > MAX_HB_NUM)
						UartCMD[UartCMD[0]-1] = 1;

					for (i = 0; i < MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION; i++)
					{
						if(i < MAX_EPSON_HEAD_CHANNEL)
						{
							if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H && i >= MAX_TEMP_CHANNEL)
								continue;					 	
							else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H&& i >= MAX_TEMP_CHANNEL)
								continue;					 	
							else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H&& i >= MAX_TEMP_CHANNEL)
								continue;	
#ifdef MANUFACTURER_DYSS
							g_headTemeprature[(UartCMD[UartCMD[0]-1] - 1)*MAX_TEMP_CHANNEL + Hardmap_to_PM_map[i]] = UartCMD[i+2]*2;  //order in headboard <=> order in main board and host 
#else
							g_headTemeprature[(UartCMD[UartCMD[0]-1] - 1)*MAX_TEMP_CHANNEL + Hardmap_to_PM_map[i]] = UartCMD[i+2]*5;  //order in headboard <=> order in main board and host 
#endif
						}else if(UartCMD[UartCMD[0]-1] == 1)
						{
#ifdef MANUFACTURER_DYSS
							g_headHeaterThermistorTemeprature[i-MAX_EPSON_HEAD_CHANNEL] = UartCMD[i+2]*2;
#else
							g_headHeaterThermistorTemeprature[i-MAX_EPSON_HEAD_CHANNEL] = UartCMD[i+2]*5;
#endif
						}
					}
#else					
					for (i = 0; i < MAX_EPSON_HEAD_CHANNEL+COLOR_NUMBER_CALIBRATION; i++)
					{
						if(i < MAX_EPSON_HEAD_CHANNEL)
						{
							if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H && i >= MAX_TEMP_CHANNEL)
								continue;					 	
							else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H&& i >= MAX_TEMP_CHANNEL)
								continue;	
							else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H&& i >= MAX_TEMP_CHANNEL)
								continue;
#ifdef MANUFACTURER_DYSS
							g_headTemeprature[Hardmap_to_PM_map[i]] = UartCMD[i+2]*2;  //order in headboard <=> order in main board and host 
#else
							g_headTemeprature[Hardmap_to_PM_map[i]] = UartCMD[i+2]*5;  //order in headboard <=> order in main board and host 
#endif
						}else
						{
#ifdef MANUFACTURER_DYSS
							g_headHeaterThermistorTemeprature[i-MAX_EPSON_HEAD_CHANNEL] = UartCMD[i+2]*2;
#else
							g_headHeaterThermistorTemeprature[i-MAX_EPSON_HEAD_CHANNEL] = UartCMD[i+2]*5;
#endif
						}
					}
#endif						
				}
				else
				{
					for (i = 0; i < MAX_HEAD_NUMBER; i++)
					{
						INT8U num;							
						num = vol_MapHeadNumber(i, True, False, False, False);
						if(num < MAX_HEAD_NUMBER/2)
						{
							if(UartCMD[2] == 0)
								g_headTemeprature[num] = UartCMD[i+3];  //order in headboard <=> order in main board and host 
							else
								g_headTemeprature[num + MAX_HEAD_NUMBER/2] = UartCMD[i+3];  //order in headboard <=> order in main board and host 
						}
						num = vol_MapHeadNumber(i, False, False, True, False);
						if(num < MAX_HEAD_NUMBER/2)
						{
							if(UartCMD[2] == 0)
								g_headHeaterThermistorTemeprature[num] = UartCMD[i+3];  //order in headboard <=> order in main board and host 
							else
								g_headHeaterThermistorTemeprature[num + MAX_HEAD_NUMBER/2] = UartCMD[i+3];  //order in headboard <=> order in main board and host 
						}
					}
				}

				if (g_headVoltageData.autoCon)
				{
					INT8U vol;
					for (i = 0; i < MAX_VOLTAGENUMBER; i++)
					{
						vol = vol_CalHeadVoltage(i);
						if (g_curHeadVoltage[i] != vol)
						{
							g_curHeadVoltage[i] = vol;
							g_headVoltageData.headUpdated = False;
						}
					}
				}
#endif                
				break;
			case UART_RPT_VERSION:
				fwInfo.hd_version = (UartCMD[5]<<24)|UartCMD[4]<<16|UartCMD[3]<<8|UartCMD[2];
				memcpy(fwInfo.hdfw_date, &UartCMD[6], 12);
#if defined(HEAD_EPSON_GEN5)&&!defined(EPSON_4H)
				if((fwInfo.hd_version  & 0xFFFFFFFF) >= 0x03030101)
				{
					if(!Already_Reset_FPGA)
					{
						if(OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, &err)&&motionInitS2)
						{
							FPGA_RESETAGAIN();
							Already_Reset_FPGA = True;
						}
						else
							Already_Reset_FPGA = False;
					}
				}
				else
					Already_Reset_FPGA = True;			
#endif

#if defined(FUNC_WAVE_MAP)
				if((fwInfo.hd_version  & 0xFFFFFFFF) >= 0x42050101)
				{
					ReadWaveName();			//读取波形名称
					ReadWaveMapTable();		//读取波形映射表	
				}
#endif

				if((fwInfo.hd_version>>31) != 0)					
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_HEAD_SET_ENCRYPT;
					UartCMD[2] = LCDMenuConfig_EX.Head_Encry_Mask;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
						OSTimeDly(1);
				}
				if (((fwInfo.hd_version & 0xFFFFFF) <0x00020200) && (factoryData.HeadType == HeadNo_Konica_KM512LNX))
					status_ReportStatus(STATUS_FTA_PRINTHEAD_NOTMATCH, STATUS_SET);
#ifdef ENABLE_PULSE_WIDTH_ADJUST
				//force flush head setting because some FW need more data.
				g_headFirePulseWData.headUpdated = False;
#endif                                                
				break;
			case UART_RPT_WRITE_EEPROM_RESULT:
				if (UartCMD[2]) //0 means OK
#ifdef CONVERSION_BOARD						
					status_ReportStatus(STATUS_FTA_HB_EEPROM_WRT_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
#else
				status_ReportStatus(STATUS_FTA_HB_EEPROM_WRT_ERR, STATUS_SET);
#endif

				break;
			case UART_HEAD_RPT_OVERHEAT:
#ifdef CONVERSION_BOARD						
				status_ReportStatus(STATUS_FTA_HB_OVERHEAT|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
#else
				status_ReportStatus(STATUS_FTA_HB_OVERHEAT, STATUS_SET); 
#endif

				break;
			case UART_HEAD_RPT_FPGA_ERR:
#ifdef CONVERSION_BOARD						
				status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
#else
				status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR, STATUS_SET); 
#endif
				break;  
			case UART_UPDATE_HB_INFO:
				if (UartCMD[6] == 0xFF)
					status_ReportStatus(STATUS_WAR_HB_FW_ID_ERROR, STATUS_SET);
				else if (UartCMD[6] != 0)
					status_ReportStatus(STATUS_WAR_HB_FW_UPDATER, STATUS_SET);
				OSFlagPost(mix_FLAG_GRP, (UartCMD[6] == 0) ? MISC_FLAGS_UPDATE_HB : MISC_UPDATE_HB_ERROR, OS_FLAG_CLR, &err);
				break;

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			case UART_HEADRPT_PIPE_DATA:
				PushCommPipeData(COMMPIPE_HEADBOARD_CHANNEL_ID, UartCMD, UartCMD[0], False);
				break;  
#endif
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			case UART_HEADRPT_EPSON_STATUS:
				if(UartCMD[2] & (1<<EHSB_ERROR))
				{
					if(UartCMD[2] & (1<<EHSB_HEAD_IS_BAD))
					{
#ifdef CONVERSION_BOARD						
						status_ReportStatus(STATUS_FTA_EPSON_HEAD_BAD|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
#else
						status_ReportStatus(STATUS_FTA_EPSON_HEAD_BAD, STATUS_SET);
#endif
					}
					/*if(UartCMD[2] & (1<<EHSB_FPGA_ERROR))
					  {
					  status_ReportStatus(STATUS_FTA_EPSON_HEAD_FPGA_ERR, STATUS_SET);
					  }*/
					if(UartCMD[2] & (1<<EHSB_HEAD_NOT_CONNECTED))
					{
						INT8U head_group = (COLOR_NUMBER_CALIBRATION * factoryDataEx.YInterleaveNum + NOZZLE_LINE_COUNT_PER_HEAD - 1)/ 
							NOZZLE_LINE_COUNT_PER_HEAD;
						INT8U headmask_subgroup = (1<<head_group) -1;
						INT8U headmask = 0, index;
						for( index = 0; index <EPSON_PERMIT_GROUPNUM; index++)
						{
							if((1<<index)&LCDMenuConfig.Active_HeadMask)
							{
								headmask |= (headmask_subgroup<<(index *head_group));
							}
						}
#ifdef CONVERSION_BOARD	
						if( headmask & UartCMD[3])
							status_ReportStatus(STATUS_WAR_EPSONHEAD_NOT_CONNECT|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
						else
							status_ReportStatus(STATUS_WAR_EPSONHEAD_NOT_CONNECT|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);

#else
						if( headmask & UartCMD[3])
							status_ReportStatus(STATUS_WAR_EPSONHEAD_NOT_CONNECT, STATUS_SET);
						else
							status_ReportStatus(STATUS_WAR_EPSONHEAD_NOT_CONNECT, STATUS_CLR);
#endif							
					}
				}
				else
				{
#ifdef CONVERSION_BOARD						
					status_ReportStatus(STATUS_WAR_EPSONHEAD_TEMP_TOO_HIGH|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), 
							(UartCMD[2] & ((1<<EHSB_TEMPERATURE_TOO_HIGH) /*| (1<<EHSB_FPGA_XHOT)*/ ) ) ? HEAD_ERROR_SET: STATUS_CLR);
					status_ReportStatus(STATUS_WAR_EPSONHEAD_TEMP_TOO_LOW|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), 
							(UartCMD[2] & (1<<EHSB_TEMPERATURE_TOO_LOW)) ? HEAD_ERROR_SET: STATUS_CLR);
#else
					status_ReportStatus(STATUS_WAR_EPSONHEAD_TEMP_TOO_HIGH, 
							(UartCMD[2] & ((1<<EHSB_TEMPERATURE_TOO_HIGH) /*| (1<<EHSB_FPGA_XHOT)*/ ) ) ? STATUS_SET: STATUS_CLR);
					status_ReportStatus(STATUS_WAR_EPSONHEAD_TEMP_TOO_LOW, 
							(UartCMD[2] & (1<<EHSB_TEMPERATURE_TOO_LOW)) ? STATUS_SET: STATUS_CLR);
#endif
				}
				if(UartCMD[2] & (1<<EHSB_FPGA_ERROR))
				{
					//the first HB version will report (1<<EHSB_FPGA_DISABLE) always though not check security chip.
					//the second HB version will report the right EHSB_FPGA_DISABLE bit based on check security chip.
					//the third will report EHSB_FPGA_ERROR and not report EHSB_FPGA_DISABLE bit.
					//So the final is,
					if(!(UartCMD[2] & (1<<EHSB_FPGA_DISABLE)) && ! (UartCMD[2] & (1<<EHSB_FPGA_XHOT)))
#ifdef CONVERSION_BOARD													
						status_ReportStatus(STATUS_SVC_HEADBOARD_INIT_FAIL|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
#else
					status_ReportStatus(STATUS_SVC_HEADBOARD_INIT_FAIL, STATUS_SET);
#endif
				}
#if defined(HEAD_RICOH_G4)
				if(UartCMD[0] == 8)
				{
					shakeInfo_Ricoh.Large_shake_1_unit_count = UartCMD[4];
					shakeInfo_Ricoh.Large_shake_0_unit_count = UartCMD[5];
					shakeInfo_Ricoh.Small_shake_0_shake_count = ((((INT16U)UartCMD[7])<<8)+UartCMD[6]);
				}
#endif           
				break;
			case UART_HEADRPT_FPGA_STATUS:
#ifdef EPSON_4H
				if((UartCMD[2] & (1<<3))&&(UartCMD[2 + 9] >= HEAD_ERROR_REPORT_MAX_NUM))
#ifdef CONVERSION_BOARD													
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_REPORT_LVDS_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_REPORT_LVDS_ERR, STATUS_SET);
#endif


				if(((UartCMD[2] & (1<<2))&&((UartCMD[2 + 10] + UartCMD[2 + 11] << 8) >= HEAD_ERROR_REPORT_MAX_NUM)) ||
						((UartCMD[2] & (1<<1))&&((UartCMD[2 + 12] + UartCMD[2 + 13] << 8) >= HEAD_ERROR_REPORT_MAX_NUM)) ||
						((UartCMD[2] & (1<<0))&&((UartCMD[2 + 14] + UartCMD[2 + 15] << 8) >= HEAD_ERROR_REPORT_MAX_NUM)))
#ifdef CONVERSION_BOARD													
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR, STATUS_SET);
#endif

#else
#ifndef EPSON_DX5E                    
#ifdef CONVERSION_BOARD													
				if(UartCMD[2] & (1<<EHFB_FPGA_ERROR))
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_CLEAR);
#else
				if(UartCMD[2] & (1<<EHFB_FPGA_ERROR))
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR, STATUS_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR, STATUS_CLR);
#endif

#endif					
#endif              
				//if(!(UartCMD[10] & EHFB_FPGA_ENABLE))
				//    status_ReportStatus(STATUS_SVC_HEADBOARD_INIT_FAIL, STATUS_SET);
#ifdef CONVERSION_BOARD													
				if(UartCMD[5] & EHFB_HB_DRIVER_OVERHEAT)
					status_ReportStatus(STATUS_WAR_EPSON_DRIVER_OVERHEAT|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_DRIVER_OVERHEAT|((UartCMD[UartCMD[0]-1]&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_CLEAR);

#else					
				if(UartCMD[5] & EHFB_HB_DRIVER_OVERHEAT)
					status_ReportStatus(STATUS_WAR_EPSON_DRIVER_OVERHEAT, STATUS_SET);
				else
					status_ReportStatus(STATUS_WAR_EPSON_DRIVER_OVERHEAT, STATUS_CLR);
#endif					

				OS_ENTER_CRITICAL();	
				memcpy((void*)headFPGADebugInfo, UartCMD, ((UartCMD[0]>sizeof(headFPGADebugInfo))? sizeof(headFPGADebugInfo) : UartCMD[0]));
				OS_EXIT_CRITICAL(); 	
				break;
			case UART_HEADRPT_HB_PARAM:
#ifdef CONVERSION_BOARD
				if(UartCMD[UartCMD[0] - 1] != 1)
					break;
#endif					
				if(UartCMD[4] == (INT8U)HBReadLen && (UartCMD[2] == (INT8U)(HBReadAddr >> 8)) && (UartCMD[3] == (INT8U)HBReadAddr))
				{
					memcpy((void*)HBParamBuf,&(UartCMD[5]), HBReadLen);
					bReadHeadBoardParam = 3;
				}
				break;
			case UART_HEAD_EPSON_PARAM_PERCENT:
#ifdef CONVERSION_BOARD

#ifdef RICOH_G4_16H
				if(UartCMD[UartCMD[0] - 1] == 1)
					memcpy(g_headParameterPercent, &(UartCMD[2]), 8);
				else if(UartCMD[UartCMD[0] - 1] == 2)
					memcpy((void *)&g_headParameterPercent[8], &(UartCMD[2]), 8);						
#else
#error
#endif

#else
				memcpy(g_headParameterPercent, &(UartCMD[2]), sizeof(g_headParameterPercent));
#endif					
				break;
#endif
#if defined( HEAD_EPSON_GEN5)
			case UART_HEAD_EPSON_SENSOR:
#ifdef CONVERSION_BOARD
				if(UartCMD[UartCMD[0] - 1] != 1)
					break;

#ifdef defined(RICOH_G4_16H)
				break;
#endif

#endif						
				{
#if defined( MANUFACTURER_ALLWIN_EPSON)||defined( MANUFACTURER_ALLWIN_FLAT)||defined(MANUFACTURER_ALLWIN_2H_ROLL_UV)
					if(!(UartCMD[2] & (1<<HEAD_HEIGHT_LEVEL_SENSOR_BIT)))
					{
						buf[0] = 2;
						buf[1] = UART_URGENTSTOP_CMD;
						while(!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
							OSTimeDly(10);	
						status_ReportStatus(STATUS_FTA + 0xF9, STATUS_SET);
					}
#else
					INT8U level;
					if(UartCMD[2] & (1<<HEAD_HEIGHT_LEVEL_SENSOR_BIT))
					{
						level = 1;
						LCDMenuConfig.Head_Height_Level = 0;
					}
					else
					{
						level = 3;
						LCDMenuConfig.Head_Height_Level = 1;
					}
#if defined( EPSON_LCD)
					LCD_SetMainInfoEx(E_IT_HeadHeightLevel, &level);
#endif                    
					bCaliSettingDirty = True;
#endif
				}
				break;
#endif
			}
			sleep = 5;
		}

#ifdef ENABLE_VOLTAGE_ADJUST
		if (!g_headVoltageData.headUpdated)
		{
			INT16S voltage;
			UartCMD[0] = 2 + 16;
			UartCMD[1] = UART_HEAD_VOLTAGE_SET;

			g_headVoltageData.headUpdated = True; //This must be changed first than COM command generation

			for (i = 0; i < 16; i++)
			{
				INT8U num;
				num = vol_MapHeadNumber(i, False, False, False, False);
				voltage = g_curHeadVoltage[num] + g_headVoltageData.manualData[num] - 20;     				    				
				if (voltage > 255)
					voltage = 255;
				if (voltage < 0)
					voltage = 0;

				UartCMD[2+i] = (INT8U)voltage;    	  
				//    				CONSOL_Printf("Head %d: CurV = %d, adjVol = %d, result = %d\r\n", i, g_curHeadVoltage[i], g_headVoltageData.manualData[i], voltage);  			
			}

			if ((headboardInfo.headBoardType == HEAD_BOARD_TYPE_KM512_16HEAD) //16 Head KM512, just for test, need change
					||(headboardInfo.headBoardType == HEAD_BOARD_TYPE_KM512_16HEAD_V2))
			{
				while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
					OSTimeDly(1);

				UartCMD[0] = 2 + MAX_VOLTAGENUMBER/VOLTAGE_NUMBER_PER_HEAD;
				UartCMD[1] = UART_HEAD_VOLTAGE_SET1;
				for (i = MAX_VOLTAGENUMBER/VOLTAGE_NUMBER_PER_HEAD; i < MAX_VOLTAGENUMBER; i++)
				{
					INT8U num;
					num = vol_MapHeadNumber(i, False, False, False, False);
					voltage = g_curHeadVoltage[num] + g_headVoltageData.manualData[num] - 20;
					if (voltage > 255)
						voltage = 255;
					if (voltage < 0)
						voltage = 0;

					UartCMD[2 + i - MAX_VOLTAGENUMBER/VOLTAGE_NUMBER_PER_HEAD] = (INT8U)voltage;    	  
				}
			}
#if 1
			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);
#else //Debug
			while (1)
			{
				if (UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))
				{
					CONSOL_Printf("Voltage command sent: \n");
					for (i = 0; i < 16; i++)
						CONSOL_Printf("0x%X\r\n", UartCMD[i+2]);
					break;
				}
			}						
#endif					
		}
#endif

#ifdef ENABLE_PULSE_WIDTH_ADJUST	
		if (!g_headFirePulseWData.headUpdated)
		{
			INT8U num;
			INT8U isDoublePulse = False;

			//some polaris head board FW support double pulse setting.
			if (((fwInfo.hd_version & 0xF00000) == 0x100000) && (headboardInfo.headCategory == HEAD_CATEGORY_SPECTRA_POLARIS))
				isDoublePulse = True;

			if(isDoublePulse)
				UartCMD[0] = 2 + 8*2;
			else
				UartCMD[0] = 2 + 8;

			UartCMD[1] = UART_HEAD_PULSEWIDTH_SET;
			for (i = 0; i < 8; i++)
			{
				num = vol_MapHeadNumber(i, False, False, False, True);
				if(isDoublePulse)
				{
					UartCMD[2+i*2] = g_headFirePulseWData.data[num*2];
					UartCMD[2+i*2+1] = g_headFirePulseWData.data[num*2+1]; 
				}
				else
					UartCMD[2+i] = g_headFirePulseWData.data[num]; 
				//    				CONSOL_Printf("PulseWidth %d:\r\n", UartCMD[2+i]);
			}

			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);

			g_headFirePulseWData.headUpdated = True;
			//				CONSOL_Printf("PULSE DATA Send to Head board\r\n");
		}
#endif

#ifdef WAVE_SET_FUN
		static INT8U pack_num = 0;
		static INT8U pack_all = 0;
		if(TRANSFER_MODE!=0)
		{
			if(pack_all == 0)
			{
				pack_all = (DATA_LEN-1)/WAVE_DATA_MAX_LEN +1;
				pack_num = 1;
			}

			if(pack_all == pack_num)
				UartCMD[0] = DATA_LEN + 4;
			else
				UartCMD[0] = WAVE_DATA_MAX_LEN + 4;

			if(TRANSFER_MODE == 1)
				UartCMD[1] = UART_HEAD_WAVE_SET;
			else if(TRANSFER_MODE == 2)
				UartCMD[1] = UART_HEAD_SWING_SET;
			else if(TRANSFER_MODE == 3)
				UartCMD[1] = UART_HEAD_POINT_SET;
			else if(TRANSFER_MODE == 4)
				UartCMD[1] = UART_HEAD_WAVE_NAME_SET;
			else if(TRANSFER_MODE == 18)  //43-25:波形映射表下发修改
				UartCMD[1] = UART_HEAD_WAVE_CHANNEL_SET;

			UartCMD[2] = pack_all;
			UartCMD[3] = pack_num;

			memcpy((void*)&UartCMD[4], (void*)&WAVE_BUF[(pack_num -1)*WAVE_DATA_MAX_LEN], UartCMD[0] - 4);
			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);
			//OSTimeDly(10);	 
			if(pack_all == pack_num)
			{
				if(TRANSFER_MODE == 1)
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_SET_READY;
				else if(TRANSFER_MODE == 2)
					((INT16U*)UartCMD)[0] = EP6_CMD_T_SWING_READY;
				else if (TRANSFER_MODE == 3)
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_POINT_FINISH;
				else if(TRANSFER_MODE == 4)
					((INT16U*)UartCMD)[0] = EP6_CMD_T_WAVE_NAME_FINISH;
				if(TRANSFER_MODE != 18)//43-25:波形映射表下发修改
					PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);	

				DATA_LEN = pack_all = pack_num = TRANSFER_MODE = 0;	
			}
			else
			{
				pack_num++;
				DATA_LEN -= WAVE_DATA_MAX_LEN;
			}
		}
#endif
#if defined(FUNC_WAVE_MAP)&& !defined(WAVE_SET_FUN)	
		static INT8U pack_num = 0;
		static INT8U pack_all = 0;
		if(TRANSFER_MODE!=0)
		{
			if(pack_all == 0)
			{
				pack_all = (DATA_LEN-1)/WAVE_DATA_MAX_LEN +1;
				pack_num = 1;
			}

			if(pack_all == pack_num)
				UartCMD[0] = DATA_LEN + 4;
			else
				UartCMD[0] = WAVE_DATA_MAX_LEN + 4;

			if(TRANSFER_MODE == 18)  //43-25:波形映射表下发修改
				UartCMD[1] = UART_HEAD_WAVE_CHANNEL_SET;

			UartCMD[2] = pack_all;
			UartCMD[3] = pack_num;

			memcpy((void*)&UartCMD[4], (void*)&WAVE_MAP_TABLE[(pack_num -1)*WAVE_DATA_MAX_LEN], UartCMD[0] - 4);
			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);

			if(pack_all == pack_num)
			{
				DATA_LEN = pack_all = pack_num = TRANSFER_MODE = 0;	
			}
			else
			{
				pack_num++;
				DATA_LEN -= WAVE_DATA_MAX_LEN;
			}
		}

#endif

#ifdef SUPPORT_HEAD_HEATER
		if (!g_UVheatTemprature.headUpdated)
		{
#ifdef CONVERSION_BOARD					
			INT8U head_num = 0,j = 0;
			UartCMD[0] = 2 + 24 +1;
			UartCMD[1] = UART_HEAD_HEAT_TEMPERATURE_CMD;

			if(Head_Board_Num == 0 || Head_Board_Num > MAX_HB_NUM)
				head_num = MAX_HB_NUM;
			else
				head_num = Head_Board_Num;

			for(j = 0;j < head_num;j++)
			{
				for(i = 0; i < MAX_HEAT_CHANNEL_PRE_HB; i++)
				{
					if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H)
					{
#ifdef MANUFACTURER_DYSS
						if(i < MAX_HEAT_CHANNEL)
						{
							if((g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/2) > 55)
								g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB] = 110;

							UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/2); 
						}
						else
							UartCMD[2 + i] = 0; 
#else
						if(i < MAX_HEAT_CHANNEL)
							UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/5); 
#endif				
					}
					else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H||
							headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H)
					{
						if(i < MAX_HEAT_CHANNEL)
							UartCMD[2+i] = (INT8U)(g_UVheatTemprature.data[Hardmap_to_PM_map[i] + j*MAX_HEAT_CHANNEL_PRE_HB]/5); 
					}
					else
					{
#ifdef MANUFACTURER_DYSS
						if((g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/2) > 55)
							g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB] = 110;
						if(i < MAX_HEAT_CHANNEL)
							UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/2); 
#else			
						if(i < MAX_HEAT_CHANNEL)
							UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i + j*MAX_HEAT_CHANNEL_PRE_HB]/5); 
#endif				
					}
				}
				if(j == 0)
				{
					if(HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H == headboardInfo.headBoardType)
					{
						for(i = 0; i < MAX_COLOR_NUMBER; i++)
							UartCMD[2+MAX_HEAT_CHANNEL_PRE_HB + i] = (INT8U)(g_UVheatTemprature.data[i + MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM]/5);
					}
					else
					{
						for(i = 0; i < MAX_COLOR_NUMBER; i++)
							UartCMD[2+MAX_EPSON_HEAD_CHANNEL + i] = (INT8U)(g_UVheatTemprature.data[i + MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM]/5);
					}
				}
				UartCMD[UartCMD[0] - 1] = j + 1;								
				while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
					OSTimeDly(1);	
			}


#else
			UartCMD[0] = 2 + 24;
			UartCMD[1] = UART_HEAD_HEAT_TEMPERATURE_CMD;

			for (i = 0; i < MAX_EPSON_HEAD_CHANNEL + MAX_COLOR_NUMBER; i++)
			{
				//INT8U num;	
				//num = vol_MapHeadNumber(i, False, True, False, False);
				if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H)
				{
#ifdef MANUFACTURER_DYSS
					if(i < MAX_HEAT_CHANNEL)
					{
						if((g_UVheatTemprature.data[i]/2) > 55)
							g_UVheatTemprature.data[i] = 110;

						UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i]/2); 
					}
					else
						UartCMD[2 + i] = 0; 
#else
					if(i < MAX_HEAT_CHANNEL)
						UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i]/5); 
					else if( i >= MAX_EPSON_HEAD_CHANNEL)
						UartCMD[2 + i] = (INT8U)(g_UVheatTemprature.data[i]/5); 
#endif				
				}
				else if(headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_3H||
						headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN5_4H)
				{
					if(i < MAX_HEAT_CHANNEL)
						UartCMD[2+i] = (INT8U)(g_UVheatTemprature.data[EpsonChannelMap[i]]/5); 
					else if( i >= MAX_EPSON_HEAD_CHANNEL)
						UartCMD[2 + i] = (INT8U)(g_UVheatTemprature.data[i]/5); 
				}
				else
				{
#ifdef MANUFACTURER_DYSS
					if((g_UVheatTemprature.data[i]/2) > 55)
						g_UVheatTemprature.data[i] = 110;
					if(i < MAX_HEAT_CHANNEL)
						UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i]/2); 
					else if( i >= MAX_EPSON_HEAD_CHANNEL)
						UartCMD[2 + i-(MAX_EPSON_HEAD_CHANNEL-8)] = (INT8U)(g_UVheatTemprature.data[i]/2); 
#else			
					if(i < MAX_HEAT_CHANNEL)
						UartCMD[2+EpsonChannelMap[i]] = (INT8U)(g_UVheatTemprature.data[i]/5); 
					else if( i >= MAX_EPSON_HEAD_CHANNEL)
						UartCMD[2 + i-(MAX_EPSON_HEAD_CHANNEL-8)] = (INT8U)(g_UVheatTemprature.data[i]/5); 
#endif				
				}


			}

			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);
#endif				


			g_UVheatTemprature.headUpdated = True;
		}
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		if(!bFPGADebugInfoUpdated)
		{
			UartCMD[0] = 2;
			UartCMD[1] = UART_HEAD_EPSON_REQ_FPGA_STATUS;
			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);

			bFPGADebugInfoUpdated = True;
		}
		if(bReadHeadBoardParam == 1)
		{
#ifdef CONVERSION_BOARD
			UartCMD[0] = 6;
			UartCMD[5] = 1;
#else
			UartCMD[0] = 5;
#endif
			UartCMD[1] = UART_HEAD_EPSON_REQ_HB_PARAM;
			UartCMD[2] = (INT8U)(HBReadAddr >> 8);
			UartCMD[3] = (INT8U)HBReadAddr;
			UartCMD[4] = (INT8U)HBReadLen;

			while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))  
				OSTimeDly(1);

			bReadHeadBoardParam = 2;
		}
#if defined(MANUFACTURER_TATE_EPSON_UV)&&defined(RIPSTAR_FLAT_EX)

		if(PIO_Get(&TATE_FLAT_STOP_MOVE)&&!IS_STOP)
		{
			IS_STOP = True;
			status_ReportStatus(STATUS_FTA + SciError_PaperJamX, STATUS_SET);
			buf[0] = 2;
			buf[1] = UART_URGENTSTOP_CMD;
			while(!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
				OSTimeDly(10);		
		}
#endif
#if defined( EPSON_LCD)
		if(bPMCaliSettingDirty)
		{
			bPMCaliSettingDirty = False;
			ConvertCaliSettingToFWWithFWFormat(&pmCaliSettingFWFormat, &epsonCaliSetting);
			ConvertCaliSettingToPMWithPMFormat(&epsonCaliSetting, &pmCaliSettingPMFormat);
			SaveEPSONCaliSetting();
		}
		if(bCaliSettingDirty)
		{
			bCaliSettingDirty = False;
			ConvertCaliSettingToPMWithPMFormat(&epsonCaliSetting, &pmCaliSettingPMFormat);
			ConvertCaliSettingToPMWithFWFormat(&epsonCaliSetting, &pmCaliSettingFWFormat);
			SaveEPSONCaliSetting();
			((INT16U*)UartCMD)[0] = EP6_CMD_T_CALIBRATION_DIRTY;
			PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
			//PushHugePipeData(EP6_CMD_T_CALIBRATION, (INT8U*)&pmCaliSettingPMFormat, sizeof(pmCaliSettingPMFormat));
		}
		if(bCaliStepDirty)
		{
			bCaliStepDirty = False;
			ConvertCaliSettingToPMWithPMFormat(&epsonCaliSetting, &pmCaliSettingPMFormat);
			ConvertCaliSettingToPMWithFWFormat(&epsonCaliSetting, &pmCaliSettingFWFormat);
			SaveEPSONCaliSetting();
			((INT16U*)UartCMD)[0] = EP6_CMD_T_STEP_DIRTY;
			PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
		}
#endif        
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
		if(bCleanSprayDirty)
		{
			bCleanSprayDirty = False;
			OSSemPend(CleaningParamSem, 0, &err);
			memcpy((void *)&cleanparam_EPSON_MICOLOR.bAutoSpray, (void *)&USB_CleanSpray, sizeof(USB_CleanSpray));			
			OSSemPost(CleaningParamSem);
			SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
					(INT8U*)&cleanparam_EPSON_MICOLOR.bAutoSpray - (INT8U*)&cleanparam_EPSON_MICOLOR, 
					sizeof(USB_CleanSpray));
		}
#endif        
#ifdef EPSON_BOTTOM_BOARD_V3
		if(bResistanceDirty)
		{
			INT8U Buff[2 + sizeof(HeaterInfo.ResistanceOffset)];
			bResistanceDirty = False;
			memset(Buff, 0, sizeof(Buff));
			Buff[0] = 18;
			Buff[1] = UART_SET_RESISTANCE_OFFSET;	 
			memcpy(Buff+2, (void *)HeaterInfo.ResistanceOffset, sizeof(HeaterInfo.ResistanceOffset));
			while (!UART_SendCMD(UART_HEAD_CHANNEL, Buff))  
				OSTimeDly(1);
		}
#endif

#ifdef FULGENCY_FUN
		if(Y_GOHOME_Dirty == Y_GOHOME_WAITE+10)
		{
			Y_GOHOME_Dirty = Y_GOHOME_IDLE;
			UV_CTR_PRT_Y_HOME_CLEAR();
		}
		else if(Y_GOHOME_Dirty > Y_GOHOME_WAITE)
			Y_GOHOME_Dirty++;

		if(FULGENCY_PLC_MASK&START_PRT)
		{
			FULGENCY_PLC_MASK &=(~START_PRT);
			((INT16U*)UartCMD)[0] = EP6_CMD_T_PRT_START;
			PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
		}
		if(FULGENCY_PLC_MASK&Y_UNLOAD_FINISH)
		{
			OSFlagAccept(status_FLAG_GRP, STATUS_MOVING|STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err);
			if (err == OS_NO_ERR && Y_GOHOME_IDLE == Y_GOHOME_Dirty)
			{
				FULGENCY_PLC_MASK &=(~Y_UNLOAD_FINISH);
				Y_GOHOME_Dirty = Y_GOHOME_CMD;
				FULGENCY_PRT_Y_GOHOME();
			}
			//((INT16U*)UartCMD)[0] = EP6_CMD_T_PRT_UNLOAD_FINISH;
			//PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
		}

		if(Head_Pump_Dirty)
		{
			Head_Pump_Dirty = False;
			//SaveLCDMenuConfig_EX();	
			SaveLCDMenuConfigEX_EM(&LCDMenuConfig_EX,
					(INT8U*)&LCDMenuConfig_EX.Head_PUMP_Mask - (INT8U*)&LCDMenuConfig_EX,
					sizeof(INT32U)*4 + 1);
			buf[0] = 19;
			buf[1] = UART_HEAD_SET_PUMP_MASK;
			buf[2] = LCDMenuConfig_EX.Head_PUMP_Mask;
			*((__packed INT32U*)&buf[3]) = LCDMenuConfig_EX.Head_PUMP1_ON;
			*((__packed INT32U*)&buf[7]) = LCDMenuConfig_EX.Head_PUMP1_OFF;
			*((__packed INT32U*)&buf[11]) = LCDMenuConfig_EX.Head_PUMP2_ON;
			*((__packed INT32U*)&buf[15]) = LCDMenuConfig_EX.Head_PUMP2_OFF;
			while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))  
				OSTimeDly(1);
		}
		if(Z_SAFE_Dirty)
		{
			Z_SAFE_Dirty = False;
			buf[0] = 6; //Length			
			buf[1] = UART_DSP_Z_SAFE_POSI; 
			*((__packed INT32U *)&buf[2]) = Z_SAFE_POSITION; //move_distance means target position
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(10);	
		}
		if(FIND_ORIGIN_Dirty)
		{
			status_ReportStatus(STATUS_MOVING, STATUS_SET);
			buf[0] = 3; //Length			
			buf[1] = UART_DSP_MANUAL_FIND_ORIGIN; 
			buf[2] = FIND_ORIGIN_Dirty; 
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(10);	
			FIND_ORIGIN_Dirty = False;
		}
#endif
		//空闲清洗
#ifdef MANUFACTURER_DGEN
		OS_ENTER_CRITICAL();
		if(cleanparam_EPSON_ALLWIN.IdleClean_intervalTime <= (IdleClean_Time /1000 / 60 / 60)
				&&cleanparam_EPSON_ALLWIN.IdleClean_intervalTime > 0)
		{
			IdleClean_Time = 0;
			ActiveCleanConfig = cleanparam_EPSON_ALLWIN.AutoCleanConfig;	//设置清洗强度
			nextband_autoClean = True;		
			status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		}
		OS_EXIT_CRITICAL();
#endif
#ifdef SUPPORT_MOTOR_CONTROL 		
		if(bMotionCfgDirty)
		{

			SaveMotionParam(&motionParam);
			bMotionCfgDirty = False;
		}
#endif	

#ifdef MANUFACTURER_DYSS		
		if(bDyssMeasureDirty)
		{
			if(LCDMenuConfig.cfg_bit&DYSS_MEASURE_PROTECT_SWITCH)
			{
				UartCMD[0] = 3;
				UartCMD[1] = UART_RPT_DSP_MEASURE_SENSOR_ONOFF;
				UartCMD[2] = 1;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, UartCMD))
					OSTimeDly(1);
			}
			else
			{
				UartCMD[0] = 3;
				UartCMD[1] = UART_RPT_DSP_MEASURE_SENSOR_ONOFF;
				UartCMD[2] = 0;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, UartCMD))
					OSTimeDly(1);
			}
			bDyssMeasureDirty = False;
		}
#endif

#if defined( EPSON_LCD)
		if(cleanParaDirty_ToPM)
		{
			INT8U Buff[2 + sizeof(cleanPara)];

			cleanParaDirty_ToPM = False;

			memset(Buff, 0, sizeof(Buff));
			Buff[0] = EP6_CMD_T_CLEAN_SETTING;
			memcpy(Buff+2, &cleanPara, sizeof(cleanPara));

			PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, sizeof(Buff), True);
			//PushHugePipeData(EP6_CMD_T_CLEAN_SETTING, (INT8U*)&cleanPara, sizeof(cleanPara));
		}
		if(mainUI_dirty)
		{
			mainUI_dirty = False;
#ifdef RIPSTAR_FLAT_EX
			SetBasePoint_All((void*)2, (float)mainUIPara.PrintOrigin,(float)mainUIPara.PrintYOrigin,(float)mainUIPara.PrintZOrigin, False, True);
#else
			SetBasePoint((void*)2, mainUIPara.PrintOrigin, False, True);
#endif
			OSSemPend(CalibrationParamSem, 0, &err);
			epsonCaliSetting.eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[mainUIPara.PassNum-1] = mainUIPara.StepModify;
			Set_PrintStep(mainUIPara.StepModify);
			OSSemPost(CalibrationParamSem);
			ConvertCaliSettingToPMWithPMFormat(&epsonCaliSetting, &pmCaliSettingPMFormat);
			ConvertCaliSettingToPMWithFWFormat(&epsonCaliSetting, &pmCaliSettingFWFormat);
			SaveEPSONCaliSetting();
		}
		if(mediaInfo_dirty)
		{
			mediaInfo_dirty = False;
			SetMediaInfo(mediaInfo.MediaOrigin, mediaInfo.MediaWidth, mediaInfo.Margin);
#ifdef RIPSTAR_FLAT_EX                
			LCDMenuConfig_EX.PRINT_Y_CONTINUE = mediaInfo.Prt_Y_Continue;	
			SaveLCDMenuConfig_EX();
#endif                
			mainUIPara_ToPM.PrintOrigin = mediaInfo.MediaOrigin;
			mainUIPara.PrintOrigin = mediaInfo.MediaOrigin;
			memcpy(&mediaInfo_ToPM, &mediaInfo, sizeof(mediaInfo_ToPM));
			((INT16U*)UartCMD)[0] = EP6_CMD_T_MEDIA;
			PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, UartCMD, 2, False);
		}
		if(printQuality_dirty)
		{
			printQuality_dirty = False;
			SetPrintQuality(printQuality.PrintQuality);
			printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
		}
#endif      

#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP) && defined(EPSON_CLEAN_UPDOWN)        
		if(printratio_dirty)
		{				       
			if(OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,&err),err == OS_NO_ERR) //Waiting moving stop	
			{
				INT8U tmp[2];
				printratio_dirty = False;
				tmp[0] = 2;
				tmp[1] = 0x59;
				UART_SendCMD(UART_DSP_CHANNEL, tmp);	
				//RegPostCapping();?
			}
		}
#endif	
		if(bFactoryDataDirty)
		{
			//U8 err;
			bFactoryDataDirty  = False;
			if(MaxGroupNumber_is_group_num)
			{
				factoryDataEx.MaxGroupNumber = absv(factoryData.group_num);
				SaveFactoryDataEx_EPSON(&factoryDataEx);
			}
			OSSemPend(LCDMenuConfigureSem, 0, &err);					
			LCDMenuConfig.MediaWidth = PAPER_MEDIA_WIDTH_INCH;
			OSSemPost(LCDMenuConfigureSem);

			SaveLCDMenuConfig();
		}
#if defined(HEAD_EPSON_GEN5)&&!defined(EPSON_4H)
		if((fwInfo.hd_version  & 0xFFFFFFFF >= 0x03030101) && !Already_Reset_FPGA&&motionInitS2)
		{
			if(OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, &err))
			{
				FPGA_RESETAGAIN();
				Already_Reset_FPGA = True;
			}
		}				
#endif		
#if defined (MICOLOR_AUTOFUCTION)
		if(coreLEDFlashDly % 4 == 0) //check per 0.2s 
		{
			if(motionInitS2 && headInitS2)
			{
				if(!IsMediaFixed())
					MediaMeasured = False;
				if(!MediaMeasured  && LCDMenuConfig.AutoMeasure)
				{
					UIAction_MeasureMedia((void *)2, 0);
				}
			}
			{
				static INT8U bStartMon = False;
				static INT32U StartTime = 0;
#ifdef DGEN_AUTOFUCTION
#define AUTOGOHOME_IDLE_TIME    (1*60*1000)   //2 minute
#else
#define AUTOGOHOME_IDLE_TIME    (2*60*1000)   //2 minute
#endif

				if(!bStartMon)
				{
					StartTime = (INT32U)OSTimeGet();
					bStartMon = True;
				}
				if ((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)&&(LCDMenuConfig.AutoGoHome))
				{
#ifdef EPSON_CLEAN_UPDOWN
					if(OSTimeGet() - StartTime > AUTOGOHOME_IDLE_TIME && curPos.x > cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos + 50 )
#else
						if(OSTimeGet() - StartTime > AUTOGOHOME_IDLE_TIME && curPos.x > cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos )
#endif							
						{
							UIAction_X_GoHome(NULL);
							StartTime = (INT32U)OSTimeGet();
						}
				}
				else
					StartTime = (INT32U)OSTimeGet();
			}
		}
#endif
#endif
#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&(defined(MB_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR))
		Control_PumpControl(INK_LEVEL_MASK);
#endif  

#ifdef FULGENCY_FUN
		Main_Color_Level_Scan();
#endif

#if defined(FULGENCY_FUN)||defined(MANUFACTURER_DYSS)
		Waste_Ink_CHECK();
#endif	

#if defined(HEAD_RICOH_G4)||defined(EPSON_FLASH_IDLE) 
#ifdef EPSON_FLASH_IDLE
		if(((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ||(OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) )
				&&(curPos.x < (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos +100) && curPos.x > (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos -100))
				&&(curPos.z < (cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos +10) && curPos.z > (cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos -10))&&cleanPara.flash)
#else
			if(((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ||( (OSFlagAccept(status_FLAG_GRP, CMD_PAUSE |STATUS_PRINT|STATUS_PAUSE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&& (OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ))
					&&(curPos.x < 10 && curPos.x > -10)&&cleanPara.flash)
#endif		
			{
				if(!flash_idle_on)
				{
					if(flash_wait_50ms == 0)
					{
						if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
						{
							buf[0] = 4;
							buf[1] = UART_HEAD_EPSON_JOBSTART;
							buf[2] = 1; //flash Job
							buf[3] = 2;
							while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
								OSTimeDly(10);
							flash_idle_send_jobstart = True;
						}
						else
						{
#ifdef HEAD_RICOH_G4            
#ifndef CLOSE_SSHAKE                
							SetSafeCmd(rFPGA_COMMAND_END_SSHAKE);
#endif
#endif
						}
						buf[0] = 6;
						buf[1] = UART_HEAD_EPSON_PASSSTART;
						buf[2] = 0;
						buf[3] = 0;
						buf[4] = 0;
						buf[5] = 0;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
							OSTimeDly(10);
					}
					if(flash_wait_50ms >= 1)//数值在50左右频繁报头版fpga错误，数值越小概率越低
					{
#ifdef EPSON_FLASH_IDLE
						EPSON_FLASH_IDLE_STAR(cleanparam_EPSON_ALLWIN.Idle_FlashFreqInterval, 0xF);
#else
						FPGA_START_FLASH_ALLWIN(cleanparam_EPSON_ALLWIN.Config[2].FlashFreqInterval, 0xF);
#endif
						flash_idle_on = True;
						flash_wait_50ms = 0;
					}
					else
						flash_wait_50ms++;

				}
			}
			else
			{
				False_Idle_Off();
			}
#endif

#if !defined(HEAD_EPSON_GEN5)&&	 !defined(HEAD_RICOH_G4)//RICOH暂时不支持
#error
		//for ricoh, maybe, it need flash when idle. depend on the cleaning way. 
		//  if it is capped when idle, it need not flash. else, need it. 
		if (
				(cleanPara.flash == True)				
				&& motionInitS2 && headInitS2
				&& (curPos.x < printer.flashFarPos) 
				&& ((  OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, &err) 
						||OSFlagAccept(status_FLAG_GRP, STATUS_MOVING_FLASH, OS_FLAG_WAIT_SET_ALL, &err))
					&& (!headBusUsedByPrintData)
				   )
		   )
		{
#ifdef HEAD_RICOH_G4
			if ( !printerFlashing )
			{
				//Start Flash here
				OSFlagPend(status_FLAG_GRP, CMD_EPSON_OPERATION, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting last operator stop
				CMD_Epson_Operate_Type = CMD_RICOH_T_START_IDEL_SPRAY;
				status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);

				printerFlashing = True;	
			}
#else
			if ((!printerFlashing) || (cleanParaDirty))
			{ 
				//Start Flash here
				FPGA_START_FLASH(cleanPara.flash_interval);
				printerFlashing = True;	
				cleanParaDirty = False;
			}
#endif
		}
		else
		{
#ifdef HEAD_RICOH_G4
			if (printerFlashing && CMD_Epson_Operate_Type != CMD_RICOH_T_STOP_IDEL_SPRAY)
			{
				OSFlagPend(status_FLAG_GRP, CMD_EPSON_OPERATION, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting last operator stop
				CMD_Epson_Operate_Type = CMD_RICOH_T_STOP_IDEL_SPRAY;
				status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);
			}
#else
			if (printerFlashing)
			{
				//Stop Flash here
				FPGA_STOP_FLASH();
				printerFlashing = False;
			}
#endif
		}
#endif
		OSTimeDly(sleep);
	}
}

/* ********************************************************************* */
/* Global functions */

void main(void)
{
	PIO_InitializeInterrupts(IRQ_PRI_PORT);

	CPU_INT08U  err;

	CPU_IntDis();
	CSP_IntInit();
	CPU_Init();
	CPU_IntDis();

	OSInit();
	IICSem = OSSemCreate(1);             /* Random number semaphore */
	IIC_KeyboardSem = OSSemCreate(1);
	CleaningParamSem = OSSemCreate(1);
	CalibrationParamSem = OSSemCreate(1);
	LCDMenuConfigureSem = OSSemCreate(1);
	(void)OSTaskCreateExt(TaskStart,
			(void *)0,
			&TaskStartStk[START_TASK_STK_SIZE - 1],
			START_TASK_PRIO,
			START_TASK_ID,
			&TaskStartStk[0],
			START_TASK_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if (OS_TASK_NAME_EN > 0u)
	OSTaskNameSet(APP_CFG_TASK_START_PRIO, (INT8U *)"Start", &err);
#endif
	OSStart();                              /* Start multitasking                                       */
}


/* ********************************************************************* */

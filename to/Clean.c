#include "os_includes.h"
#include "status.h"
#include "miscellance.h"
#include "control.h"
#include "consol.h"

#include "global.h"
#include "fpga.h"
#include "clean.h"
#include "job.h"
#include "uv.h"
#include "calibration.h"
#include "HugeBuf.h"
#include "motion.h"
#include "display.h"
#include "../LCD_GUI/Menu_GUI.h"
#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
extern INT8U ResetZTCCoor(void);
#endif
#if !(defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
#error "only support HEAD_EPSON_GEN5 and HEAD_RICOH_G4"
#endif

#ifdef ALLWIN_MEDIA_KEY
#if !defined(ALLWIN_EPSON_SAME)
#error //ALLWIN_MEDIA_KEY 只作用与ALLWIN
#endif
#ifdef UV_PRINTER
#error //ALLWIN_MEDIA_KEY使用了UV_PRINTER的GPIO 不能同时定义
#endif
#ifdef HUMAN_Y_STATUS_OUTPUT
#error //ALLWIN_MEDIA_KEY使用了UV_PRINTER的GPIO 不能同时定义
#endif
const Pin ALLWIN_KEY_1 = PIN_DSP_UV_CONTROL_2;
const Pin ALLWIN_KEY_2 = PIN_DSP_UV_CONTROL_1;
INT8U ALLWIN_KEY_STATUS = 3;
INT8U ALLWIN_KEY_SPEED = 1;
#endif
#ifdef  EPSON_CLEAN_UPDOWN_WIPER_EX
#ifndef EPSON_CLEAN_UPDOWN
#error
#endif
#endif

#if defined(EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
INT8U Backhaul_Flag = False;
#endif    //EPSON_CLEAN_UPDOWN
extern volatile INT8U nextband_autoClean;
extern volatile INT8U nextband_autoSpray;
#ifdef FAST_CLEAN
extern volatile INT8U nextband_autoCleanUseFlash;
#endif
extern struct Struct_JobStart job_info;
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
struct USB_CleanSpray_Config USB_CleanSpray;
#endif                

INT8U CMD_Epson_Operate_Type = 0;

#ifdef FULGENCY_FUN
extern const Pin FULGENCY_WASTE_INK ;
#endif

#ifdef DYSS_UV_CLEAN

#define DYSS_CLEAN_NONE 1
#define DYSS_CLEAN_MANUAL 1

INT8U DYSS_CLEAN_FLASH = 0;

#define DYSS_CLEAN_SENSOR1_ON 1
#define DYSS_CLEAN_SENSOR2_ON 0


const Pin DYSS_CLEAN_SENSOR1 = PIN_UV_STAT_LP1_SH_CL_I; 
const Pin DYSS_CLEAN_SENSOR2 = PIN_UV_STAT_LP1_SH_OP_I; 
const Pin DYSS_CLEAN_SENSOR3 = PIN_UV_STAT_LP2_SH_CL_I;
//const Pin UVStatPinLP2ShOp = PIN_UV_STAT_LP2_SH_OP_I; 
const Pin DYSS_CLEAN_SENSOR4 = PIN_DYSS_UV_CLEAN_J10; 
const Pin DYSS_CLEAN_SENSOR5 = PIN_DYSS_UV_CLEAN_J12;

const Pin DYSS_CLEAN_MOTOR1 = PIN_INK_SUPPLY_CTRL1;
const Pin DYSS_CLEAN_MOTOR2 = PIN_INK_SUPPLY_CTRL2;
const Pin DYSS_CLEAN_MOTOR3 = PIN_INK_SUPPLY_CTRL3;


const Pin DYSS_CLEAN_MOVEING_MOTOR1 = PIN_INK_SUPPLY_CTRL4;
const Pin DYSS_CLEAN_MOVEING_MOTOR2 = PIN_INK_SUPPLY_CTRL5;
const Pin DYSS_CLEAN_TURN_MOTOR1 = PIN_INK_SUPPLY_CTRL7;
const Pin DYSS_CLEAN_TURN_MOTOR2 = PIN_INK_SUPPLY_CTRL6;

void CONTROL_CLOSE_SUCKER(INT8U HeadIndex)
{
	if(HeadIndex == 0)
		PIO_Clear(&DYSS_CLEAN_MOTOR1);
	else if(HeadIndex == 1)
		PIO_Clear(&DYSS_CLEAN_MOTOR2);
	else if(HeadIndex == 2)
		PIO_Clear(&DYSS_CLEAN_MOTOR3);
}

void CONTROL_OPEN_SUCKER(INT8U HeadIndex)
{
	if(HeadIndex == 0)
		PIO_Set(&DYSS_CLEAN_MOTOR1);
	else if(HeadIndex == 1)
		PIO_Set(&DYSS_CLEAN_MOTOR2);
	else if(HeadIndex == 2)
		PIO_Set(&DYSS_CLEAN_MOTOR3);
}

void DYSS_CLEAN_MOVEING_MOTOR_POS(void)
{
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	PIO_Set(&DYSS_CLEAN_MOVEING_MOTOR1);
	PIO_Clear(&DYSS_CLEAN_MOVEING_MOTOR2);
}

void DYSS_CLEAN_MOVEING_MOTOR_NEG(void)
{
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	PIO_Clear(&DYSS_CLEAN_MOVEING_MOTOR1);
	PIO_Set(&DYSS_CLEAN_MOVEING_MOTOR2);
}

void DYSS_CLEAN_MOVEING_MOTOR_STOP(void)
{
	PIO_Clear(&DYSS_CLEAN_MOVEING_MOTOR1);
	PIO_Clear(&DYSS_CLEAN_MOVEING_MOTOR2);
	status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
}

void DYSS_CLEAN_TURN_MOTOR_POS(void)
{
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	PIO_Set(&DYSS_CLEAN_TURN_MOTOR1);
	PIO_Clear(&DYSS_CLEAN_TURN_MOTOR2);
}

void DYSS_CLEAN_TURN_MOTOR_NEG(void)
{
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	PIO_Clear(&DYSS_CLEAN_TURN_MOTOR1);
	PIO_Set(&DYSS_CLEAN_TURN_MOTOR2);
}

void DYSS_CLEAN_TURN_MOTOR_STOP(void)
{
	PIO_Clear(&DYSS_CLEAN_TURN_MOTOR1);
	PIO_Clear(&DYSS_CLEAN_TURN_MOTOR2);
	status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
}

void DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR1(void)
{
	if(PIO_Get(&DYSS_CLEAN_SENSOR1)!= DYSS_CLEAN_SENSOR1_ON)	
	{	
		INT8U err;
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR1, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR1);
		DYSS_CLEAN_MOVEING_MOTOR_NEG();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR1);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR1);
	}
}

void DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR2(void)
{
	INT8U err;
	if(PIO_Get(&DYSS_CLEAN_SENSOR2)== DYSS_CLEAN_SENSOR1_ON)
		return;
	if(PIO_Get(&DYSS_CLEAN_SENSOR1)== DYSS_CLEAN_SENSOR1_ON)	
	{	
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR2, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR2);
		DYSS_CLEAN_MOVEING_MOTOR_POS();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR2);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR2);
	}
	else if(PIO_Get(&DYSS_CLEAN_SENSOR3)== DYSS_CLEAN_SENSOR1_ON)
	{
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR2, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR2);
		DYSS_CLEAN_MOVEING_MOTOR_NEG();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR2);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR2);
	}
	else
	{
		DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR1();
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR2, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR2);
		DYSS_CLEAN_MOVEING_MOTOR_POS();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR2);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR2);
	}
}

void DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR3(void)
{
	if(PIO_Get(&DYSS_CLEAN_SENSOR3)!= DYSS_CLEAN_SENSOR1_ON)	
	{	
		INT8U err;
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR3, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR3);
		DYSS_CLEAN_MOVEING_MOTOR_POS();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR3);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR3);
	}
}
void DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR3_BYTIME(INT16U ms)
{
	if(PIO_Get(&DYSS_CLEAN_SENSOR3)!= DYSS_CLEAN_SENSOR1_ON)	
	{	
		INT8U err;
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR3, DYSS_CLEAN_MOVEING_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR3);
		DYSS_CLEAN_MOVEING_MOTOR_POS();
		if(ms > 0xFFFF)
			OSTimeDly(0xFFFF);
		else
			OSTimeDly(ms);
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR3);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR3);
	}
}

void DYSS_CLEAN_MOVEING_MOTOR_INIT(void)
{
	DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR1();
}

void DYSS_CLEAN_TURN_MOTOR_SENSOR4(void)
{
	if(PIO_Get(&DYSS_CLEAN_SENSOR4)!= DYSS_CLEAN_SENSOR2_ON)	
	{	
		INT8U err;
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR4, DYSS_CLEAN_TURN_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR4);
		DYSS_CLEAN_TURN_MOTOR_NEG();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR4);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR4);
	}
}

void DYSS_CLEAN_TURN_MOTOR_SENSOR5(void)
{
	if(PIO_Get(&DYSS_CLEAN_SENSOR5)!= DYSS_CLEAN_SENSOR2_ON)	
	{	
		INT8U err;
		PIO_ConfigureIt(&DYSS_CLEAN_SENSOR5, DYSS_CLEAN_TURN_MOTOR_STOP);
		PIO_EnableIt(&DYSS_CLEAN_SENSOR5);
		DYSS_CLEAN_TURN_MOTOR_POS();
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		PIO_DisableIt(&DYSS_CLEAN_SENSOR5);
		PIO_UnConfigureIt(&DYSS_CLEAN_SENSOR5);
	}
}

void DYSS_CLEAN_TURN_MOTOR_INIT(void)
{
	DYSS_CLEAN_TURN_MOTOR_SENSOR4();
}

INT8U SetCleanParam_EPSON_ALLWIN_Flash_Time(void * pData, int Index, float Data)
{
	INT8U err;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	if(Index == 2)
		Index = 1;
	else
		Index = 0;
	if(Data <= 0 ||Data > 25.5 )
		return False;
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_ALLWIN.Config[Index].FlashTime= Data*10;
	OSSemPost(CleaningParamSem);
	
	SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
								   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].FlashTime- (INT8U*)&cleanparam_EPSON_ALLWIN, 
								   sizeof(cleanparam_EPSON_ALLWIN.Config[Index].FlashTime));
	return True;
}
float GetCleanParam_EPSON_ALLWIN_Flash_Time(void * pData, int Index)
{
	U8 err;
	if(Index == 2)
		Index = 1;
	else
		Index = 0;
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = (float)(cleanparam_EPSON_ALLWIN.Config[Index].FlashTime)/10;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
void DYSS_DO_FLASH(INT8U index)
{
	INT8U buf[10] = {0},err; 
	INT16U flash_time = 0;

	if(index <=1)
		flash_time = (cleanparam_EPSON_ALLWIN.Config[index].FlashTime)*100;
	else
#ifdef RICOH_3H
		flash_time = 7000;
#else
		flash_time = 2000;
#endif

	status_ReportStatus(STATUS_MOVING, STATUS_SET);		

	buf[0] = 8; //Length									
	buf[1] = UART_MOVEDISTANCE_CMD; 
	buf[2] = 1; //1,2 X, 3,4 Y, 5,6 Z
	buf[3] = printer.movSpeed;
	*((__packed INT32S *)&buf[4]) = 0;

	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	
	buf[0] = 6;
	buf[1] = UART_HEAD_EPSON_PASSSTART;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
		OSTimeDly(10);
	
	FPGA_START_FLASH_ALLWIN(1000, 0xF);
	OSTimeDly(flash_time);
	FPGA_STOP_FLASH_Safe();
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = 0;
	buf[6] = printer.movSpeed;
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	
}
#endif
#ifdef DYSS_UV_CLEAN

volatile INT8U Power_Off_Mode = False;
INT8U  DoPower_Off_Mode_DYSS(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT|STATUS_CLEANING|STATUS_MOVING|STATUS_INI, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	
	if(cleaning)
	{
		nextband_autoClean = False;
		DYSS_CLEAN_FLASH = 0;
		Power_Off_Mode = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}

#endif
#if defined (EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)

struct EPR_CleanParaEpsonAllwin cleanparam_EPSON_ALLWIN;
volatile INT8U ActiveCleanConfig = 0xFF;

void SetCleanParam_EPSON_Allwin(CleanParaType_EPSON_ALLWIN_USB * pUsb,INT8U index)
{
	INT8U err,i;
	struct CleanParaEpsonAllwin_Config * pActiveConfig;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = pUsb->Carriage_X_SuckPos;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = pUsb->HeadBox_Z_SuckPos;
	
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = pUsb->HeadBox_Z_WipePos;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = pUsb->HeadBox_Z_FlashPos;
	if(pUsb->Idle_FlashFreqInterval <= 1000)
		cleanparam_EPSON_ALLWIN.Idle_FlashFreqInterval = pUsb->Idle_FlashFreqInterval;
	for(i =0; i< 4; i++)
	{
		cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[i] = pUsb->WiperPos_Y[i];
		cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[i] = pUsb->Carriage_X_WipePos_Start[i];
		cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[i] = pUsb->Carriage_X_WipePos_End[i];
	}
	
	if(index >= MAX_CLEAN_CONFIG)
		pActiveConfig = cleanparam_EPSON_ALLWIN.Config;
	else
		pActiveConfig = &cleanparam_EPSON_ALLWIN.Config[index];
	
	if(pUsb->SuckTimes & 0x80)
		pActiveConfig->CleanType = ACT_QUICKSUCK;
	else
		pActiveConfig->CleanType = ACT_FULLLOOP;
	
	pActiveConfig->SuckTimes = pUsb->SuckTimes & (~0x80);
	pActiveConfig->SuckInkTime = pUsb->SuckInkTime; //unit:0.1s;
	pActiveConfig->InputAirTime = pUsb->InputAirTime; //unit:0.1s;
	pActiveConfig->SuckWasteInkTime = pUsb->SuckWasteInkTime; //unit:0.1s;
	
	pActiveConfig->WipeTimes = pUsb->WipeTimes; //only for ACT_SUCKTHENWIPE
	pActiveConfig->Carriage_X_Wipe_Speed = pUsb->Carriage_X_Wipe_Speed;
	
	pActiveConfig->FlashFreqInterval = pUsb->FlashFreqInterval; //unit: us.
	pActiveConfig->FlashTime = pUsb->FlashTime; //unit:0.1s.
	pActiveConfig->FlashCycle = pUsb->FlashCycle; //unit:0.1s.
	pActiveConfig->FlashIdleInCycle = pUsb->FlashIdleInCycle; //unit:0.1s.
	
	cleanparam_EPSON_ALLWIN.Runtime.HeadMask = pUsb->HeadMask;
	OSSemPost(CleaningParamSem);
}

void OutCleanParam_EPSON_Allwin(CleanParaType_EPSON_ALLWIN_USB * pUsb,INT8U index)
{
	INT8U err,i;
	struct CleanParaEpsonAllwin_Config * pActiveConfig;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	ActiveCleanConfig = 0xFF;
	
	pUsb->Carriage_X_SuckPos = cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
	pUsb->HeadBox_Z_SuckPos = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;
	
	pUsb->HeadBox_Z_WipePos = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos;
	pUsb->HeadBox_Z_FlashPos = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;
	pUsb->Idle_FlashFreqInterval = cleanparam_EPSON_ALLWIN.Idle_FlashFreqInterval;
	for(i =0; i< 4; i++)
	{
		pUsb->WiperPos_Y[i] = cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[i];
		pUsb->Carriage_X_WipePos_Start[i] = cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[i];
		pUsb->Carriage_X_WipePos_End[i] = cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[i];
	}
	
	if(index >= MAX_CLEAN_CONFIG)
		pActiveConfig = cleanparam_EPSON_ALLWIN.Config;
	else
		pActiveConfig = &cleanparam_EPSON_ALLWIN.Config[index];
	
	pUsb->SuckTimes = pActiveConfig->SuckTimes;
	if(pActiveConfig->CleanType == ACT_QUICKSUCK)
		pUsb->SuckTimes |= 0x80;
	
	pUsb->SuckInkTime = pActiveConfig->SuckInkTime; //unit:0.1s;
	pUsb->InputAirTime = pActiveConfig->InputAirTime; //unit:0.1s;
	pUsb->SuckWasteInkTime = pActiveConfig->SuckWasteInkTime; //unit:0.1s;
	
	pUsb->WipeTimes = pActiveConfig->WipeTimes; //only for ACT_SUCKTHENWIPE
	pUsb->Carriage_X_Wipe_Speed = pActiveConfig->Carriage_X_Wipe_Speed;
	
	pUsb->FlashFreqInterval = pActiveConfig->FlashFreqInterval; //unit: us.
	pUsb->FlashTime = pActiveConfig->FlashTime; //unit:0.1s.
	pUsb->FlashCycle = pActiveConfig->FlashCycle; //unit:0.1s.
	pUsb->FlashIdleInCycle = pActiveConfig->FlashIdleInCycle; //unit:0.1s.
	
	pUsb->HeadMask = cleanparam_EPSON_ALLWIN.Runtime.HeadMask;
	OSSemPost(CleaningParamSem);
}

void SetAutoClean_EPSON_Allwin(CleanParaType * pUsb)
{
	INT8U err;
	
	cleanPara.structsize = pUsb->structsize;
#if !defined(HEAD_RICOH_G4)&&!defined(EPSON_FLASH_IDLE)	
	cleanPara.flash = pUsb->flash;
#endif	
	cleanPara.pause_gohome = pUsb->pause_gohome;
	cleanPara.flash_interval = pUsb->flash_interval;
	cleanPara.longflash_beforeprint = pUsb->longflash_beforeprint;
	cleanPara.manualCleanTimes = pUsb->manualCleanTimes;	
	cleanPara.longflash_passInterval = pUsb->longflash_passInterval;
	cleanPara.blowInk_passInterval = pUsb->blowInk_passInterval;
	cleanPara.flashTimes = pUsb->flashTimes;
	cleanPara.pauseIntervalAfterClean = pUsb->pauseIntervalAfterClean;
	cleanPara.pauseIntervalAfterBlowInk = pUsb->pauseIntervalAfterBlowInk;
	cleanPara.paraFlag = pUsb->paraFlag;		
	if(!kb_installed)
	{
		OSSemPend(CleaningParamSem, 0, &err);
		
		cleanparam_EPSON_ALLWIN.bAutoClean = pUsb->autoClean_passInterval != 0;
		if(pUsb->autoClean_passInterval != 0)
			cleanparam_EPSON_ALLWIN.AutoClean_passInterval = pUsb->autoClean_passInterval;
		
		cleanparam_EPSON_ALLWIN.AutoCleanConfig = pUsb->autoClean_way;
		
		OSSemPost(CleaningParamSem);
		
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.bAutoClean - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   3);
	}
}

void OutAutoClean_EPSON_Allwin(CleanParaType * pUsb)
{
	INT8U err;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	if(cleanparam_EPSON_ALLWIN.bAutoClean)
		pUsb->autoClean_passInterval = cleanparam_EPSON_ALLWIN.AutoClean_passInterval;
	else
		pUsb->autoClean_passInterval = 0;
	pUsb->autoClean_way = cleanparam_EPSON_ALLWIN.AutoCleanConfig;
	
	OSSemPost(CleaningParamSem);
}

void InitialCleanParam_EPSON_Allwin()
{
	INT8U i;
	INT8U err;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	cleanparam_EPSON_ALLWIN.flag = EPR_CLEANPARA_FLAG;
	cleanparam_EPSON_ALLWIN.structsize = sizeof(struct EPR_CleanParaEpsonAllwin);
	
    cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 2800;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 0;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 0;
	for(i=0; i<MAX_EPSONHEAD; i++)
	{
		cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[i] = 2240 + i*700;
		cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[i] = 1000 + i*3000;
		cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[i] = 3840 + i*3000 + 3000;
	}
	
	for(i=0; i< MAX_CLEAN_CONFIG; i++)
	{
		cleanparam_EPSON_ALLWIN.Config[i].CleanType = ACT_FULLLOOP;
		cleanparam_EPSON_ALLWIN.Config[i].SuckTimes = 1;
		switch(i)
		{
#if defined(MANUFACTURER_HUMAN_EPSON)        
		case 0:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 30;
			break;
		case 1:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 20;
			break;			
		case 2:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 10;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 100;
			break;
#elif (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))
		case 0:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 35;
			break;
		case 1:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 20;
			break;			
		case 2:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 10;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 1800;
			break;
#else
		case 0:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 50;
			break;
		case 1:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 30;
			break;			
		case 2:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 10;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckInkTime = 1800;
			break;			
#endif
		}
		
#if defined(MANUFACTURER_HUMAN_EPSON)			
		cleanparam_EPSON_ALLWIN.Config[i].InputAirTime = 30;
#else
		cleanparam_EPSON_ALLWIN.Config[i].InputAirTime = 50;
#endif
		
		switch(i)
		{
#if (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))
		case 0:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 20;
			break;
		case 1:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 15;
			break;			
		case 2:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 10;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 20;
			break;
#elif defined(ALLWIN_EPSON_SAME)
		case 0:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 50;
			break;
		case 1:	
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 40;
			break;
		case 2:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 30;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 50;
			break;
#elif defined(MANUFACTURER_HUMAN_EPSON)
		case 0:
		case 1:		
		case 2:
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 30;
			break;						
#else
		case 0:
		case 1:		
		case 2:
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 10;
			break;	
#endif
		}
		
		cleanparam_EPSON_ALLWIN.Config[i].WipeTimes = 1;
#ifdef EPSON_CLEAN_UPDOWN_TATE_8H_RICOH
		cleanparam_EPSON_ALLWIN.Config[i].Carriage_X_Wipe_Speed = TATE_CLEAN_CZ_SPEED_LEVEL;
#else
		cleanparam_EPSON_ALLWIN.Config[i].Carriage_X_Wipe_Speed = 7;
#endif
		
#if defined(ALLWIN_EPSON_SAME)	
		cleanparam_EPSON_ALLWIN.Config[i].FlashFreqInterval = 1536;
		cleanparam_EPSON_ALLWIN.Config[i].FlashTime = 150;
		cleanparam_EPSON_ALLWIN.Config[i].FlashCycle = 50;
		cleanparam_EPSON_ALLWIN.Config[i].FlashIdleInCycle = 15;
#else
		cleanparam_EPSON_ALLWIN.Config[i].FlashFreqInterval = 1000;
		cleanparam_EPSON_ALLWIN.Config[i].FlashTime = 120;
		cleanparam_EPSON_ALLWIN.Config[i].FlashCycle = 40;
		cleanparam_EPSON_ALLWIN.Config[i].FlashIdleInCycle = 10;
#endif				
	}
	cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig = 2;
	//when InitialCleanParam, calibration module is NOT ready.
	
	if(factoryData.HeadType == HeadNo_Epson_Gen5) 
	{
		INT8U head_group = (COLOR_NUMBER_CALIBRATION * factoryDataEx.YInterleaveNum + NOZZLE_LINE_COUNT_PER_HEAD - 1)/ 
			NOZZLE_LINE_COUNT_PER_HEAD;
		switch(EPSON_PERMIT_GROUPNUM * head_group)
		{
		case 1:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
			break;
		case 2:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 3;
			break;
		case 3:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 7;
			break;
		case 4:
		default:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 0xF;
			break;
		}
	}
	else
	{
		//wait more implement.
		switch(EPSON_PERMIT_GROUPNUM)
		{
		case 1:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
			break;
		case 2:
		default:
			cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 3;
			break;
		}
	}
	cleanparam_EPSON_ALLWIN.bAutoClean = 1;
	cleanparam_EPSON_ALLWIN.AutoClean_passInterval = 100;
	cleanPara.autoClean_passInterval = 100;
	cleanparam_EPSON_ALLWIN.AutoCleanConfig = 2;
	
#if defined(MANUFACTURER_BEMAJET)
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 950;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 2000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4000;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 1250;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8000;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 500;
#elif defined(A1802)	
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 580;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 1700;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4500;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 770;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 5000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8000;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 850;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 210;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 500;
#elif defined(ALLWIN_EPSON_SAME)
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
#ifdef EPSON_4H
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2400;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 2000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4000;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3100;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 6200;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8600;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 2700;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 800;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 1000;	
#else
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2300;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 2000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 3800;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3100;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 5200;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 7000;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 1600;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 400;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 400;	
#endif
#elif defined(MANUFACTURER_HUMAN_EPSON)
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2420;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 1000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4000;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3080;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 7500;	
#elif (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 1850;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 2000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4000;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 2550;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8000;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 3500;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 0;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 2500;
#elif defined(EPSON_CLEAN_UPDOWN_FULGENCY)
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4840;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3200;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8840;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = Z_SUCK_POSITION;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = Z_WIPE_POSITION;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = Z_FLASH_POSITION;	
#else
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4840;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3200;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8840;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 2700;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 1150;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 1000;	    
#endif
	cleanparam_EPSON_ALLWIN.Idle_FlashFreqInterval = 1000;
	cleanparam_EPSON_ALLWIN.IdleClean_intervalTime = 0;
	OSSemPost(CleaningParamSem);
}

INT8U ReadCleanParamEPR_EPSON_Allwin(struct EPR_CleanParaEpsonAllwin *pCleanPara)
{
	INT8U err,ret;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(struct EPR_CleanParaEpsonAllwin) + sizeof(EPM_Head);
	if(length > EPR_CLEANPARAM_DATA_SIZE)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	if(!IIC_ReadEEPRom_Head(EPR_CLEANPARAM_DATA_OFFSET, &head))
	{
		OSSemPost(CleaningParamSem);
		return False;
	}
	
	memset(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonAllwin));
	if(head.flag == EPR_CLEANPARA_FLAG_3B5A) //it is old structure before add the head.
	{
		length = head.version; //really, it is element structsize of old structure.
		if(length > sizeof(struct EPR_CleanParaEpsonAllwin))
			length = sizeof(struct EPR_CleanParaEpsonAllwin);
		if(!IIC_ReadEEPRom_Ex(EPR_CLEANPARAM_DATA_OFFSET, (INT8U * )pCleanPara, &length))
		{
			OSSemPost(CleaningParamSem);
			return False;
		}
		pCleanPara->HeadBox_Z_FlashPos = pCleanPara->factory.HeadBox_Z_SuckPos; //with ver 0x102
		pCleanPara->Idle_FlashFreqInterval = 1000;//with ver 0x103
		pCleanPara->IdleClean_intervalTime = 0;//with ver 0x104
		//add new elememt initialization here.
		ret = SaveCleanParamEPR_EPSON_Allwin(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonAllwin));
		OSSemPost(CleaningParamSem);
		return ret;
	}
	else if(head.flag == EPR_CLEANPARA_HEAD_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_CLEANPARAM_DATA_OFFSET + sizeof(EPM_Head), 
								  (INT8U * )pCleanPara, sizeof(struct EPR_CleanParaEpsonAllwin), &head))
		{
			OSSemPost(CleaningParamSem);
			return False;
		}
		
		if( pCleanPara->flag != EPR_CLEANPARA_FLAG || 
		   pCleanPara->structsize != sizeof(struct EPR_CleanParaEpsonAllwin))
		{
			if(pCleanPara->flag == EPR_CLEANPARA_FLAG_3B5A)
			{
				pCleanPara->HeadBox_Z_FlashPos = pCleanPara->factory.HeadBox_Z_SuckPos;
				pCleanPara->Idle_FlashFreqInterval = 1000;//with ver 0x103
				pCleanPara->IdleClean_intervalTime = 0;//with ver 0x104
			}
			if(head.version == EPR_CLEANPARA_VER_102)
			{
				pCleanPara->Idle_FlashFreqInterval = 1000;//with ver 0x103
				pCleanPara->IdleClean_intervalTime = 0;//with ver 0x104
			}
			if(head.version == EPR_CLEANPARA_VER_103)
			{
				pCleanPara->IdleClean_intervalTime = 0;//with ver 0x104
			}			
			pCleanPara->flag = EPR_CLEANPARA_FLAG;
			pCleanPara->structsize = sizeof(struct EPR_CleanParaEpsonAllwin);
			
			ret = SaveCleanParamEPR_EPSON_Allwin(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonAllwin) );
			OSSemPost(CleaningParamSem);
			return ret;
		}
		OSSemPost(CleaningParamSem);
		return True;
	}
	else
	{
		OSSemPost(CleaningParamSem);
		InitialCleanParam_EPSON_Allwin();
		
		return SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 0, sizeof(struct EPR_CleanParaEpsonAllwin));
	}
}

INT8U SaveCleanParamEPR_EPSON_Allwin(struct EPR_CleanParaEpsonAllwin *pCleanPara, INT16S start, INT16U len)
{
	INT16U i;
	INT8U err;
	INT16U length;
	INT8U ret = True;
	
	EPM_Head head = 
	{
		EPR_CLEANPARA_HEAD_FLAG,
		EPR_CLEANPARA_VER,
		sizeof(struct EPR_CleanParaEpsonAllwin) + sizeof(EPM_Head),
		0
	};
	
	if(start <0 || start >= sizeof(struct EPR_CleanParaEpsonAllwin) || len <=0 || len > sizeof(struct EPR_CleanParaEpsonAllwin))
	{
		start = 0;
		len = sizeof(struct EPR_CleanParaEpsonAllwin);
	}
	
	CalcCheckSum((INT8U *)pCleanPara, &head);
	ret = IIC_WriteEEPRom_Head(EPR_CLEANPARAM_DATA_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_CLEANPARAM_DATA_OFFSET + sizeof(EPM_Head) + start, 
							  ((INT8U *)pCleanPara)+ start, &len);
	return ret;
}

INT8U UI_Reset_CleanConfig(void * pData)
{
	InitialCleanParam_EPSON_Allwin();
	return SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 0, sizeof(struct EPR_CleanParaEpsonAllwin));
}

INT8U DoKeyClean_EPSON_ALLWIN(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			cleaning = True;
	}
	if(cleaning)
	{
#ifdef FUNCTION_CLOSE_LCD
		if(Index>=0 && Index <4)
			ActiveCleanConfig = Index;
		else
#endif
			ActiveCleanConfig = 0xFF;
		nextband_autoClean = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}

//UIActionFuncType
INT8U DoClean_EPSON_ALLWIN(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			cleaning = True;
	}
	if(cleaning)
	{
#ifdef DYSS_UV_CLEAN
		DYSS_CLEAN_FLASH = 2;
#endif
		if(Index>=0 && Index <5)
			ActiveCleanConfig = Index;
		else
			ActiveCleanConfig = 0xFF;
		nextband_autoClean = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}
INT8U  DoFlash_EPSON_ALLWIN(void * pData, int Index)
{
	U8 err;
	U8 buf[10] = {0};
	INT8U Flash = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		Flash = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			Flash = True;
	}
#ifdef DYSS_UV_CLEAN
	if(Flash)
	{
		DYSS_CLEAN_FLASH = 1;
		nextband_autoClean = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
	}
	return True;
#else			
	if(Flash)
	{
		status_ReportStatus(STATUS_CLEANING, STATUS_SET);
		
#if defined(HEAD_RICOH_G4)
		if(printerFlashing)
			FPGA_STOP_FLASH_Safe();
		
		OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err);
		if(err == OS_NO_ERR && !printerFlashing)
#else
			if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
#endif
			{
				buf[0] = 4;
				buf[1] = UART_HEAD_EPSON_JOBSTART;
				buf[2] = 1; //flash Job
				buf[3] = 2;
				while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
					OSTimeDly(10);
				OSTimeDly(100);
				
#ifdef HEAD_EPSON_GEN5	
				buf[0] = 6;
				buf[1] = UART_HEAD_EPSON_PASSSTART;
				buf[2] = 0;
				buf[3] = 0;
				buf[4] = 0;
				buf[5] = 0;
				UART_SendCMD(UART_HEAD_CHANNEL, buf);
				OSTimeDly(100);
#endif
			}
		
#ifdef HEAD_EPSON_GEN5	
#ifdef EPSON_4H
		FPGA_START_FLASH_ALLWIN(1000, 15);
#else
		FPGA_START_FLASH_ALLWIN(1000, 3);
#endif
		OSTimeDly(30);
		FPGA_STOP_FLASH_Safe();
#else

#ifdef CONVERSION_BOARD
		FPGA_START_FLASH_ALLWIN(1000, 15);
		OSTimeDly(30);
		FPGA_STOP_FLASH();
#else
		FPGA_START_RICOH_AUTO_C();
		OSTimeDly(10);
#endif

#endif
		
#if defined(HEAD_RICOH_G4)
		OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err);
		if(err == OS_NO_ERR && !printerFlashing)
#else
			if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
#endif
			{
				buf[0] = 2;
				buf[1] = UART_HEAD_EPSON_JOBEND;
				while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
					OSTimeDly(10);									
				OSTimeDly(100);
			}
		
#if defined(HEAD_RICOH_G4)
		if(printerFlashing)
			FPGA_START_FLASH_RICOH(0xFF);
#endif
		status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
		return True;
	}
	else
		return False;
#endif		
}

volatile INT8U nextband_Clean_InkBox = False;

INT8U  DoClean_InkBox_EPSON_ALLWIN(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			cleaning = True;
	}
	if(cleaning)
	{
		nextband_autoClean = False;
		nextband_Clean_InkBox = True;
		ActiveCleanConfig = 3;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}
INT8U SetCleanParam_EPSON_ALLWIN_HeadMask(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
	if(Data == 0)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 0xF;
	else if(Data == 1)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
	else if(Data == 2)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 2;
	else if(Data == 3)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 4;
	else if(Data == 4)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 8;
	else
		ret = False;
#elif defined (DYSS_UV_CLEAN)	
	if(Data == 0)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
	else if(Data == 1)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 2;
	else if(Data == 2)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 4;
	else if(Data == 3)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 7;

	else
		ret = False;
#else    
	if(Data == 0)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
	else if(Data == 1)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 2;
	else if(Data == 2)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 3;
	else
		ret = False;
#endif    
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Runtime.HeadMask - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Runtime.HeadMask));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_Shortcuts(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//Index = Data;
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<4)
		cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig = Index;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_SuckTimes(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data < 0)
		Data = 0;
	else if(Data >= 0x80)
		Data = 0x7F;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].SuckTimes = Data;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].SuckTimes - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].SuckTimes));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_SuckInkTime(void * pData, int Index, float Data) //unit is second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].SuckInkTime = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].SuckInkTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].SuckInkTime));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_InputAirTime(void * pData, int Index, float Data) //unit is second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5 && Data >= 0.0)
		cleanparam_EPSON_ALLWIN.Config[Index].InputAirTime = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].InputAirTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].InputAirTime));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_SuckWasteInkTime(void * pData, int Index, float Data) //unit is second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].SuckWasteInkTime = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].SuckWasteInkTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].SuckWasteInkTime));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_WipeTimes(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0)
		Data = 1;
	else if(Data >= 0xFF) 
		Data = 0xFF;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].WipeTimes = Data;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].WipeTimes - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].WipeTimes));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_Carriage_X_Wipe_Speed(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0)
		Data = 1;
	else if(Data >= 7) 
		Data = 7;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].Carriage_X_Wipe_Speed = Data;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].Carriage_X_Wipe_Speed - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].Carriage_X_Wipe_Speed));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_FlashFreq(void * pData, int Index, float Data) //unit: KHz.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 1.0)
		Data = 1.0;
	else if(Data >= 10.0)
		Data = 10.0;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].FlashFreqInterval = (INT16U)(1000/Data+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].FlashFreqInterval - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].FlashFreqInterval));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_FlashTime(void * pData, int Index, float Data) //unit: second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].FlashTime = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].FlashTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].FlashTime));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_FlashCycle(void * pData, int Index, float Data) //unit: second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].FlashCycle = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].FlashCycle - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].FlashCycle));
	return ret;
}

INT8U SetCleanParam_EPSON_ALLWIN_FlashIdleInCycle(void * pData, int Index, float Data) //unit: second.
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_ALLWIN.Config[Index].FlashIdleInCycle = (INT16U)(Data*10+0.5);
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.Config[Index].FlashIdleInCycle - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.Config[0].FlashIdleInCycle));
	return ret;
}

//UIGetIntFuncType
INT8U GetCleanParam_EPSON_ALLWIN_HeadMask(void * pData, int Index) 
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
	if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 0xF)
		ret = 0;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 1)
		ret = 1;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 2)
		ret = 2;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 4)
		ret = 3;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 8)
		ret = 4;
	else
		ret = -1;
#elif defined (DYSS_UV_CLEAN)
	if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 1)
		ret = 0;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 2)
		ret = 1;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 4)
		ret = 2;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 7)
		ret = 3;		

	else
		ret = -1;
#else
	if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 1)
		ret = 0;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 2)
		ret = 1;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 3)
		ret = 2;
	else
		ret = -1;
#endif    
	OSSemPost(CleaningParamSem);
	return ret;
}

INT8U GetCleanParam_EPSON_ALLWIN_Shortcuts(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_ALLWIN_SuckTimes(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	if(Index>=0 && Index<5)
		Index = 0;
	ret = cleanparam_EPSON_ALLWIN.Config[Index].SuckTimes;
	
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_SuckInkTime(void * pData, int Index) //unit is second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = ((float)cleanparam_EPSON_ALLWIN.Config[Index].SuckInkTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_InputAirTime(void * pData, int Index) //unit is second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = ((float)cleanparam_EPSON_ALLWIN.Config[Index].InputAirTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_SuckWasteInkTime(void * pData, int Index) //unit is second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = ((float)cleanparam_EPSON_ALLWIN.Config[Index].SuckWasteInkTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_ALLWIN_WipeTimes(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = cleanparam_EPSON_ALLWIN.Config[Index].WipeTimes;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_ALLWIN_Carriage_X_Wipe_Speed(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = cleanparam_EPSON_ALLWIN.Config[Index].Carriage_X_Wipe_Speed;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_FlashFreq(void * pData, int Index) //unit: KHz.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = 1000/(float)cleanparam_EPSON_ALLWIN.Config[Index].FlashFreqInterval;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_FlashTime(void * pData, int Index) //unit: second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = (float)cleanparam_EPSON_ALLWIN.Config[Index].FlashTime/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_FlashCycle(void * pData, int Index) //unit: second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = (float)cleanparam_EPSON_ALLWIN.Config[Index].FlashCycle/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_ALLWIN_FlashIdleInCycle(void * pData, int Index) //unit: second.
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		Index = 0;
	ret = (float)cleanparam_EPSON_ALLWIN.Config[Index].FlashIdleInCycle/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

//auto clean setting. share the same data structure with PM.
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	OS_CPU_SR cpu_sr;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//  Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
#ifdef AUTO_CLEAN_BYTIME
		OS_ENTER_CRITICAL();
		Clean_Wait_Time = 0;
		OS_EXIT_CRITICAL();
#endif	
		cleanparam_EPSON_ALLWIN.bAutoClean = Index;
		if(cleanparam_EPSON_ALLWIN.bAutoClean)
			cleanPara.autoClean_passInterval = cleanparam_EPSON_ALLWIN.AutoClean_passInterval;
		else
			cleanPara.autoClean_passInterval = 0;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.bAutoClean - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.bAutoClean));
	return ret;
}
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_Pass(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	OS_CPU_SR cpu_sr;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Data > 0 && Data <0x100)
	{
#ifdef AUTO_CLEAN_BYTIME
		OS_ENTER_CRITICAL();
		Clean_Wait_Time = 0;
		OS_EXIT_CRITICAL();
#endif	
		cleanparam_EPSON_ALLWIN.AutoClean_passInterval = Data;
		if(cleanparam_EPSON_ALLWIN.bAutoClean)
		{
			cleanPara.autoClean_passInterval = cleanparam_EPSON_ALLWIN.AutoClean_passInterval;
			cleanParaDirty_ToPM = True;
		}
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.AutoClean_passInterval - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.AutoClean_passInterval));
	return ret;
}
//index: 0~3: custom/strong/normal/weak.
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_Way(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	OS_CPU_SR cpu_sr;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<4)
	{
		cleanparam_EPSON_ALLWIN.AutoCleanConfig = Index;
		cleanPara.autoClean_way = cleanparam_EPSON_ALLWIN.AutoCleanConfig + 1;
		cleanParaDirty_ToPM = True;
#ifdef AUTO_CLEAN_BYTIME
		OS_ENTER_CRITICAL();
		Clean_Wait_Time = 0;
		OS_EXIT_CRITICAL();
#endif	
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.AutoCleanConfig - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.AutoCleanConfig));
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.bAutoClean;
	OSSemPost(CleaningParamSem);
	return ret;
}
INT32S GetCleanParam_EPSON_ALLWIN_AutoClean_Pass(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.AutoClean_passInterval;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_ALLWIN_AutoClean_Time(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.Config[0].FlashTime;
	OSSemPost(CleaningParamSem);
	return ret;
}

//return value: 0~3: custom/strong/normal/weak.
INT8U GetCleanParam_EPSON_ALLWIN_AutoClean_Way(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.AutoCleanConfig;
	OSSemPost(CleaningParamSem);
	return ret;
}

#ifdef MANUFACTURER_DGEN
//空闲清洗
INT8U GetCleanParam_IdleClean_OnOff(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = (cleanparam_EPSON_ALLWIN.IdleClean_intervalTime != 0);
	OSSemPost(CleaningParamSem);
	
	return ret;
}

INT8U SetCleanParam_IdleClean_OnOff(void * pData, int Index, int Data)
{
	INT8U err;
	INT32S ret;

	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	//  Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		IdleClean_Time = 0;		//空闲清洗时间清0
		if(Index)
			cleanparam_EPSON_ALLWIN.IdleClean_intervalTime = 1;
		else
			cleanparam_EPSON_ALLWIN.IdleClean_intervalTime = 0;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.IdleClean_intervalTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.IdleClean_intervalTime));
	return ret;
}

INT32S GetCleanParam_IdleClean_Time(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_ALLWIN.IdleClean_intervalTime;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT8U SetCleanParam_IdleClean_Time(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);

	if(Data < 0)
		Data = 0;
	if(Data > 12)
		Data = 12;
	if(Data >= 0 && Data <=12)
	{
		IdleClean_Time = 0;		//空闲清洗时间清0
		cleanparam_EPSON_ALLWIN.IdleClean_intervalTime = Data;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
									   (INT8U*)&cleanparam_EPSON_ALLWIN.IdleClean_intervalTime - (INT8U*)&cleanparam_EPSON_ALLWIN, 
									   sizeof(cleanparam_EPSON_ALLWIN.IdleClean_intervalTime));
	return ret;
}
#endif

void StartCapping()
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
	
	move_dir = 4;
	move_distance = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;//move_distance means target position
	move_type = 3;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
}

void NotifyCappingEnd()
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_CLR, &err); 
}

void Capping()
{
	INT8U err;
	
	StartCapping();
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
	//NotifyCappingEnd();
}

void StartUnCapping()
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
	
	move_dir = 4;
#if defined (RIPSTAR_FLAT_EX)&&(defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H))
	move_distance = LCDMenuConfig_EX.OriginZ*Z_BASE_RES + Z_PLAT_LENGTH;//move_distance means target position
#elif defined(FULGENCY_FUN)	
	move_distance = Z_SAFE_POSITION;//move_distance means target position
#else
	move_distance = 0;//move_distance means target position
#endif	
	move_type = 3;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
	
#ifdef EPSON_FLASH_IDLE
	cleanPara.flash = False;
#endif
	
}

void UnCapping()
{
	INT8U err;
	StartUnCapping();

	OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
	//NotifyCappingEnd();
}

//only support X movement
static INT8U postMove_dir = 0;
static INT32S postMove_distance = 0;
INT8U postCapping = False;
INT8U RegPostMoveOfUncapping(INT8U dir, INT32S distance)
{
	if ( dir == 1 || dir == 2 || (dir & 0x30 != 0)  ||dir == 3) //only support X movement
	{
		postMove_dir = dir;
		postMove_distance = distance;
		return True;
	}
	else
		return False;
}

INT8U ActivePostXMove()
{
	INT8U err;
	
	NotifyCappingEnd();
	
	if(postMove_dir != 0)
	{
		if((postMove_dir & 0x10) != 0)
		{
			move_dir = postMove_dir & (~0x10);
			move_type = 1;
		}
		else if((postMove_dir & 0x20) != 0)
		{
			move_dir = postMove_dir & (~0x20);
			move_distance = postMove_distance;
			move_type = 3;
		}
		else if(postMove_dir == 3)
		{
			move_type = 4;
		}
		else
		{
			move_dir = postMove_dir;
			move_distance = postMove_distance;
			move_type = 0;
		}
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		postMove_dir = 0;
		return True;
	}
	else
		return False;
}

void RegPostCapping()
{
	postCapping = True;
}

INT8U ActivePostCapping()
{
	INT8U err;
	if(postCapping)
	{
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
		
		move_dir = 4;
		move_distance = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;//move_distance means target position
		move_type = 3;
		
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		
		postCapping = False;
		return True;
	}
#ifdef EPSON_FLASH_IDLE	
	else if(postCapping_flash)
	{
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
		
		move_dir = 4;
		move_distance = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;//move_distance means target position
		move_type = 3;
		
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		
		postCapping_flash = False;
		return True;
	}
#endif
	else
		return False;
}

INT8U CapPostHandleAfterMoveCompleted()
{
	OS_CPU_SR     cpu_sr = 0;
	INT8U err;
	
	OS_ENTER_CRITICAL();
	if (OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		if(ActivePostXMove())
		{
			OS_EXIT_CRITICAL();
			return True;
		}
		else if(ActivePostCapping())
		{
			OS_EXIT_CRITICAL();
			return True;
		}
		else
		{
			NotifyCappingEnd();
			OS_EXIT_CRITICAL();
			return False;
		}
	}
	else
	{
		NotifyCappingEnd();
		postMove_dir = 0;
		postCapping = False;
#ifdef EPSON_FLASH_IDLE			
		postCapping_flash = False;
#endif
		OS_EXIT_CRITICAL();
		return False;
	}
}
#endif

#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)

void StartCapping()
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
	
	move_dir = 7;
    	move_distance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance + cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance;
	move_type = 0;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
}

void NotifyCappingEnd()
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_CLR, &err); 
}

void Capping()
{
	INT8U err;
	
	StartCapping();
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
	//NotifyCappingEnd();
}

void StartUnCapping(INT8U bmove_C_Axis)
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
	
	move_dir = 8;
	if (bmove_C_Axis)
	{
		move_distance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance;//move_distance means target position
	}
	else
	{
		move_distance = 0xFFFFFFF;
	}
	move_type = 0;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);
	
#ifdef EPSON_FLASH_IDLE
	cleanPara.flash = False;
#endif
	
}

void UnCapping()
{
	INT8U err;
	StartUnCapping(True);

	OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
	//NotifyCappingEnd();
}

//only support X movement
static INT8U postMove_dir = 0;
static INT32S postMove_distance = 0;
INT8U postCapping = False;
INT8U RegPostMoveOfUncapping(INT8U dir, INT32S distance)
{
	if ( dir == 1 || dir == 2 || (dir & 0x30 != 0)  ||dir == 3) //only support X movement
	{
		postMove_dir = dir;
		postMove_distance = distance;
		return True;
	}
	else
		return False;
}

INT8U ActivePostXMove()
{
	INT8U err;
	
	NotifyCappingEnd();
	
	if(postMove_dir != 0)
	{
		if((postMove_dir & 0x10) != 0)
		{
			move_dir = postMove_dir & (~0x10);
			move_type = 1;
		}
		else if((postMove_dir & 0x20) != 0)
		{
			move_dir = postMove_dir & (~0x20);
			move_distance = postMove_distance;
			move_type = 3;
		}
		else if(postMove_dir == 3)
		{
			move_type = 4;
		}
		else
		{
			move_dir = postMove_dir;
			move_distance = postMove_distance;
			move_type = 0;
		}
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		postMove_dir = 0;
		return True;
	}
	else
		return False;
}

void RegPostCapping()
{
	postCapping = True;
}

INT8U ActivePostCapping()
{
	INT8U err;
	if(postCapping)
	{
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
		
		move_dir = 7;
		//move_distance = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;//move_distance means target position
	       move_distance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance + cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance;
		move_type = 0;
		
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		
		postCapping = False;
		return True;
	}
#ifdef EPSON_FLASH_IDLE	
	else if(postCapping_flash)
	{
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
		
		move_dir = 7;
		//move_distance = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;//move_distance means target position
        	move_distance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance;
		move_type = 0;
		
		//status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		OSFlagPost(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_SET,&err); 
		
		postCapping_flash = False;
		return True;
	}
#endif
	else
		return False;
}

INT8U CapPostHandleAfterMoveCompleted()
{
	OS_CPU_SR     cpu_sr = 0;
	INT8U err;
	
	OS_ENTER_CRITICAL();
	if (OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		if(ActivePostXMove())
		{
			OS_EXIT_CRITICAL();
			return True;
		}
		else if(ActivePostCapping())
		{
			OS_EXIT_CRITICAL();
			return True;
		}
		else
		{
			NotifyCappingEnd();
			OS_EXIT_CRITICAL();
			return False;
		}
	}
	else
	{
		NotifyCappingEnd();
		postMove_dir = 0;
		postCapping = False;
#ifdef EPSON_FLASH_IDLE			
		postCapping_flash = False;
#endif
		OS_EXIT_CRITICAL();
		return False;
	}
}

#endif

#if (defined (EPSON_CLEAN_INTEGRATE)|| defined (EPSON_CLEAN_INTEGRATE_1)||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3))
struct EPR_CleanParaEpsonMicolor cleanparam_EPSON_MICOLOR;
volatile INT8U ActiveCleanConfig = 0xFF;
//auto clean setting. share the same data structure with PM.
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_OnOff(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	OS_CPU_SR cpu_sr;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//  Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		cleanparam_EPSON_MICOLOR.bAutoClean = Index;
#ifdef AUTO_CLEAN_BYTIME
		OS_ENTER_CRITICAL();
		Clean_Wait_Time = 0;
		OS_EXIT_CRITICAL();
#endif		
		if(cleanparam_EPSON_MICOLOR.bAutoClean)
			cleanPara.autoClean_passInterval = cleanparam_EPSON_MICOLOR.AutoClean_passInterval;
		else
			cleanPara.autoClean_passInterval = 0;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.bAutoClean - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.bAutoClean));
	return ret;
}
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_Pass(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	OS_CPU_SR cpu_sr;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
#ifdef AUTO_CLEAN_BYTIME
	if(Data > 0 && Data <999)
	{
		cleanparam_EPSON_MICOLOR.AutoClean_timeInterval = Data;
		OS_ENTER_CRITICAL();
		Clean_Wait_Time = 0;
		OS_EXIT_CRITICAL();	
	}
#else
	if(Data > 0 && Data <0x100)
	{
		cleanparam_EPSON_MICOLOR.AutoClean_passInterval = Data;
		if(cleanparam_EPSON_MICOLOR.bAutoClean)
		{
			cleanPara.autoClean_passInterval = cleanparam_EPSON_MICOLOR.AutoClean_passInterval;
			cleanParaDirty_ToPM = True;
		}
	}
#endif
	else
		ret = False;
	OSSemPost(CleaningParamSem);
#ifdef AUTO_CLEAN_BYTIME    
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.AutoClean_timeInterval - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.AutoClean_timeInterval));
#else
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.AutoClean_passInterval - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.AutoClean_passInterval));
#endif
	return ret;
}
//index: 0~3: custom/strong/normal/weak.
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_Way(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<4)
	{
		cleanparam_EPSON_MICOLOR.AutoCleanConfig = Index;
		cleanPara.autoClean_way = cleanparam_EPSON_MICOLOR.AutoCleanConfig + 1;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.AutoCleanConfig - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.AutoCleanConfig));
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_MICOLOR_AutoClean_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.bAutoClean;
	OSSemPost(CleaningParamSem);
	return ret;
}
INT32S GetCleanParam_EPSON_MICOLOR_AutoClean_Pass(void * pData, int Index)
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
#ifdef AUTO_CLEAN_BYTIME
	ret = cleanparam_EPSON_MICOLOR.AutoClean_timeInterval;
#else
	ret = cleanparam_EPSON_MICOLOR.AutoClean_passInterval;
#endif   
	OSSemPost(CleaningParamSem);
	return ret;
}

//return value: 0~3: custom/strong/normal/weak.
INT8U GetCleanParam_EPSON_MICOLOR_AutoClean_Way(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.AutoCleanConfig;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT8U GetCleanParam_EPSON_MICOLOR_HeadMask(void * pData, int Index) 
{
	INT8U err;
	INT32S ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 1)
		ret = 0;
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 2)
		ret = 1;
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 3)
		ret = 2;
	else
		ret = -1;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT8U SetCleanParam_EPSON_MICOLOR_HeadMask(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Data == 0)
		cleanparam_EPSON_MICOLOR.Runtime.HeadMask = 1;
	else if(Data == 1)
		cleanparam_EPSON_MICOLOR.Runtime.HeadMask = 2;
	else if(Data == 2)
		cleanparam_EPSON_MICOLOR.Runtime.HeadMask = 3;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.Runtime.HeadMask - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.Runtime.HeadMask));
	return ret;
}

void SetCleanParam_EPSON_Micolor(CleanParaType_EPSON_MICOLOR_USB * pUsb,INT8U index)
{
	INT8U err,i;
	struct CleanParaEpsonMicolor_Config * pActiveConfig;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	for(i =0; i< 4; i++)
	{
		cleanparam_EPSON_MICOLOR.factory.Speed_Interval[i] = pUsb->Speed_Interval[i];
	}
	cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir = pUsb->RotateDir;
	
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance = pUsb->Wiper_Y_HideToWipeDistance;
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance = pUsb->Wiper_Y_WipeToSuckDistance;
#if defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance = pUsb->Wiper_Y_SuckToHideDistance;
#else	
	if(GetCleanPartCount() == 1)
		cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance = pUsb->Wiper_Y_SuckToHideDistance;
	else
		cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance = pUsb->Wiper_Y_HideToWipeDistance + pUsb->Wiper_Y_WipeToSuckDistance + 100;
#endif    
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SuckPos = pUsb->Carriage_X_SuckPos;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos = pUsb->Carriage_X_ReleasePos;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start = pUsb->Carriage_X_WipePos_Start;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End = pUsb->Carriage_X_WipePos_End;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = pUsb->Carriage_X_FlashPos;
	if(GetCleanPartCount() == 1)
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = pUsb->Carriage_X_FlashPos;
	else //temp way.
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = pUsb->Wiper_Y_SuckToHideDistance;
	
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start = pUsb->Carriage_X_FlashPos - (216*3+64*4)/2; //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End = pUsb->Carriage_X_FlashPos + (216*3+64*4)/2; //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_Start = cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 - (216*3+64*4)/2; //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_End = cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 + (216*3+64*4)/2; //printhead width
	cleanparam_EPSON_MICOLOR.Wiper_Y_HideToWipeDistance_1 = pUsb->Wiper_Y_HideToWipeDistance_1;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start = pUsb->Carriage_X_WipePos_1_Start;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End=pUsb->Carriage_X_WipePos_1_End;
	if(index >= MAX_CLEAN_CONFIG)
		pActiveConfig = cleanparam_EPSON_MICOLOR.Config;
	else
		pActiveConfig = &cleanparam_EPSON_MICOLOR.Config[index];
	
	pActiveConfig->Carriage_X_Wipe_Speed = pUsb->Carriage_X_Wipe_Speed;
	pActiveConfig->sectionCount = pUsb->sectionCount;
	memcpy(pActiveConfig->sections, pUsb->sections, sizeof(pActiveConfig->sections));
	
	OSSemPost(CleaningParamSem);
}

void OutCleanParam_EPSON_Micolor(CleanParaType_EPSON_MICOLOR_USB * pUsb,INT8U index)
{
	INT8U err,i;
	struct CleanParaEpsonMicolor_Config * pActiveConfig;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	pUsb->RotateDir = cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir;
	for(i =0; i< 4; i++)
	{
		pUsb->Speed_Interval[i] = cleanparam_EPSON_MICOLOR.factory.Speed_Interval[i];
	}
	
	pUsb->Wiper_Y_HideToWipeDistance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance;
	pUsb->Wiper_Y_WipeToSuckDistance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance;
#if defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
	pUsb->Wiper_Y_SuckToHideDistance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance;
#else	
	if(GetCleanPartCount() == 1)
		pUsb->Wiper_Y_SuckToHideDistance = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance;
	else
		pUsb->Wiper_Y_SuckToHideDistance = cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1;
#endif	
	
	pUsb->Carriage_X_SuckPos = cleanparam_EPSON_MICOLOR.factory.Carriage_X_SuckPos;
	pUsb->Carriage_X_ReleasePos = cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos;
	pUsb->Carriage_X_WipePos_Start = cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start;
	pUsb->Carriage_X_WipePos_End = cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End;
	pUsb->Carriage_X_FlashPos = cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos;
	
	if(index >= MAX_CLEAN_CONFIG)
		pActiveConfig = cleanparam_EPSON_MICOLOR.Config;
	else
		pActiveConfig = &cleanparam_EPSON_MICOLOR.Config[index];
	
	pUsb->Carriage_X_Wipe_Speed = pActiveConfig->Carriage_X_Wipe_Speed;
	pUsb->sectionCount = pActiveConfig->sectionCount;
	pUsb->Wiper_Y_HideToWipeDistance_1 = cleanparam_EPSON_MICOLOR.Wiper_Y_HideToWipeDistance_1;
	pUsb->Carriage_X_WipePos_1_Start =  cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start;
	pUsb->Carriage_X_WipePos_1_End =  cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End;
	memcpy(pUsb->sections, pActiveConfig->sections, sizeof(pUsb->sections));
	
	OSSemPost(CleaningParamSem);
}

void SetAutoClean_EPSON_Micolor(CleanParaType * pUsb)
{
	INT8U err;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	cleanparam_EPSON_MICOLOR.bAutoSpray = pUsb->longflash_passInterval != 0;
	
	OSSemPost(CleaningParamSem);
	
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.bAutoSpray - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.bAutoSpray));
}

void OutAutoClean_EPSON_Micolor(CleanParaType * pUsb)
{
	INT8U err;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	if(cleanparam_EPSON_MICOLOR.bAutoSpray)
		pUsb->longflash_passInterval = 1;
	else
		pUsb->longflash_passInterval = 0;
	
	OSSemPost(CleaningParamSem);
}

INT16U GetMaxSprayLength(INT32U MaxBuffLen, INT16U MaxPosiLen)
{
	INT32U len = RESERVED_MEMSIZE / epsonCaliSetting.printerSetting.BytesPerFire;
	len = len * 720 / (720+360+540+270+1440) / 2; // all resolution.
	len = len * 95 / 100; //only for safe.
	if(len > MaxPosiLen)
		len = MaxPosiLen;
	return (INT16U)len;
}

void InitialCleanParam_EPSON_Micolor()
{
	INT8U i,j;
	INT8U err;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.flag = EPR_CLEANPARA_FLAG;
	cleanparam_EPSON_MICOLOR.structsize = sizeof(struct EPR_CleanParaEpsonMicolor);
	
#ifdef SUPPORT_MOTOR_CONTROL
	//    cleanparam_EPSON_MICOLOR.factory.Speed_Interval[0] = 56;
	//    cleanparam_EPSON_MICOLOR.factory.Speed_Interval[1] = 91;
	//    cleanparam_EPSON_MICOLOR.factory.Speed_Interval[2] = 143;
	//    cleanparam_EPSON_MICOLOR.factory.Speed_Interval[3] = 250;
	
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[0] = 83;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[1] = 125;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[2] = 167;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[3] = 250;
	
	cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir = True;
#if defined(COLORFUL_EPSON)
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance = 50;
#else
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance = 500;
#endif
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance = 100; 
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance = 700;
	//        cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance + cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SuckPos = 0;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos = 500;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start = 300;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End = 3000;
#if defined(W1615EX) ||defined(W1815EX)
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 7700;
#elif defined(S1828EX) ||defined(S2628EX)||defined(T1828EX)
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 4450;
#elif defined(R600)
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 2800;
#elif defined(TATE_BEMAJET)
#ifdef BEMAJET_2H
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 3600;
#else
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 4118;
#endif
#elif defined(COLORFUL_EPSON)
    cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 501;
#else
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 4860;
#endif
	
#if defined(TATE_BEMAJET)
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 4600;
#elif defined(S1828EX) ||defined(S2628EX)||defined(T1828EX)
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 6300;
#elif defined(COLORFUL_EPSON)
    cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 500;
#else
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 6760;
#endif
	
#else
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[0] = 25;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[1] = 50;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[2] = 125;
	cleanparam_EPSON_MICOLOR.factory.Speed_Interval[3] = 250;
	
	cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir = True;
#if defined(MANUFACTURER_INKWIN)		
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance = 900;
#else
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance = 800;
#endif
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance = 45; 
	cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance = 
		cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance + cleanparam_EPSON_MICOLOR.factory.Wiper_Y_WipeToSuckDistance;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SuckPos = 0;
#if defined(MANUFACTURER_INKWIN)	
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos = 800;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start = 2000;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End = 5000;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 5500;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 7500;
#else
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos = 360;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start = 400;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End = 1500;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos = 3000;
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 = 3000;
#endif	
#endif
	
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start = 
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos - GetMaxSprayLength(RESERVED_MEMSIZE, (216*3+64*4)); //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End = 
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos + GetMaxSprayLength(RESERVED_MEMSIZE, (216*3+64*4)); //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_Start = 
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 - GetMaxSprayLength(RESERVED_MEMSIZE, (216*3+64*4)); //printhead width
	cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_End = 
		cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos_1 + GetMaxSprayLength(RESERVED_MEMSIZE, (216*3+64*4)); //printhead width
	
	for(i=0; i<MAX_CLEAN_CONFIG; i++)
	{
		cleanparam_EPSON_MICOLOR.Config[i].Carriage_X_Wipe_Speed = 7;
		cleanparam_EPSON_MICOLOR.Config[i].sectionCount = 3;
		
		if(i== 3)
		{
			//section 1: fill.
			j=0;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = SuckOnly;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = 1;
#if defined(TATE_BEMAJET)						
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 3000;
#else
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 2400;
#endif
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 100;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 20;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = 2;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].MoveSpeed = 2;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].Option = 0;
		}
		else
		{
			//section 1: pre-clean.
			j=0;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = PreClean;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = 1;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 0;
#if defined(TATE_BEMAJET) 	
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 10;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 10;
#else
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 100;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 20;
#endif					
#if defined(R600) 
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 20; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 20; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 10; //unit:0.1s.
#elif defined(TATE_BEMAJET) 	 
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.
#else
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.
#endif
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = 2;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].MoveSpeed = 2;
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].Option = 0;
		}
		//section 2: strong suck.
		j++;
#if defined(TATE_BEMAJET)	||defined(MANUFACTURER_INKWIN)	 ||defined(TATE_BEMAJET)	//changed by daoxiutao					
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = SuckWipeAndFlash;
#else
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = SuckAndWipe;
#endif				
		
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = 1;
#if defined(MANUFACTURER_INKWIN)
		if(i==2 || i==3)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 50;
		else if(i==1)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 80;
		else if(i==0)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 100;
#else
		if(i==2 || i==3)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 30;
		else if(i==1)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 50;
		else if(i==0)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 80;
#endif				
#if defined(TATE_BEMAJET)
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 1;
		if(i==2 || i==3)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 60;
		else if(i==1)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 80;
		else if(i==0)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 100;
#else
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 100;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 30;
#endif
#if defined(R600) 
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 20; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 20; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 10; //unit:0.1s.
#elif defined(TATE_BEMAJET) 	 
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.        
#else
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.
#endif
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = 2;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].MoveSpeed = 2;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].Option = 0;
		
		//section 3: slow suck and flash.
		j++;
#if defined(TATE_BEMAJET)	||defined(MANUFACTURER_INKWIN)	 ||defined(TATE_BEMAJET)	//changed by daoxiutao								
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = None;
#else
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = SuckWipeAndFlash;
#endif	
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = 1;
		if(i==2 || i==3)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 50;
		else if(i==1)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 80;
		else if(i==0)
			cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 100;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 100;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 50;
#if defined(R600) 
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 20; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 20; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 10; //unit:0.1s.
#else
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 40; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 20; //unit:0.1s.
#endif
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = 3;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].MoveSpeed = 2;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].Option = 0;
		
		//section 4: reserved for the more complex cleaning ways.
		j++;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = None;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = 0;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = 0;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = 0;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = 0;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = 1000; //unit: us.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = 0; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = 0; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = 0; //unit:0.1s.
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = 3;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].MoveSpeed = 3;
		cleanparam_EPSON_MICOLOR.Config[i].sections[j].Option = 0;
	}
	
	cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig = 0;
	if(GetCleanPartCount() == 2)
		cleanparam_EPSON_MICOLOR.Runtime.HeadMask = 3;
	else
		cleanparam_EPSON_MICOLOR.Runtime.HeadMask = 1;
	
	cleanparam_EPSON_MICOLOR.bAutoSpray = True;
	//4mm width.
	cleanparam_EPSON_MICOLOR.SprayWidth = (INT16U)(4 * 720 /25.4); 
	//cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End - 
	//    cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start;
	cleanparam_EPSON_MICOLOR.SprayInterval = 200;
	cleanparam_EPSON_MICOLOR.passNum = 1;
	cleanparam_EPSON_MICOLOR.Clean_Fill_Safe = False;
#if defined(MANUFACTURER_INKWIN)	
	cleanparam_EPSON_MICOLOR.Wiper_Y_HideToWipeDistance_1 = 1200;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start = 5500;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End = 8000;
#else
	cleanparam_EPSON_MICOLOR.Wiper_Y_HideToWipeDistance_1 = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance+200;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start = cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_Start+200;
	cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End = cleanparam_EPSON_MICOLOR.factory.Carriage_X_WipePos_End+200;
#endif
	
	cleanparam_EPSON_MICOLOR.bAutoClean = 0;
	cleanparam_EPSON_MICOLOR.AutoClean_passInterval = 100;
	cleanPara.autoClean_passInterval = 100;
	cleanparam_EPSON_MICOLOR.AutoCleanConfig = 2;
	cleanparam_EPSON_MICOLOR.AutoClean_timeInterval = 10;
	
	OSSemPost(CleaningParamSem);
}

INT8U ReadCleanParamEPR_EPSON_Micolor(struct EPR_CleanParaEpsonMicolor *pCleanPara)
{
	INT8U err, ret;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(struct EPR_CleanParaEpsonMicolor) + sizeof(EPM_Head);
	if(length > EPR_CLEANPARAM_DATA_SIZE)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	
	if(!IIC_ReadEEPRom_Head(EPR_CLEANPARAM_DATA_OFFSET, &head))
	{
		OSSemPost(CleaningParamSem);
		return False;
	}
	
	memset(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor));
	if(head.flag == EPR_CLEANPARA_FLAG || head.flag == EPR_CLEANPARA_FLAG_VER_3A5C ) //it is old structure before add the head.
	{
		length = head.version; //really, it is element structsize of old structure.
		if(length > sizeof(struct EPR_CleanParaEpsonMicolor))
			length = sizeof(struct EPR_CleanParaEpsonMicolor);
		if(!IIC_ReadEEPRom_Ex(EPR_CLEANPARAM_DATA_OFFSET, (INT8U * )pCleanPara, &length))
		{
			OSSemPost(CleaningParamSem);
			return False;
		}
		//add new elememt initialization here.
		
		if(head.flag == EPR_CLEANPARA_FLAG_VER_3A5C)
		{
			pCleanPara->SprayInterval = 200;
			pCleanPara->passNum = 1;
		}
		if((head.flag == EPR_CLEANPARA_FLAG_VER_3A5C)||(head.flag == EPR_CLEANPARA_FLAG))
		{
			pCleanPara->Clean_Fill_Safe = False;
			pCleanPara->Wiper_Y_HideToWipeDistance_1 = pCleanPara->factory.Wiper_Y_HideToWipeDistance+200;
			pCleanPara->Carriage_X_WipePos_1_Start = pCleanPara->factory.Carriage_X_WipePos_Start+200;
			pCleanPara->Carriage_X_WipePos_1_End = pCleanPara->factory.Carriage_X_WipePos_End+200;
			pCleanPara->bAutoClean = 0;
			pCleanPara->AutoClean_passInterval = 0;
			pCleanPara->AutoCleanConfig = 2;
			pCleanPara->AutoClean_timeInterval = 10;
		}
		pCleanPara->flag = EPR_CLEANPARA_FLAG;
		pCleanPara->structsize = sizeof(struct EPR_CleanParaEpsonMicolor);
		
		ret = SaveCleanParamEPR_EPSON_Micolor(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor));
		OSSemPost(CleaningParamSem);
		return ret;
	}
	else if(head.flag == EPR_CLEANPARA_HEAD_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_CLEANPARAM_DATA_OFFSET + sizeof(EPM_Head), 
								  (INT8U * )pCleanPara, sizeof(struct EPR_CleanParaEpsonMicolor), &head))
		{
			OSSemPost(CleaningParamSem);
			return False;
		}
		if(head.version == EPR_CLEANPARA_VER_101)
		{
			pCleanPara->flag = EPR_CLEANPARA_FLAG;
			pCleanPara->structsize = sizeof(struct EPR_CleanParaEpsonMicolor);
			pCleanPara->Clean_Fill_Safe = False;
			pCleanPara->Wiper_Y_HideToWipeDistance_1 = pCleanPara->factory.Wiper_Y_HideToWipeDistance+200;
			pCleanPara->Carriage_X_WipePos_1_Start = pCleanPara->factory.Carriage_X_WipePos_Start+200;
			pCleanPara->Carriage_X_WipePos_1_End = pCleanPara->factory.Carriage_X_WipePos_End+200;
			pCleanPara->bAutoClean = 0;
			pCleanPara->AutoClean_passInterval = 0;
			pCleanPara->AutoCleanConfig = 2;
			pCleanPara->AutoClean_timeInterval = 10;
			ret = SaveCleanParamEPR_EPSON_Micolor(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor) );
			
			OSSemPost(CleaningParamSem);
			return ret;
		}
		if(head.version == EPR_CLEANPARA_VER_102)
		{
			pCleanPara->Wiper_Y_HideToWipeDistance_1 = pCleanPara->factory.Wiper_Y_HideToWipeDistance+200;
			pCleanPara->Carriage_X_WipePos_1_Start = pCleanPara->factory.Carriage_X_WipePos_Start+200;
			pCleanPara->Carriage_X_WipePos_1_End = pCleanPara->factory.Carriage_X_WipePos_End+200;
			pCleanPara->bAutoClean = 0;
			pCleanPara->AutoClean_passInterval = 0;
			pCleanPara->AutoCleanConfig = 2; 
			pCleanPara->AutoClean_timeInterval = 10;
			ret = SaveCleanParamEPR_EPSON_Micolor(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor) );
			OSSemPost(CleaningParamSem);
			return ret;
		}
		if(head.version == EPR_CLEANPARA_VER_103)
		{
			pCleanPara->bAutoClean = 0;
			pCleanPara->AutoClean_passInterval = 0;
			pCleanPara->AutoCleanConfig = 2;
			pCleanPara->AutoClean_timeInterval = 10;
			ret = SaveCleanParamEPR_EPSON_Micolor(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor) );
			OSSemPost(CleaningParamSem);
			return ret;
		}	
		if( pCleanPara->flag != EPR_CLEANPARA_FLAG || 
		   pCleanPara->structsize != sizeof(struct EPR_CleanParaEpsonMicolor))
		{
			pCleanPara->flag = EPR_CLEANPARA_FLAG;
			pCleanPara->structsize = sizeof(struct EPR_CleanParaEpsonMicolor);
			pCleanPara->Wiper_Y_HideToWipeDistance_1 = pCleanPara->factory.Wiper_Y_HideToWipeDistance+200;
			pCleanPara->Carriage_X_WipePos_1_Start = pCleanPara->factory.Carriage_X_WipePos_Start+200;
			pCleanPara->Carriage_X_WipePos_1_End = pCleanPara->factory.Carriage_X_WipePos_End+200;
			OSSemPost(CleaningParamSem);
			ret = SaveCleanParamEPR_EPSON_Micolor(pCleanPara, 0, sizeof(struct EPR_CleanParaEpsonMicolor) );
			
			OSSemPost(CleaningParamSem);
			return ret;
		}
		OSSemPost(CleaningParamSem);
		return True;
	}
	else
	{
		OSSemPost(CleaningParamSem);
		InitialCleanParam_EPSON_Micolor();
		
		return SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 0, sizeof(struct EPR_CleanParaEpsonMicolor));
	}
}

INT8U SaveCleanParamEPR_EPSON_Micolor(struct EPR_CleanParaEpsonMicolor *pCleanPara, INT16S start, INT16U len)
{
	INT16U i;
	INT8U err;
	INT16U length;
	INT8U ret = True;
	
	EPM_Head head = 
	{
		EPR_CLEANPARA_HEAD_FLAG,
		EPR_CLEANPARA_VER,
		sizeof(struct EPR_CleanParaEpsonMicolor) + sizeof(EPM_Head),
		0
	};
	
	if(start <0 || start >= sizeof(struct EPR_CleanParaEpsonMicolor) || len <=0 || len > sizeof(struct EPR_CleanParaEpsonMicolor))
	{
		start = 0;
		len = sizeof(struct EPR_CleanParaEpsonMicolor);
	}
	
	CalcCheckSum((INT8U*)pCleanPara, &head);
	ret = IIC_WriteEEPRom_Head(EPR_CLEANPARAM_DATA_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_CLEANPARAM_DATA_OFFSET + sizeof(EPM_Head) + start, 
							  ((INT8U *)pCleanPara)+ start, &len);
	return ret;
}

INT8U UI_Reset_CleanConfig(void * pData)
{
	INT8U err;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	InitialCleanParam_EPSON_Micolor();
	return SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 0, sizeof(struct EPR_CleanParaEpsonMicolor));
}

void QuickClean_EPSON_MICOLOR()
{
	ActiveCleanConfig = 0xFF;
	nextband_autoClean = True;
	status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
}

//for the clean key of panel or clean button of PM
INT8U DoKeyClean_EPSON_MICOLOR(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			cleaning = True;
	}
	if(cleaning)
	{
		ActiveCleanConfig = 0xFF;
		nextband_autoClean = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}
//index: 0~4: custom/strong/normal/weak/Refill.
INT8U DoClean_EPSON_MICOLOR(void * pData, int Index)
{
	U8 err;
	
	INT8U cleaning = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		cleaning = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			cleaning = True;
	}
	if(cleaning)
	{
		if(Index>=1 && Index <5)
			ActiveCleanConfig = Index - 1;
		else
			ActiveCleanConfig = 0xFF;
		nextband_autoClean = True;
		status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
		return True;
	}
	else
		return False;
}
INT8U DoFlash_EPSON_MICOLOR(void * pData, int Index)
{
	U8 err;
	U8 buf[10] = {0};
	INT8U Flash = False;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		Flash = True;
	}
	else if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CLEAN_BITS_FOR_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			Flash = True;
	}
	if(Flash)
	{
		//Need more change to support ricoh. see ALLWIN DoFlash function.
		buf[0] = 4;
		buf[1] = UART_HEAD_EPSON_JOBSTART;
		buf[2] = 1; //flash Job
		buf[3] = 2;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
			OSTimeDly(10);
		OSTimeDly(100);
		
		buf[0] = 6;
		buf[1] = UART_HEAD_EPSON_PASSSTART;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 0;
		UART_SendCMD(UART_HEAD_CHANNEL, buf);
		OSTimeDly(100);
		
#ifdef HEAD_EPSON_GEN5	

#ifdef EPSON_4H
		FPGA_START_FLASH_ALLWIN(1000, 15);
#else
		FPGA_START_FLASH_ALLWIN(1000, 3);
#endif

#else

#ifdef CONVERSION_BOARD
		FPGA_START_FLASH_ALLWIN(1000, 15);
#else
		FPGA_START_RICOH_AUTO_C();
#endif

#endif        
		OSTimeDly(30);
#ifdef HEAD_EPSON_GEN5	
		FPGA_STOP_FLASH_Safe();
#else

#ifdef CONVERSION_BOARD
		FPGA_STOP_FLASH();
#endif

#endif   
		buf[0] = 2;
		buf[1] = UART_HEAD_EPSON_JOBEND;
		while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
			OSTimeDly(10);		
		return True;
	}
	else
		return False;
}
INT8U GetClean_EPSON_MICOLOR(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = ActiveCleanConfig;
	OSSemPost(CleaningParamSem);
	return ret;
}

//index: 0~3: strong/normal/weak/Refill.
INT8U SetCleanParam_EPSON_MICOLOR_Shortcuts(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	Index = Data;
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<5)
		cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig = Index;
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig));
	return ret;
}
//Index: there are two part: the first is clean way, 0~4: custom/strong/normal/weak/Refill.
//  the second is, 0~3, clean step.
//  the final index = (clean way)*100+(clean step).
INT8U SetCleanParam_EPSON_MICOLOR_Type(void * pData, int Index, int Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data < 0 || Data > 5)
		return False;
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].type = (enum MicolorCleanType)Data;
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].type - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].type));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_LoopTimes(void * pData, int Index, int Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data < 0)
		Data = 0;
	else if(Data >= 0x100)
		Data = 0xFF;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes = Data;
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].LoopTimes));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_SuckInkTime(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].SuckInkTime));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_StayTime(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].StayTime));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_ReleaseTime(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0;
	else if(Data >= 36000) //1 hour
		Data = 36000;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].ReleaseTime));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_SuckSpeed(void * pData, int Index, int Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data < 0)
		Data = 0;
	else if(Data >= 4)
		Data = 3;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed = Data;
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].SuckSpeed));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_FlashFreq(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 1.0)
		Data = 1.0;
	else if(Data >= 10.0)
		Data = 10.0;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval = (INT16U)(1000/Data+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].FlashFreqInterval));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_FlashTime(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].FlashTime));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_FlashCycle(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].FlashCycle));
	return True;
}

INT8U SetCleanParam_EPSON_MICOLOR_FlashIdleInCycle(void * pData, int Index, float Data)
{
	INT8U err;
	INT8S i,j;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if(Data <= 0.0)
		Data = 0.1;
	else if(Data >= (float)255/10)
		Data = (float)255/10;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		return False;
	if(j<0 || j>3)
		return False;
	
	OSSemPend(CleaningParamSem, 0, &err);
	cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle = (INT16U)(Data*10+0.5);
	OSSemPost(CleaningParamSem);
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.Config[i].sections[0].FlashIdleInCycle));
	return True;
}

//return value: 0~4: custom/strong/normal/weak/Refill.
INT8U GetCleanParam_EPSON_MICOLOR_Shortcuts(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig;
	OSSemPost(CleaningParamSem);
	return ret;
}

//Index: there are two part: the first is clean way, 0~4: custom/strong/normal/weak/Refill.
//  the second is, 0~3, clean step.
//  the final index = (clean way)*100+(clean step).
INT32S GetCleanParam_EPSON_MICOLOR_Type(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	INT32S ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.Config[i].sections[j].type;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_MICOLOR_LoopTimes(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	INT32S ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.Config[i].sections[j].LoopTimes;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_SuckInkTime(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = ((float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckInkTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_StayTime(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = ((float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].StayTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_ReleaseTime(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = ((float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].ReleaseTime)/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

INT32S GetCleanParam_EPSON_MICOLOR_SuckSpeed(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	INT32S ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.Config[i].sections[j].SuckSpeed;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_FlashFreq(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = 1000/(float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashFreqInterval;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_FlashTime(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = (float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashTime/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_FlashCycle(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = (float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashCycle/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_FlashIdleInCycle(void * pData, int Index)
{
	INT8U err;
	INT8S i,j ;
	float ret;
	
	i = Index/100;
	j = Index%100;
	if(i<0 || i>4)
		i =0;
	if(j<0 || j>3)
		j =0 ;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = (float)cleanparam_EPSON_MICOLOR.Config[i].sections[j].FlashIdleInCycle/10;
	OSSemPost(CleaningParamSem);
	return ret;
}

//auto spray setting. share the same data structure with PM.
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff(void * pData, int Index)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		cleanparam_EPSON_MICOLOR.Clean_Fill_Safe = Index;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.Clean_Fill_Safe - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.Clean_Fill_Safe));
	return ret;
}
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff(void * pData, int Index)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		cleanparam_EPSON_MICOLOR.bAutoSpray = Index;
		if(cleanparam_EPSON_MICOLOR.bAutoSpray)
			cleanPara.longflash_passInterval = 1;
		else
			cleanPara.longflash_passInterval = 0;
		cleanParaDirty_ToPM = True;
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.bAutoSpray - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.bAutoSpray));
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.bAutoSpray;
	OSSemPost(CleaningParamSem);
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.Clean_Fill_Safe;
	OSSemPost(CleaningParamSem);
	return ret;
}
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_Pass(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Data>=1 && Data<=100)
		cleanparam_EPSON_MICOLOR.passNum = Data;
	else if(Data < 1)
		cleanparam_EPSON_MICOLOR.passNum = 1;
	else
		cleanparam_EPSON_MICOLOR.passNum = 100;
	OSSemPost(CleaningParamSem);
	
	SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
									(INT8U*)&cleanparam_EPSON_MICOLOR.passNum - (INT8U*)&cleanparam_EPSON_MICOLOR, 
									sizeof(cleanparam_EPSON_MICOLOR.passNum));
	return ret;
}

INT32S GetCleanParam_EPSON_MICOLOR_AutoSpray_Pass(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.passNum;
	OSSemPost(CleaningParamSem);
	return ret;
}
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_Width(void * pData, int Index, float Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	OSSemPend(CleaningParamSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		if((INT16U)(Data * 720) >= cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End - 
		   cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start)
		{
			cleanparam_EPSON_MICOLOR.SprayWidth = cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End - 
				cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start;
		}
		else
			cleanparam_EPSON_MICOLOR.SprayWidth = (INT16U)(Data * 720);
	}
	else
		ret = False;
	OSSemPost(CleaningParamSem);
	if(ret)
		SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
										(INT8U*)&cleanparam_EPSON_MICOLOR.SprayWidth - (INT8U*)&cleanparam_EPSON_MICOLOR, 
										sizeof(cleanparam_EPSON_MICOLOR.SprayWidth ));
	return ret;
}

float GetCleanParam_EPSON_MICOLOR_AutoSpray_Width(void * pData, int Index)
{
	INT8U err;
	float ret;
	
	OSSemPend(CleaningParamSem, 0, &err);
	ret = cleanparam_EPSON_MICOLOR.SprayWidth /720;
	OSSemPost(CleaningParamSem);
	return ret;
}

#ifdef SUPPORT_MOTOR_CONTROL
INT8U SpeedMapTable[4] = {3,2,1,0};
#else
INT8U SpeedMapTable[4] = {6,4,2,0};
#endif

void RotateCleanMotor(INT8U RotateDir, INT32U distance, INT8U speedIndx)
{
	INT8U err;
	INT8U buf[32];
	INT8U mask = cleanparam_EPSON_MICOLOR.Runtime.HeadMask & (( 1<< GetCleanPartCount()) -1);
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 8; //Length						
	buf[1] = UART_MOVEDISTANCE_CMD; 
#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
	if(mask == 1)
	{
#endif
		if(RotateDir)
			buf[2] = 7; //1,2 X, 3,4 Y, 5,6 Z
		else
			buf[2] = 8; //1,2 X, 3,4 Y, 5,6 Z
#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
	}
	else if(mask == 2)
	{
		if(RotateDir)
			buf[2] = 5; //1,2 X, 3,4 Y, 5,6 Z
		else
			buf[2] = 6; //1,2 X, 3,4 Y, 5,6 Z
	}
	else
	{
		buf[2] = 0;
		buf[2] = (1<<7)|(RotateDir<<6)|(3<<2);
	}
#endif
	buf[3] = SpeedMapTable[speedIndx]; 
	*((__packed INT32S *)&buf[4]) = distance; //move_distance means target position
	
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
}

void RotateCleanMotorByTime(INT8U RotateDir, INT32U Time, INT8U speedIndx)
{
#ifdef SUPPORT_MOTOR_CONTROL
	INT32U step = Time * 100 / cleanparam_EPSON_MICOLOR.factory.Speed_Interval[speedIndx];
#else
	INT32U step = Time * 10 / cleanparam_EPSON_MICOLOR.factory.Speed_Interval[speedIndx];
#endif    
	if(step==0)
		step = 1;
	RotateCleanMotor(RotateDir, step, speedIndx);
}
#define PRINT_STARTPOS_OFFSET (180 + 720) //180 is old offset  the new  add 720
void SlowLeaveXOrigin()
{
	INT8U buf[32];
	INT8U err;
	
	INT32S StartPos = (cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos + PRINT_STARTPOS_OFFSET);
	if(curPos.x <= StartPos)
	{
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = StartPos;
		buf[6] = printer.movSpeed;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		OSTimeDly(200);			
	}
}


#endif
#if defined(EPSON_CLEAN_INTEGRATE_1)||defined(RICOH_CLEAN_PRESS)
void wiper_down(void)
{
	INT8U buf[32];
	INT8U err;
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
#ifdef MANUFACTURER_ADDTOP_EPSON
	buf[0] = 8; //Length						
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance; 
#else
	buf[0] = 8; //Length						
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = 0; 
#endif    
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
	
}

void wiper_up(INT32U distance)
{
	INT8U buf[32];
	INT8U err;
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 8; //Length						
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = distance; 
	
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
}

#if defined(COLORFUL_EPSON)
void sucker_bytime(INT32U ms, INT8U CleanConfig)
{
	INT8U buf[32];
	INT8U err;
	INT32U lefttime;
		
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 8; //Length						
	buf[1] = UART_MOVEDISTANCE_CMD; 
	buf[2] = 5; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = 0x00FFFFFF; 

	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);	

	lefttime = ms /100;

	if (CleanConfig == 3)
	{
		while(1)
		{
			if (OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err), err == OS_NO_ERR)
			{
				lefttime = 0;
            			break;
			}
			OSTimeDly(10);
		}
	}
	else
	{
		while(lefttime > 0)
		{
			if(lefttime > 650)
				lefttime -= 650;
			else
				break;
					
			OSTimeDly(650*100);
		}
	}

	OSTimeDly(lefttime*100);
	
	buf[0] = 2;
	buf[1] = UART_STOPMOVE_CMD;
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
		OSTimeDly(10);	
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
}
#else
void sucker_bytime(INT32U ms)//极性反转
{
	CONTROL_OPEN_SUCKER(0);
	OSTimeDly(ms);
	CONTROL_CLOSE_SUCKER(0);
}
#endif
#elif defined(EPSON_CLEAN_INTEGRATE_2)
extern void wiper_down(INT8U num);
extern void wiper_up(INT8U num);
#define SUCK_ONCE_TIME 65000
void sucker_bytime(INT32U ms,INT16U wait_time,INT16U release_time,INT8U SUCK_INK)//极性反转
{
	INT8U flags = 0,err = 0;
#ifdef MANUFACTURER_TENGFENG_EPSON	
	if(SUCK_INK)
		CONTROL_CLOSE_RELEASE_RELAY(0);
#else
	CONTROL_CLOSE_RELEASE_RELAY(0);
#endif
	CONTROL_OPEN_SUCKER(0);
	while(ms > SUCK_ONCE_TIME)
	{
		if(SUCK_INK == 1)
		{
			flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,SUCK_ONCE_TIME,&err);
			if(flags == 0 && err ==  OS_TIMEOUT)
				ms -= SUCK_ONCE_TIME;
			else
			{
				ms = 0;
				break;
			}
		}
		else
		{
			OSTimeDly(SUCK_ONCE_TIME);	
			ms -= SUCK_ONCE_TIME;
		}
	}
	OSTimeDly(ms);
#ifdef MANUFACTURER_TENGFENG_EPSON	
	if(SUCK_INK)
	{
		CONTROL_OPEN_RELEASE_RELAY(0);
		OSTimeDly(release_time);
		CONTROL_CLOSE_SUCKER(0);	
	}
	else
		CONTROL_CLOSE_SUCKER(0);	
#else
	CONTROL_CLOSE_SUCKER(0);
	OSTimeDly(wait_time);
	CONTROL_OPEN_RELEASE_RELAY(0);
#ifndef MANUFACTURER_JHF		
	if(wait_time != 0)
	{
		CONTROL_OPEN_SUCKER(0);
		OSTimeDly(release_time);
		CONTROL_CLOSE_SUCKER(0);
		OSTimeDly(2000);
	}
#endif
#endif
}
#elif defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
extern void wiper_down(INT8U num);
extern void wiper_up(INT8U num);
#endif
#ifdef RICOH_CLEAN_PRESS
void Wiper_move_assembly(INT32U wiper_dis, INT32U x_start, INT32U x_end, INT32U x_speed,INT8U WipeTimes)
{
	INT8U buf[32],err;
	
	while(WipeTimes--!=0)
	{
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		buf[0] = 8; //Length						
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = 0; 
		*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = x_start;
		buf[6] = printer.movSpeed;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		buf[0] = 8; //Length						
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = 0; 
		*((__packed INT32S *)&buf[4]) = wiper_dis; //move_distance means target position
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = x_end;
		buf[6] = x_speed;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	}
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 8; //Length						
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = 0; 
	*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
}
#endif
void Set_Dis_Inf_For_Fill_Clean(void)
{
	if(UILanguage == en)
	{
		Dis_Inf_Case = "Fill Protect!\nPlease Wait %d Seconds And Retry.";
	}
	else
	{
		Dis_Inf_Case = "填充保护\n请在%d秒后重试";
	}
}
void Display_Fill_Inf(INT32U ms)
{
	Set_Dis_Inf_For_Fill_Clean();
	sprintf(Dis_Inf, Dis_Inf_Case,ms/1000);		
	LCD_SetMainInfoEx(E_IT_DIS_INF, (void *)Dis_Inf);
}
INT8U IS_FIRST_FILL = True;
INT8U DO_NOT_ANSWER_KEY = False;
INT8U CLEAN_head_group = 1;
#if defined(EPSON_CLEAN_UPDOWN)||defined(EPSON_CLEAN_INTEGRATE_FLASH_UNION)
void Flash_Before_Print(void)
{
	U8 err,times = 5,i;
	U8 buf[10] = {0};
	status_ReportStatus(STATUS_CLEANING, STATUS_SET);
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		                 
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
#ifdef EPSON_CLEAN_INTEGRATE_FLASH_UNION	
	*((__packed INT32S *)&buf[2]) = cleanparam_EPSON_MICOLOR.factory.Carriage_X_FlashPos;
#else
	*((__packed INT32S *)&buf[2]) = cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
#endif
	buf[6] = 3;  
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	
#if defined(HEAD_RICOH_G4)
	if(printerFlashing)
		FPGA_STOP_FLASH_Safe();
	
	OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err);
	if(err == OS_NO_ERR && !printerFlashing)
#else
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
#endif
		{
			buf[0] = 4;
			buf[1] = UART_HEAD_EPSON_JOBSTART;
			buf[2] = 1; //flash Job
			buf[3] = 2;
			while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
				OSTimeDly(10);
			OSTimeDly(100);         
		}
#ifdef HEAD_EPSON_GEN5	
	buf[0] = 6;
	buf[1] = UART_HEAD_EPSON_PASSSTART;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	UART_SendCMD(UART_HEAD_CHANNEL, buf);
	OSTimeDly(100);
#endif
#ifdef EPSON_CLEAN_INTEGRATE_FLASH_UNION	
	FPGA_START_FLASH(1000, 3);
	CONTROL_OPEN_SUCKER(0);
	OSTimeDly(1000);
	FPGA_STOP_FLASH_Safe();
	OSTimeDly(70);
	OSTimeDly(400);
	CONTROL_CLOSE_SUCKER(0);
#else
	while(times-- > 0)
	{
#ifdef HEAD_EPSON_GEN5	
		FPGA_START_FLASH_ALLWIN(1000, 3);
		OSTimeDly(30);
		FPGA_STOP_FLASH_Safe();
		OSTimeDly(70);			 
#else

#ifdef CONVERSION_BOARD
		FPGA_START_FLASH_ALLWIN(1000, 15);
		OSTimeDly(30);
		FPGA_STOP_FLASH();
		OSTimeDly(70);			 
#else
		FPGA_START_RICOH_AUTO_C();
		OSTimeDly(10);
#endif

#endif
	}
#endif	
#if defined(HEAD_RICOH_G4)
	OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err);
	if(err == OS_NO_ERR && !printerFlashing)
#else
		if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
#endif
		{
			buf[0] = 2;
			buf[1] = UART_HEAD_EPSON_JOBEND;
			while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
				OSTimeDly(10);									
			OSTimeDly(100);
		}
	
#if defined(HEAD_RICOH_G4)
	if(printerFlashing)
		FPGA_START_FLASH_RICOH(0xFF);
#endif
	
#ifndef EPSON_CLEAN_INTEGRATE_FLASH_UNION
	for(i =0; i<GetCleanPartCount(); i++)
	{
		if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask & (1<<i))
		{
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
			CONTROL_CLOSE_RELEASE_RELAY(i);
#endif
			CONTROL_OPEN_SUCKER(i);
		}
	}
	
	OSTimeDly(1000);			
	
	for(i =0; i<GetCleanPartCount(); i++)
	{
		if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask & (1<<i))
		{
			CONTROL_CLOSE_SUCKER(i);
		}
	}
#endif                    
	status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
}
#endif

INT8U GetCleanPartCount()
{
#ifdef HEAD_RICOH_G4
	return 4;
#elif DYSS_UV_CLEAN
	return 3;
#else
	INT8U CleanPartCount;
	if(factoryData.HeadType == HeadNo_Epson_Gen5) 
	{
		INT8U head_group = (COLOR_NUMBER_CALIBRATION * factoryDataEx.YInterleaveNum + NOZZLE_LINE_COUNT_PER_HEAD - 1)/ 
			NOZZLE_LINE_COUNT_PER_HEAD;
		return (EPSON_PERMIT_GROUPNUM * head_group)/CLEAN_head_group;
	}
	else
	{
		//wait more implement.
		return EPSON_PERMIT_GROUPNUM;
	}
#endif    
}
#ifdef EPSON_4H
INT8U Cal_Flash_mask(INT8U head_mask)
{
	INT8U Flash_Mask = 0,i = 0;
#ifdef EPSON_4H_CLEAN_SPLIT	
	if(head_mask ==1)
		head_mask = 2;
	else if(head_mask ==2)
		head_mask = 0x4;
	else if(head_mask ==4)
		head_mask = 0x1;	
	else
		head_mask = 0x8;
#else
	if(head_mask ==1)
		head_mask = 3;
	else if(head_mask ==2)
		head_mask = 0xc;
	else
		head_mask = 0xF;
#endif
	return head_mask;
#if 0    
	if(GetGroupHeadNumber() == 1)
	{
		INT8U shift_map[] = {2,3,1,4};
		for(i = 0; i < MAX_HEAD_NUMBER_CALIBRATION; i++)
		{
			if(head_mask & (1 << i))
				Flash_Mask |= (1 << (shift_map[i] - 1));
		}
	}
#endif  
}
#endif

#ifdef EPSON_CLEAN_UPDOWN_TATE_8H_RICOH                                                
void WIPE_TATE_RICOH(INT8U Wipe_Speed,INT8U Wipe_Times,INT32S Wiper_Pos,
					 INT32S Z_WipePos,INT32S X_WipePos_Start)
{
	INT8U buf[32],err,head_mask,times = Wipe_Times;
	
	if(EPSON_ACTIVE_GROUPNUM == 2)
		head_mask = 3;
	else
		head_mask = LCDMenuConfig.Active_HeadMask;
	if(head_mask & 0x2)
	{
		while(times-- > 0)
		{
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 8; //Length
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
			
			*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position         
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);					       
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = 0;
			buf[6] = 7;
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 8; //Length
			buf[1] = UART_MOVEDISTANCE_CMD; 
			buf[2] = 7; //1,2 X, 3,4 Y, 5,6 Z
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL; 
			*((__packed INT32S *)&buf[4]) = Wiper_Pos; 
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = X_WipePos_Start;
			buf[6] = 7;
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 8; //Length
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
			*((__packed INT32S *)&buf[4]) = Z_WipePos; //move_distance means target position        
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);					       
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 8; //Length
			buf[1] = UART_MOVEDISTANCE_CMD; 
			buf[2] = 8; //1,2 X, 3,4 Y, 5,6 Z
			buf[3] = Wipe_Speed; 
			*((__packed INT32S *)&buf[4]) = Wiper_Pos; 
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
		}
	}
	if(head_mask & 0x1)
	{		
		X_WipePos_Start -= factoryData.m_fHeadXGroupSpace * X_BASE_RES;
		while(Wipe_Times-- > 0)
		{
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 8; //Length
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
			*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position         
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);					       
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = 0;
			buf[6] = 7;
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 8; //Length
			buf[1] = UART_MOVEDISTANCE_CMD; 
			buf[2] = 7; //1,2 X, 3,4 Y, 5,6 Z
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL; 
			*((__packed INT32S *)&buf[4]) = Wiper_Pos; 
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = X_WipePos_Start;
			buf[6] = 7;
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 8; //Length
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
			*((__packed INT32S *)&buf[4]) = Z_WipePos; //move_distance means target position        
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);					       
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 8; //Length
			buf[1] = UART_MOVEDISTANCE_CMD; 
			buf[2] = 8; //1,2 X, 3,4 Y, 5,6 Z
			buf[3] = Wipe_Speed; 
			*((__packed INT32S *)&buf[4]) = Wiper_Pos; 
			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
			
		}
	}
	
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
	buf[0] = 8; //Length
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position         
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);					       
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = 0;
	buf[6] = 7;
	
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	
	
}
#endif

#ifdef EPSON_CLEAN_UPDOWN_FULGENCY
void FULGENCY_WIPE(INT8U Wipe_Speed,INT32S X_WipePos_Start,INT32S X_Wipe_End,INT8U head_mask)
{
	INT8U buf[32],err;
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);			
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = X_WipePos_Start;
	buf[6] = Wipe_Speed;
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

	wiper_up();
	//CLEAN_CTR_WIPER_OPEN();

	status_ReportStatus(STATUS_MOVING, STATUS_SET);			
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = X_Wipe_End;
	buf[6] = Wipe_Speed;
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

	wiper_down();
	//CLEAN_CTR_WIPER_CLOSE();
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);			
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = 0;
	buf[6] = Wipe_Speed;
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop	
		
}
#endif

#if (defined(EPSON_CLEAN_UPDOWN_WIPER_EX)||defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP))&&defined (EPSON_CLEAN_UPDOWN)
void motion_wipe_split(INT8U Wipe_Speed,INT8U Wipe_Times,INT32S Wiper_Pos,
					   INT32S Z_WipePos,INT32S X_WipePos_Start,INT32S X_Wipe_Distance,INT8U head_num)
{
	INT8U buf[32],err;
	
	while(Wipe_Times-- > 0)
	{
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
		buf[0] = 8; //Length
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)      
		*((__packed INT32S *)&buf[4]) = Z_WipePos; //move_distance means target position
#else						
		*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
#endif     
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);					       
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#if defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
		
#ifdef WIPER_TOGETHER
		if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 3)
			wiper_down(3);
		else
			wiper_down(head_num);
#else
		wiper_down(head_num);
#endif
		
#else
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
		buf[0] = 8; //Length
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
		*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position        
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);					       
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = 0;
		buf[6] = Wipe_Speed;
		
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = X_WipePos_Start;
		buf[6] = Wipe_Speed;
		
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		
#if defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
		
#ifdef WIPER_TOGETHER
		if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 3)
			wiper_up(3);
		else
			wiper_up(head_num);
#else
		wiper_up(head_num);
#endif
		
#else
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
		buf[0] = 8; //Length
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
		*((__packed INT32S *)&buf[4]) = Wiper_Pos; //move_distance means target position        
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);					       
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
		buf[0] = 8; //Length
		buf[1] = UART_AXISMOVETO_CMD; 
		buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
		buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
		*((__packed INT32S *)&buf[4]) = Z_WipePos; //move_distance means target position        
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);					       
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
		
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		
		buf[0] = 7; //Length
		buf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&buf[2]) = X_WipePos_Start + X_Wipe_Distance;
		buf[6] = Wipe_Speed;
		
		while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
	}
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
	buf[0] = 8; //Length
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)      
	*((__packed INT32S *)&buf[4]) = Z_WipePos; //move_distance means target position
#else						
	*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
#endif     
	
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);					       
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
	
#if defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
	
#ifdef WIPER_TOGETHER
	if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 3)
		wiper_down(3);
	else
		wiper_down(head_num);
#else
	wiper_down(head_num);
#endif
	
#else
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
	buf[0] = 8; //Length
	buf[1] = UART_AXISMOVETO_CMD; 
	buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position         
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);					       
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif
	
	status_ReportStatus(STATUS_MOVING, STATUS_SET);		
	buf[0] = 7; //Length
	buf[1] = UART_MOVETO_CMD;
	*((__packed INT32S *)&buf[2]) = 0;
	buf[6] = Wipe_Speed;
	
	while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
		OSTimeDly(100);			
	OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
}
#endif

//The priority of this task is higher than print task
void Clean_Task(void* data)
{
	INT8U err,i,j;
	OS_FLAGS flags;
	INT32S pos;
	INT8U buf[32];
	INT8S single_index;
	
	INT16U flash_interval_value, lefttime;
	INT8U  flash_onoff;
	static INT32U Last_Fill_Time = 0;
#if (defined( HEAD_2_SWAP)|| defined(UV_PRINTER)&&!defined(EPSON_4H))
	INT8U CLEAN_HEAD = 3;
#endif
	OSFlagPend(status_FLAG_GRP, STATUS_INI, OS_FLAG_WAIT_CLR_ALL,0,&err);
	OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
	while (1)
	{
		single_index = -3;
#if defined(ALLWIN_MEDIA_KEY) 		
		flags = OSFlagPend(status_FLAG_GRP, CMD_CLEAN_MANUAL|CMD_CLEAN_SINGLE|CMD_CLEAN_AUTO|CMD_START_MOVE|CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,50,&err); //Cleaning is request		
#else
		flags = OSFlagPend(status_FLAG_GRP, CMD_CLEAN_MANUAL|CMD_CLEAN_SINGLE|CMD_CLEAN_AUTO|CMD_START_MOVE|CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,0,&err); //Cleaning is request		
#endif
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop		
		
		flags = OSFlagAccept(status_FLAG_GRP, CMD_CLEAN_MANUAL|CMD_CLEAN_SINGLE|CMD_CLEAN_AUTO|CMD_START_MOVE|CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,&err);
		if (flags == 0)
		{
#if defined(ALLWIN_MEDIA_KEY)
		REPEAT_MOVE:
			if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			{
				if(PIO_Get(&ALLWIN_KEY_1)&& !PIO_Get(&ALLWIN_KEY_2))
				{
					if(ALLWIN_KEY_STATUS == 0x3)//进纸
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);
						buf[0] = 8; //Length									
						buf[1] = UART_PAPERFEED_CMD; 
						buf[2] = 1;
						buf[3] = printer.feedSpeed - 2;
						*((__packed INT32S *)&buf[4]) = 0;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(1);
						while(PIO_Get(&ALLWIN_KEY_1) && !PIO_Get(&ALLWIN_KEY_2))
							OSTimeDly(10);		
						
						buf[0] = 2;
						buf[1] = UART_STOPMOVE_CMD;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
							OSTimeDly(10);	
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
						
						ALLWIN_KEY_STATUS = 0x01;
					}
				}
				if(!PIO_Get(&ALLWIN_KEY_1)&& PIO_Get(&ALLWIN_KEY_2))
				{
					if(ALLWIN_KEY_STATUS == 0x3)//退纸
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);
						buf[0] = 8; //Length									
						buf[1] = UART_PAPERFEED_CMD; 
						buf[2] = 0;
						buf[3] = printer.feedSpeed - 2;
						*((__packed INT32S *)&buf[4]) = 0;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(1);
						while(!PIO_Get(&ALLWIN_KEY_1) && PIO_Get(&ALLWIN_KEY_2))
							OSTimeDly(10);	
						
						buf[0] = 2;
						buf[1] = UART_STOPMOVE_CMD;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
							OSTimeDly(10);	
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
						
						ALLWIN_KEY_STATUS = 0x2;
					}
				}
				if(PIO_Get(&ALLWIN_KEY_1) && PIO_Get(&ALLWIN_KEY_2))
				{
					if(ALLWIN_KEY_STATUS == 0x2)//退纸
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);
						buf[0] = 8; //Length									
						buf[1] = UART_PAPERFEED_CMD; 
						buf[2] = 0;
						buf[3] = printer.feedSpeed - 2 + ALLWIN_KEY_SPEED;
						*((__packed INT32S *)&buf[4]) = 0;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(1);
					}
					else if(ALLWIN_KEY_STATUS == 0x01)//进纸
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);
						buf[0] = 8; //Length									
						buf[1] = UART_PAPERFEED_CMD; 
						buf[2] = 1;
						buf[3] = printer.feedSpeed - 2 + ALLWIN_KEY_SPEED;
						*((__packed INT32S *)&buf[4]) = 0;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(1);
					}
					else
						continue;
					while(PIO_Get(&ALLWIN_KEY_1) && PIO_Get(&ALLWIN_KEY_2))
						OSTimeDly(10);
					
					buf[0] = 2;
					buf[1] = UART_STOPMOVE_CMD;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
						OSTimeDly(10);	
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
					
					OSTimeDly(40);
					
					ALLWIN_KEY_STATUS = 3;
					if(PIO_Get(&ALLWIN_KEY_1) || PIO_Get(&ALLWIN_KEY_2))	
					{
						if(PIO_Get(&ALLWIN_KEY_1) && PIO_Get(&ALLWIN_KEY_2))
							continue;
						else
							goto REPEAT_MOVE;
					}					
				}
				ALLWIN_KEY_STATUS = 3;
			}
#else		
			continue;
#endif
		}		
		
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		if (flags & CMD_CLEAN_AUTO)
		{
#if defined (EPSON_CLEAN_UPDOWN)&&!defined(EPSON_CLEAN_UPDOWN_FULGENCY)
			if (nextband_autoClean)
			{
				struct CleanParaEpsonAllwin_Factory ActiveFactory;
				struct CleanParaEpsonAllwin_Config ActiveConfig;
				struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
				INT8U bAutoCleanInPrint = False;
				INT16U Z_FlashPos = 0;
				INT8U Allwin_ActivedConfig = 0;
				
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);
				nextband_autoClean = False;
				
				OSSemPend(CleaningParamSem, 0, &err);
				memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
				memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
				if( ActiveCleanConfig != 0xFF)
				{
					ActiveRuntime.ActivedConfig = ActiveCleanConfig;
					ActiveCleanConfig = 0xFF;
				}
				Allwin_ActivedConfig = ActiveRuntime.ActivedConfig;
				memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonAllwin_Config));
				Z_FlashPos = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;
				OSSemPost(CleaningParamSem);
#if 0                
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 2;
					buf[1] = UART_HEAD_EPSON_JOBEND;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
						OSTimeDly(10);				
				}
#endif                
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
#ifdef HEAD_EPSON_GEN5
					buf[0] = 2;
					buf[1] = UART_HEAD_EPSON_PASSSTART;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
						OSTimeDly(10);
#else
#ifndef CLOSE_SSHAKE    
					
					SetSafeCmd(rFPGA_COMMAND_END_SSHAKE);
#endif
#endif
				}
#if 0				
				OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ANY,&err);
				if(!(err == OS_NO_ERR && curPos.x >= (INT16S)ActiveFactory.Carriage_X_SuckPos - PERMITTED_POSI_ERROR  && 
					 curPos.x <= (INT16S)ActiveFactory.Carriage_X_SuckPos + PERMITTED_POSI_ERROR))
				{   //check if capped.
#endif                
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length						
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
					*((__packed INT32S *)&buf[4]) = Z_SAFE_POSITION; //move_distance means target position
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					
					buf[0] = 7; //Length
					buf[1] = UART_MOVETO_CMD;
					*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
					buf[6] = printer.movSpeed;
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
		#if defined(FAST_CLEAN) && !defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					if (nextband_autoCleanUseFlash)
					{
						//如果非打印状态，则退出
						if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ANY,&err), err != 0 ||
						OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE | CMD_PAUSE, OS_FLAG_WAIT_SET_ALL,&err), err == 0)
						{
							nextband_autoCleanUseFlash = False;
							status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);
							status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
							continue;
						}
						
						//Z轴移动flash位置
						buf[0] = 8; //Length
						buf[1] = UART_AXISMOVETO_CMD; 
						buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
						buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
						*((__packed INT32S *)&buf[4]) = Z_FlashPos; //move_distance means target position
						
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					}
					else
					{
		#endif		//end FAST_CLEAN
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 8; //Length
						buf[1] = UART_AXISMOVETO_CMD; 
						buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
						buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					*((__packed INT32S *)&buf[4]) = 0X0000FFFF; //move_distance means target position
				#else
					*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
				#endif
						
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
		#if defined(FAST_CLEAN) && !defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					}
#endif		//end FAST_CLEAN
					
#if 0                    
				}
#endif                
#ifdef FAST_CLEAN
				if (nextband_autoCleanUseFlash)
				{
					nextband_autoCleanUseFlash = False;
					//快速清洗(闪喷），写死闪喷参数
					ActiveConfig.FlashFreqInterval = 1000;
					ActiveConfig.FlashTime = 120;
					ActiveConfig.FlashCycle = 40;
					ActiveConfig.FlashIdleInCycle = 10;
					
					lefttime = ActiveConfig.FlashTime;
					
					//清洗前进行抽墨
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
							CONTROL_CLOSE_RELEASE_RELAY(i);
#endif
							CONTROL_OPEN_SUCKER(i);
						}
					}
					//清洗：闪喷
					while(lefttime > 0)
					{
						buf[0] = 6;
						buf[1] = UART_HEAD_EPSON_PASSSTART;
						buf[2] = 0;
						buf[3] = 0;
						buf[4] = 0;
						buf[5] = 0;
						UART_SendCMD(UART_HEAD_CHANNEL, buf);
						OSTimeDly(100);
						
						FPGA_START_FLASH(ActiveConfig.FlashFreqInterval, 0x3);
						OSTimeDly((ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle)*100);
						FPGA_STOP_FLASH_Safe();
						
						if(lefttime > (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle))
							lefttime -= (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle);
						else
							break;
						
						OSTimeDly(ActiveConfig.FlashIdleInCycle*100);
						
						if(lefttime > ActiveConfig.FlashIdleInCycle)
							lefttime -= ActiveConfig.FlashIdleInCycle;
						else
							break;
					}
					
					//清洗（闪喷）后停止抽墨
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
							CONTROL_CLOSE_SUCKER(i);
						}
					}
					
					//移动Z轴到打印（0）位置
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
					*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					//查看是否是打印/暂停，如果是暂停，则Z轴移动到吸墨位置
					if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
					{
						//when printing and not pause, it means auto cleaning
						if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
							bAutoCleanInPrint = True;
					}
					if(!bAutoCleanInPrint)
					{ //for speed up. because capping is too slow.
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 8; //Length						
						buf[1] = UART_AXISMOVETO_CMD; 
						buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
						buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
						*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
						
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
						
						status_ReportStatus(STATUS_CAPPED, STATUS_SET);
					}

					//如果当前为暂停状态，则设置暂停状态标志
					OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
					if (err == OS_NO_ERR)
					{								
						OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err); //
						if (err == OS_NO_ERR)
						{	
							status_ReportStatus(STATUS_PAUSE, STATUS_SET);
						}
					}
					
					//清除清洗命令标志及自动清洗状态标志
					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);		
					status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
					
					continue;
				}
#endif			//end FAST_CLEAN

				for (j = 0; j < ActiveConfig.SuckTimes; j++)
				{
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
							CONTROL_CLOSE_RELEASE_RELAY(i);
#endif
							CONTROL_OPEN_SUCKER(i);
						}
					}
					if(Allwin_ActivedConfig == 3)
					{
						OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
					}
					
					lefttime = ActiveConfig.SuckInkTime;
					INT8U bBreak = FALSE;
					while(lefttime > 650)
					{
						if(Allwin_ActivedConfig == 3)
						{
							flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
							if(flags == 0 && err ==  OS_TIMEOUT)
								lefttime -= 650;
							else
							{
								bBreak = TRUE;
								break;
							}
						}
						else
						{
							OSTimeDly(650*100);	
							lefttime -= 650;
						}
					}
					if(!bBreak)
						OSTimeDly(lefttime*100);			
					//OSTimeDly(cleanPara.pauseIntervalAfterClean);
					
					
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
							CONTROL_CLOSE_SUCKER(i);
						}
					}
#ifdef EPSON_4H
					OSTimeDly(2000);
#else
					OSTimeDly(ActiveConfig.InputAirTime*100);
#endif
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
							CONTROL_OPEN_RELEASE_RELAY(i);
						}
					}
#endif                    
#ifdef EPSON_4H
					OSTimeDly(ActiveConfig.InputAirTime*100);
#endif
					if(ActiveConfig.CleanType != ACT_QUICKSUCK)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 8; //Length						
						buf[1] = UART_AXISMOVETO_CMD; 
						buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
						buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
		#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H) || defined(LECAI_EPSON_4H_Y_ROOLENCODER)      
						*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_WipePos; //move_distance means target position
#else						
						*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
#endif                                          
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					}
					
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
							CONTROL_CLOSE_RELEASE_RELAY(i);
#endif
							CONTROL_OPEN_SUCKER(i);
						}
					}
					
					OSTimeDly(ActiveConfig.SuckWasteInkTime*100);			
					
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
							CONTROL_CLOSE_SUCKER(i);
						}
					}
					
					if(ActiveConfig.CleanType != ACT_QUICKSUCK)
					{
						for(i =0; i<GetCleanPartCount(); i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
							{
#ifdef EPSON_CLEAN_UPDOWN_TATE_8H_RICOH                                                                            
								WIPE_TATE_RICOH(ActiveConfig.Carriage_X_Wipe_Speed, ActiveConfig.WipeTimes, ActiveFactory.WiperPos_Y[i], 
												ActiveFactory.HeadBox_Z_WipePos, ActiveFactory.Carriage_X_WipePos_Start[i]);
#elif defined(EPSON_CLEAN_UPDOWN_WIPER_EX)||defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
								motion_wipe_split(ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes,ActiveFactory.WiperPos_Y[i]
												  ,ActiveFactory.HeadBox_Z_WipePos,ActiveFactory.Carriage_X_WipePos_Start[i],ActiveFactory.Carriage_X_WipePos_End[i] - 
													  ActiveFactory.Carriage_X_WipePos_Start[i],ActiveRuntime.HeadMask & (1<<i));										
#else
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		              		              
								buf[0] = 20; //Length						
								buf[1] = UART_STARTSWIPE_CMD; 
								buf[2] = ActiveConfig.Carriage_X_Wipe_Speed; 
								buf[3] = ActiveConfig.WipeTimes; 
								*((__packed INT32S *)&buf[4]) =  ActiveFactory.WiperPos_Y[i];
								*((__packed INT32S *)&buf[8]) =  ActiveFactory.HeadBox_Z_WipePos;
								*((__packed INT32S *)&buf[12]) =  ActiveFactory.Carriage_X_WipePos_Start[i];
								*((__packed INT32S *)&buf[16]) =  (ActiveFactory.Carriage_X_WipePos_End[i] - 
																   ActiveFactory.Carriage_X_WipePos_Start[i]);
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif  
								
#ifdef WIPER_TOGETHER
								if(ActiveRuntime.HeadMask == 3)
								{
									break;
								}
#endif
							}
						}
						
						if(j < ActiveConfig.SuckTimes -1)
						{
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							buf[0] = 8; //Length
							buf[1] = UART_AXISMOVETO_CMD; 
							buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
							buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
					#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
							*((__packed INT32S *)&buf[4]) = 0X0000FFFF; //move_distance means target position
					#else
							*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
					#endif
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
						}
					}
				}				
				if(ActiveConfig.CleanType == ACT_QUICKSUCK)
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length						
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
		#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)  || defined(LECAI_EPSON_4H_Y_ROOLENCODER)    
					*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_WipePos; //move_distance means target position
#else						
					*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
#endif                                          
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					
					for(i =0; i<GetCleanPartCount(); i++)
					{
						if(ActiveRuntime.HeadMask & (1<<i))
						{
#ifdef EPSON_CLEAN_UPDOWN_TATE_8H_RICOH                                                                            
							WIPE_TATE_RICOH(ActiveConfig.Carriage_X_Wipe_Speed, ActiveConfig.WipeTimes, ActiveFactory.WiperPos_Y[i], 
											ActiveFactory.HeadBox_Z_WipePos, ActiveFactory.Carriage_X_WipePos_Start[i]);
#elif defined(EPSON_CLEAN_UPDOWN_WIPER_EX)||defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
							motion_wipe_split(ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes,ActiveFactory.WiperPos_Y[i]
											  ,ActiveFactory.HeadBox_Z_WipePos,ActiveFactory.Carriage_X_WipePos_Start[i],ActiveFactory.Carriage_X_WipePos_End[i] - 
												  ActiveFactory.Carriage_X_WipePos_Start[i],ActiveRuntime.HeadMask & (1<<i));										
#else
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		                            buf[0] = 20; //Length						
							buf[1] = UART_STARTSWIPE_CMD; 
							buf[2] = ActiveConfig.Carriage_X_Wipe_Speed; 
							buf[3] = ActiveConfig.WipeTimes; 
							*((__packed INT32S *)&buf[4]) =  ActiveFactory.WiperPos_Y[i];
							*((__packed INT32S *)&buf[8]) =  ActiveFactory.HeadBox_Z_WipePos;
							*((__packed INT32S *)&buf[12]) =  ActiveFactory.Carriage_X_WipePos_Start[i];
							*((__packed INT32S *)&buf[16]) =  (ActiveFactory.Carriage_X_WipePos_End[i] - 
															   ActiveFactory.Carriage_X_WipePos_Start[i]);
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
#endif   
							
#ifdef WIPER_TOGETHER
							if(ActiveRuntime.HeadMask == 3)
							{
								break;
							}
#endif
							
						}
					}
				}
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				*((__packed INT32S *)&buf[4]) = Z_FlashPos; //move_distance means target position
				
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				Send_Second_Backhaul();
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
#ifdef EPSON_4H
				INT8U Flash_mask = Cal_Flash_mask(ActiveRuntime.HeadMask);
#endif
				if(ActiveConfig.FlashTime > 0)
				{                        
					INT8U send_jobstart= False;
		#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					Open_Induced_Draft();   // open induced draft
					OSTimeDly(2000);
		#endif
					if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
					{
						buf[0] = 4;
						buf[1] = UART_HEAD_EPSON_JOBSTART;
						buf[2] = 1; //flash Job
						buf[3] = 2;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
							OSTimeDly(10);
						send_jobstart = True;
					}
					
					OSTimeDly(100);
					
					lefttime = ActiveConfig.FlashTime;
					//lefttime = 0xffff;
					
					while(lefttime > 0)
					{
						buf[0] = 6;
						buf[1] = UART_HEAD_EPSON_PASSSTART;
						buf[2] = 0;
						buf[3] = 0;
						buf[4] = 0;
						buf[5] = 0;
						UART_SendCMD(UART_HEAD_CHANNEL, buf);
						OSTimeDly(100);
#if (defined( HEAD_2_SWAP)|| defined(UV_PRINTER)&&!defined(EPSON_4H))
						//#if (defined( HEAD_2_SWAP)&&!defined(EPSON_4H))
						if(ActiveRuntime.HeadMask != 3)
						{
							if(ActiveRuntime.HeadMask == 1)
								CLEAN_HEAD = 2;
							else
								CLEAN_HEAD = 1;
						}
						else
						{
							CLEAN_HEAD = 3;
						}
						FPGA_START_FLASH_ALLWIN(ActiveConfig.FlashFreqInterval, CLEAN_HEAD);
#else
#ifdef EPSON_4H
						FPGA_START_FLASH_ALLWIN(ActiveConfig.FlashFreqInterval, Flash_mask);
#else
						FPGA_START_FLASH_ALLWIN(ActiveConfig.FlashFreqInterval, ActiveRuntime.HeadMask);
#endif
#endif
						//FPGA_START_FLASH(cleanparam_EPSON_ALLWIN.FlashFreqInterval, 0);
						OSTimeDly((ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle)*100);
						//#ifdef A1802      //commented by cheney.
						//							FPGA_STOP_FLASH(); //解决自动清洗闪喷线问题
						//#else
						FPGA_STOP_FLASH_Safe();
						//#endif
						
						if(lefttime > (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle))
							lefttime -= (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle);
						else
							break;
						
						OSTimeDly(ActiveConfig.FlashIdleInCycle*100);
						
						if(lefttime > ActiveConfig.FlashIdleInCycle)
							lefttime -= ActiveConfig.FlashIdleInCycle;
						else
							break;
					}
					if(send_jobstart)
					{
						buf[0] = 2;
						buf[1] = UART_HEAD_EPSON_JOBEND;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
							OSTimeDly(10);		
					}
					else
					{
#ifdef HEAD_RICOH_G4
#ifndef CLOSE_SSHAKE    
						
						SetSafeCmd(rFPGA_COMMAND_BGN_SSHAKE);	
#endif
#endif
					}
		#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					OSTimeDly(2000);
					Close_Induced_Draft();   // close induced draft
		#endif					
				}
#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)      
				
#else                
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
				
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
#endif                
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask && (1<<i))
					{
						CONTROL_OPEN_SUCKER(i);
					}
				}
				
				OSTimeDly(ActiveConfig.SuckWasteInkTime*100);			
				
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask && (1<<i))
					{
						CONTROL_CLOSE_SUCKER(i);
					}
				}
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				Send_Second_Backhaul();
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					//when printing and not pause, it means auto cleaning
					if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
						bAutoCleanInPrint = True;
				}
				if(!bAutoCleanInPrint)
				{ //for speed up. because capping is too slow.
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length						
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
			#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
					*((__packed INT32S *)&buf[4]) = 0xFFFF; 
			#else
					*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
			#endif
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					status_ReportStatus(STATUS_CAPPED, STATUS_SET);
				}
#if 0                
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 4;
					buf[1] = UART_HEAD_EPSON_JOBSTART;
					buf[2] = 0; //flash Job
					buf[3] = job_info.VSDModel;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
						OSTimeDly(10);
					OSTimeDly(100);
				}
#endif                
				OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
				if (err == OS_NO_ERR)
				{								
					OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err); //20120914
					if (err == OS_NO_ERR)
					{	
						status_ReportStatus(STATUS_PAUSE, STATUS_SET);
					}
				}
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
				status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
			}
			else 
#elif defined (EPSON_CLEAN_UPDOWN_FULGENCY)

			if (nextband_autoClean)
			{
				struct CleanParaEpsonAllwin_Factory ActiveFactory;
				struct CleanParaEpsonAllwin_Config ActiveConfig;
				struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
				INT8U bAutoCleanInPrint = False;
				INT16U Z_FlashPos = 0;
				INT8U Allwin_ActivedConfig = 0;
				
				if(PIO_Get(&FULGENCY_WASTE_INK))
				{
					nextband_autoClean = False;
					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
					status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
					status_ReportStatus(STATUS_FTA_WASTE_INK_FULL, STATUS_SET);
					continue;
				}
				
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);
				nextband_autoClean = False;
				
				OSSemPend(CleaningParamSem, 0, &err);
				memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
				memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
				if( ActiveCleanConfig != 0xFF)
				{
					ActiveRuntime.ActivedConfig = ActiveCleanConfig;
					ActiveCleanConfig = 0xFF;
				}
				Allwin_ActivedConfig = ActiveRuntime.ActivedConfig;
				memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonAllwin_Config));
				Z_FlashPos = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;
				OSSemPost(CleaningParamSem);
  
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{

					buf[0] = 2;
					buf[1] = UART_HEAD_EPSON_PASSSTART;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
						OSTimeDly(10);

				}
              
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length						
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
					*((__packed INT32S *)&buf[4]) = Z_SAFE_POSITION; //move_distance means target position
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					
					buf[0] = 7; //Length
					buf[1] = UART_MOVETO_CMD;
					*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
					buf[6] = printer.movSpeed;
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
					
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 8; //Length
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
					*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop             
				
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask & (1<<i))
					{
						CONTROL_OPEN_SUCKER(i);
					}
				}
				if(Allwin_ActivedConfig == 3)
				{
					OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
				}
				
				lefttime = ActiveConfig.SuckInkTime;
				INT8U bBreak = FALSE;
				while(lefttime > 650)
				{
					if(Allwin_ActivedConfig == 3)
					{
						flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
						if(flags == 0 && err ==  OS_TIMEOUT)
							lefttime -= 650;
						else
						{
							bBreak = TRUE;
							break;
						}
					}
					else
					{
						OSTimeDly(650*100);	
						lefttime -= 650;
					}
				}
				if(!bBreak)
					OSTimeDly(lefttime*100);			
				//OSTimeDly(cleanPara.pauseIntervalAfterClean);
				
				
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask & (1<<i))
					{
						CONTROL_CLOSE_SUCKER(i);
					}
				}

				OSTimeDly(ActiveConfig.InputAirTime*100);

				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;					
				*((__packed INT32S *)&buf[4]) = Z_FlashPos; //move_distance means target position                                        
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
								

				INT8U Flash_mask = Cal_Flash_mask(ActiveRuntime.HeadMask);

				if(ActiveConfig.FlashTime > 0)
				{                        
					INT8U send_jobstart= False;
					if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
					{
						buf[0] = 4;
						buf[1] = UART_HEAD_EPSON_JOBSTART;
						buf[2] = 1; //flash Job
						buf[3] = 2;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
							OSTimeDly(10);
						send_jobstart = True;
					}
					
					OSTimeDly(100);
					
					lefttime = ActiveConfig.FlashTime;
					//lefttime = 0xffff;
					
					while(lefttime > 0)
					{
						buf[0] = 6;
						buf[1] = UART_HEAD_EPSON_PASSSTART;
						buf[2] = 0;
						buf[3] = 0;
						buf[4] = 0;
						buf[5] = 0;
						UART_SendCMD(UART_HEAD_CHANNEL, buf);
						OSTimeDly(100);
						
						FPGA_START_FLASH_ALLWIN(ActiveConfig.FlashFreqInterval, Flash_mask);


						OSTimeDly((ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle)*100);

						FPGA_STOP_FLASH_Safe();
						
						if(lefttime > (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle))
							lefttime -= (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle);
						else
							break;
						
						OSTimeDly(ActiveConfig.FlashIdleInCycle*100);
						
						if(lefttime > ActiveConfig.FlashIdleInCycle)
							lefttime -= ActiveConfig.FlashIdleInCycle;
						else
							break;
					}
					if(send_jobstart)
					{
						buf[0] = 2;
						buf[1] = UART_HEAD_EPSON_JOBEND;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
							OSTimeDly(10);		
					}

				}								
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;   
				*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_WipePos; //move_distance means target position                                  
				
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask && (1<<i))
					{
						CONTROL_OPEN_SUCKER(i);
					}
				}
				
				OSTimeDly(ActiveConfig.SuckWasteInkTime*100);			
				
				for(i =0; i<GetCleanPartCount(); i++)
				{
					if(ActiveRuntime.HeadMask && (1<<i))
					{
						CONTROL_CLOSE_SUCKER(i);
					}
				}
																	
				FULGENCY_WIPE(ActiveConfig.Carriage_X_Wipe_Speed,ActiveFactory.Carriage_X_WipePos_Start[0],ActiveFactory.Carriage_X_WipePos_End[0],ActiveRuntime.HeadMask);		
								
  				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				Send_Second_Backhaul();
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 8; //Length						
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;					
				*((__packed INT32S *)&buf[4]) = Z_FlashPos; //move_distance means target position                                        
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				
				OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
				if (err == OS_NO_ERR)
				{								
					OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err); //20120914
					if (err == OS_NO_ERR)
					{	
						status_ReportStatus(STATUS_PAUSE, STATUS_SET);
					}
				}
				            
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
				status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
			}
			else 
#elif defined (DYSS_UV_CLEAN)
				if (nextband_autoClean)
				{
					struct CleanParaEpsonAllwin_Factory ActiveFactory;
					struct CleanParaEpsonAllwin_Config ActiveConfig;
					struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
					INT8U bAutoCleanInPrint = False;
					INT8U Allwin_ActivedConfig = 0;
					status_ReportStatus(STATUS_CLEANING, STATUS_SET);
					nextband_autoClean = False;
					OSSemPend(CleaningParamSem, 0, &err);
					memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
					memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
					if( ActiveCleanConfig != 0xFF)
					{
						ActiveRuntime.ActivedConfig = ActiveCleanConfig;
						ActiveCleanConfig = 0xFF;
					}
					Allwin_ActivedConfig = ActiveRuntime.ActivedConfig;
					memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonAllwin_Config));
					OSSemPost(CleaningParamSem);
					INT8U send_jobstart= False;
					DisableFPGAPosiIT(0);
					if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
					{
						buf[0] = 4;
						buf[1] = UART_HEAD_EPSON_JOBSTART;
						buf[2] = 1; //flash Job
						buf[3] = 2;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
							OSTimeDly(10);
						send_jobstart = True;
					}								
					else
					{
#ifndef CLOSE_SSHAKE    		
						SetSafeCmd(rFPGA_COMMAND_END_SSHAKE);	
#endif
					}
					if(DYSS_CLEAN_FLASH != 2)
					{
						if(DYSS_CLEAN_FLASH == 1)
							DYSS_DO_FLASH(0);
						else
							DYSS_DO_FLASH(1);
					}
					else
					{
						if(PIO_Get(&DYSS_CLEAN_SENSOR1)!= DYSS_CLEAN_SENSOR1_ON)
						{			
							DYSS_CLEAN_TURN_MOTOR_SENSOR4();
							status_ReportStatus(STATUS_MOVING, STATUS_SET); 	
							buf[0] = 7; //Length
							buf[1] = UART_MOVETO_CMD;
							*((__packed INT32S *)&buf[2]) = ActiveFactory.HeadBox_Z_SuckPos;
							buf[6] = printer.movSpeed;
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100); 		
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
							DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR1();
						}
						
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
						buf[6] = printer.movSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR2();
						DYSS_CLEAN_TURN_MOTOR_SENSOR5();
						
						for(i =0; i<GetCleanPartCount(); i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
							{
								CONTROL_OPEN_SUCKER(i);
							}
						}
						if(Allwin_ActivedConfig == 3)
						{
							OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
						}
						lefttime = ActiveConfig.SuckInkTime;
						while(lefttime > 650)
						{
							if(Allwin_ActivedConfig == 3)
							{
								flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
								if(flags == 0 && err ==  OS_TIMEOUT)
									lefttime -= 650;
								else
								{
									break;
								}
							}
							else
							{
								OSTimeDly(650*100);	
								lefttime -= 650;
							}
						}
						if(Allwin_ActivedConfig != 3||(flags == 0 && err ==  OS_TIMEOUT))
							OSTimeDly(lefttime*100);			
						for(i =0; i<GetCleanPartCount(); i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
							{
								CONTROL_CLOSE_SUCKER(i);
							}
						}
						OSTimeDly(ActiveConfig.InputAirTime*100);
						
						DYSS_CLEAN_TURN_MOTOR_SENSOR4();
						for(i =0; i<GetCleanPartCount(); i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
							{
								CONTROL_OPEN_SUCKER(i);
							}
						}			
						
						DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR3_BYTIME(ActiveConfig.SuckWasteInkTime*100);
						
						for(i =0; i<GetCleanPartCount(); i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
							{
								CONTROL_CLOSE_SUCKER(i);
							}
						}			
						if((LCDMenuConfig_EX.b_Sundry & DYSS_FLASH_AFTER_SUCK) != 0)
							DYSS_DO_FLASH(2);
						else
						{
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							buf[0] = 7; //Length
							buf[1] = UART_MOVETO_CMD;
							*((__packed INT32S *)&buf[2]) = 0;
							buf[6] = printer.movSpeed;
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						}
						
					}
					if(send_jobstart)
					{
						buf[0] = 2;
						buf[1] = UART_HEAD_EPSON_JOBEND;
						while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
							OSTimeDly(10);		
					}
					else
					{
#ifndef CLOSE_SSHAKE    
						SetSafeCmd(rFPGA_COMMAND_BGN_SSHAKE);	
#endif
					}
					EnableFPGAPosiIT(0);	
					DYSS_CLEAN_FLASH = 0;      

					OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
					if (err == OS_NO_ERR)
					{	
						OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err); //20120914
						if (err == OS_NO_ERR)
						{	
							status_ReportStatus(STATUS_PAUSE, STATUS_SET);
						}
					}
					            
					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
					status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
				}
				else 
					
#elif defined (RICOH_CLEAN_NORMAL)	//zgf
					if (nextband_autoClean)
					{
						struct CleanParaEpsonAllwin_Factory ActiveFactory;
						struct CleanParaEpsonAllwin_Config ActiveConfig;
						struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
						INT8U bAutoCleanInPrint = False;
						INT8U Allwin_ActivedConfig = 0;
						
						status_ReportStatus(STATUS_CLEANING, STATUS_SET);
						nextband_autoClean = False;
						
						OSSemPend(CleaningParamSem, 0, &err);
						memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
						memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
						if( ActiveCleanConfig != 0xFF)
						{
							ActiveRuntime.ActivedConfig = ActiveCleanConfig;
							ActiveCleanConfig = 0xFF;
						}
						Allwin_ActivedConfig = ActiveRuntime.ActivedConfig;
						memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonAllwin_Config));
						OSSemPost(CleaningParamSem);
						
						if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
						{
							buf[0] = 2;
							buf[1] = UART_HEAD_EPSON_JOBEND;
							while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
								OSTimeDly(10);				
						}
						
						OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ANY,&err);
						if(!(err == OS_NO_ERR && curPos.x >= (INT16S)ActiveFactory.Carriage_X_SuckPos - PERMITTED_POSI_ERROR  && 
							 curPos.x <= (INT16S)ActiveFactory.Carriage_X_SuckPos + PERMITTED_POSI_ERROR))
						{   			
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
							
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							
							buf[0] = 7; //Length
							buf[1] = UART_MOVETO_CMD;
							*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
							buf[6] = printer.movSpeed;
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						}
						
						
						for(i =0; i<COLOR_NUMBER_CALIBRATION; i++)
						{
							if(ActiveRuntime.HeadMask & (1<<i))
								CLEAN_PumpOpenMask(ActiveRuntime.HeadMask & (1<<i));
						}
						
						OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
						
						while(1)
						{
							flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
							if(flags == 0 && err ==  OS_TIMEOUT)
								continue;
							break;
						}
						
						for(i =0; i<COLOR_NUMBER_CALIBRATION; i++)
							CLEAN_PumpCloseMask(ActiveRuntime.HeadMask & (1<<i));
						
						status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
						status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
					}
					else 			
#elif defined (RICOH_CLEAN_PRESS)
						if (nextband_autoClean)
						{
							struct CleanParaEpsonAllwin_Factory ActiveFactory;
							struct CleanParaEpsonAllwin_Config ActiveConfig;
							struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
							INT8U bAutoCleanInPrint = False;
							INT16U Z_FlashPos = 0;
							INT8U Allwin_ActivedConfig = 0;
							INT32U WIPER_OFFSET = 0;
							
							status_ReportStatus(STATUS_CLEANING, STATUS_SET);
							nextband_autoClean = False;
							if(EPSON_PERMIT_GROUPNUM > 1)
								WIPER_OFFSET = factoryData.m_fHeadXGroupSpace * X_BASE_RES;
							else
								WIPER_OFFSET = factoryData.m_fHeadXColorSpace * X_BASE_RES;
							OSSemPend(CleaningParamSem, 0, &err);
							memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
							memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
							if( ActiveCleanConfig != 0xFF)
							{
								ActiveRuntime.ActivedConfig = ActiveCleanConfig;
								ActiveCleanConfig = 0xFF;
							}
							Allwin_ActivedConfig = ActiveRuntime.ActivedConfig;
							memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonAllwin_Config));
							Z_FlashPos = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;
							OSSemPost(CleaningParamSem);
							
							if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
							{
								buf[0] = 2;
								buf[1] = UART_HEAD_EPSON_JOBEND;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
									OSTimeDly(10);				
							}
							
							OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ANY,&err);
							if(!(err == OS_NO_ERR && curPos.x >= (INT16S)ActiveFactory.Carriage_X_SuckPos - PERMITTED_POSI_ERROR  && 
								 curPos.x <= (INT16S)ActiveFactory.Carriage_X_SuckPos + PERMITTED_POSI_ERROR))
							{   //check if capped.
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								buf[0] = 8; //Length						
								buf[1] = UART_AXISMOVETO_CMD; 
								buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
								buf[3] = 0; 
								*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
								
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								
								buf[0] = 7; //Length
								buf[1] = UART_MOVETO_CMD;
								*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
								buf[6] = printer.movSpeed;
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
								
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								buf[0] = 8; //Length
								buf[1] = UART_AXISMOVETO_CMD; 
								buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
								buf[3] = 0; 
								*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
							}
							
							for (j = 0; j < ActiveConfig.SuckTimes; j++)
							{
								for(i =0; i<COLOR_NUMBER_CALIBRATION; i++)
								{
									if(ActiveRuntime.HeadMask & (1<<i))
									{
										CLEAN_PumpOpenMask(ActiveRuntime.HeadMask & (1<<i));
									}
								}
								if(Allwin_ActivedConfig == 3)
								{
									OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
								}
								
								lefttime = ActiveConfig.SuckInkTime;
								INT8U bBreak = FALSE;
								while(lefttime > 650)
								{
									if(Allwin_ActivedConfig == 3)
									{
										flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
										if(flags == 0 && err ==  OS_TIMEOUT)
											lefttime -= 650;
										else
										{
											bBreak = TRUE;
											break;
										}
									}
									else
									{
										OSTimeDly(650*100);	
										lefttime -= 650;
									}
								}
								if(!bBreak)
									OSTimeDly(lefttime*100);			
								//OSTimeDly(cleanPara.pauseIntervalAfterClean);
								
								
								for(i =0; i<COLOR_NUMBER_CALIBRATION; i++)
								{
									CLEAN_PumpCloseMask(ActiveRuntime.HeadMask & (1<<i));
								}
								
								if(ActiveConfig.CleanType != ACT_QUICKSUCK)
								{
									status_ReportStatus(STATUS_MOVING, STATUS_SET);		
									buf[0] = 8; //Length						
									buf[1] = UART_AXISMOVETO_CMD; 
									buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
									buf[3] = 0; 
									*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
									
									while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
										OSTimeDly(100);			
									OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
								}
								
								if(ActiveConfig.CleanType != ACT_QUICKSUCK)
								{
#if 0                    
									for(i =0; i<EPSON_PERMIT_HEADNUM; i++)
									{
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
										if(ActiveRuntime.HeadMask & (1<<i))
										{
											buf[0] = 20; //Length						
											buf[1] = UART_STARTSWIPE_CMD; 
											buf[2] = ActiveConfig.Carriage_X_Wipe_Speed; 
											buf[3] = ActiveConfig.WipeTimes; 
											*((__packed INT32S *)&buf[4]) =  ActiveFactory.WiperPos_Y[i];
											*((__packed INT32S *)&buf[8]) =  ActiveFactory.HeadBox_Z_WipePos;
											*((__packed INT32S *)&buf[12]) =  ActiveFactory.Carriage_X_WipePos_Start[i];
											*((__packed INT32S *)&buf[16]) =  (ActiveFactory.Carriage_X_WipePos_End[i] - 
																			   ActiveFactory.Carriage_X_WipePos_Start[i]);
											
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop			
										}
									}
#else					
									if(EPSON_PERMIT_GROUPNUM = 1)
									{
										Wiper_move_assembly(ActiveFactory.WiperPos_Y[0],ActiveFactory.Carriage_X_WipePos_Start[0],ActiveFactory.Carriage_X_WipePos_End[0],ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes );
										for(i = 1; i < COLOR_NUMBER_CALIBRATION;i++)
										{
											Wiper_move_assembly(ActiveFactory.WiperPos_Y[0],ActiveFactory.Carriage_X_WipePos_End[0]+WIPER_OFFSET*(i-1),ActiveFactory.Carriage_X_WipePos_End[0]+WIPER_OFFSET*i,ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes );
										}
									}
									else
									{
										Wiper_move_assembly(ActiveFactory.WiperPos_Y[0],ActiveFactory.Carriage_X_WipePos_Start[0],ActiveFactory.Carriage_X_WipePos_End[0],ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes );
										for(i = 1; i < COLOR_NUMBER_CALIBRATION*EPSON_PERMIT_GROUPNUM;i++)
										{
											Wiper_move_assembly(ActiveFactory.WiperPos_Y[i%2],ActiveFactory.Carriage_X_WipePos_End[0]+WIPER_OFFSET*(i-1),ActiveFactory.Carriage_X_WipePos_End[0]+WIPER_OFFSET*i,ActiveConfig.Carriage_X_Wipe_Speed,ActiveConfig.WipeTimes );
										}
									}
#endif
									
									if(j < ActiveConfig.SuckTimes -1)
									{
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										buf[0] = 8; //Length
										buf[1] = UART_AXISMOVETO_CMD; 
										buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
										buf[3] = 0; 
										*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
										
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
									}
								}
							}				
							if(ActiveConfig.CleanType == ACT_QUICKSUCK)
							{
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								buf[0] = 8; //Length						
								buf[1] = UART_AXISMOVETO_CMD; 
								buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
								buf[3] = 0; 
								*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
								
								
								for(i =0; i<EPSON_PERMIT_GROUPNUM; i++)
								{
									status_ReportStatus(STATUS_MOVING, STATUS_SET);		
									if(ActiveRuntime.HeadMask & (1<<i))
									{
										buf[0] = 20; //Length						
										buf[1] = UART_STARTSWIPE_CMD; 
										buf[2] = ActiveConfig.Carriage_X_Wipe_Speed; 
										buf[3] = ActiveConfig.WipeTimes; 
										*((__packed INT32S *)&buf[4]) =  ActiveFactory.WiperPos_Y[i];
										*((__packed INT32S *)&buf[8]) =  ActiveFactory.HeadBox_Z_WipePos;
										*((__packed INT32S *)&buf[12]) =  ActiveFactory.Carriage_X_WipePos_Start[i];
										*((__packed INT32S *)&buf[16]) =  (ActiveFactory.Carriage_X_WipePos_End[i] - 
																		   ActiveFactory.Carriage_X_WipePos_Start[i]);
										
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop			
									}
								}
							}
							
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							buf[0] = 8; //Length						
							buf[1] = UART_AXISMOVETO_CMD; 
							buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
							buf[3] = 0; 
							*((__packed INT32S *)&buf[4]) = Z_FlashPos; //move_distance means target position
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
							
							if(ActiveConfig.FlashTime > 0)
							{                        
								buf[0] = 4;
								buf[1] = UART_HEAD_EPSON_JOBSTART;
								buf[2] = 1; //flash Job
								buf[3] = 2;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
									OSTimeDly(10);
								OSTimeDly(100);
								
								lefttime = ActiveConfig.FlashTime;
								//lefttime = 0xffff;
								
								while(lefttime > 0)
								{
									buf[0] = 6;
									buf[1] = UART_HEAD_EPSON_PASSSTART;
									buf[2] = 0;
									buf[3] = 0;
									buf[4] = 0;
									buf[5] = 0;
									UART_SendCMD(UART_HEAD_CHANNEL, buf);
									OSTimeDly(100);
									
									FPGA_START_FLASH_ALLWIN(ActiveConfig.FlashFreqInterval, ActiveRuntime.HeadMask);
									//FPGA_START_FLASH(cleanparam_EPSON_ALLWIN.FlashFreqInterval, 0);
									OSTimeDly((ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle)*100);
									FPGA_STOP_FLASH_Safe();
									
									if(lefttime > (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle))
										lefttime -= (ActiveConfig.FlashCycle - ActiveConfig.FlashIdleInCycle);
									else
										break;
									
									OSTimeDly(ActiveConfig.FlashIdleInCycle*100);
									
									if(lefttime > ActiveConfig.FlashIdleInCycle)
										lefttime -= ActiveConfig.FlashIdleInCycle;
									else
										break;
								}
								
								buf[0] = 2;
								buf[1] = UART_HEAD_EPSON_JOBEND;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
									OSTimeDly(10);		                    
							}
							
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							buf[0] = 8; //Length						
							buf[1] = UART_AXISMOVETO_CMD; 
							buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
							buf[3] = 0; 
							*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
							
							
							if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
							{
								//when printing and not pause, it means auto cleaning
								if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
									bAutoCleanInPrint = True;
							}
							if(!bAutoCleanInPrint)
							{ //for speed up. because capping is too slow.
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								buf[0] = 8; //Length						
								buf[1] = UART_AXISMOVETO_CMD; 
								buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
								buf[3] = 0; 
								*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
								
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
								
								status_ReportStatus(STATUS_CAPPED, STATUS_SET);
							}
							if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
							{
								buf[0] = 4;
								buf[1] = UART_HEAD_EPSON_JOBSTART;
								buf[2] = 0; //flash Job
								buf[3] = job_info.VSDModel;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
									OSTimeDly(10);
								OSTimeDly(100);
							}
							status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
							status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
						}
						else 				
#elif defined (EPSON_CLEAN_INTEGRATE)
							if (nextband_autoClean)
							{
								struct CleanParaEpsonMicolor_Factory ActiveFactory;
								struct CleanParaEpsonMicolor_Config ActiveConfig;
								struct CleanParaEpsonMicolor_Runtime ActiveRuntime;
								INT8U CleanConfig = 0;
								INT8U Is_Break = False;
								status_ReportStatus(STATUS_CLEANING, STATUS_SET);
								nextband_autoClean = False;
								
								OSSemPend(CleaningParamSem, 0, &err);
								memcpy(&ActiveFactory, &cleanparam_EPSON_MICOLOR.factory, sizeof(struct CleanParaEpsonMicolor_Factory));
								memcpy(&ActiveRuntime, &cleanparam_EPSON_MICOLOR.Runtime, sizeof(struct CleanParaEpsonMicolor_Runtime));
								if(ActiveCleanConfig != 0xFF)
								{
									ActiveRuntime.ActivedConfig = ActiveCleanConfig;
									ActiveCleanConfig = 0xFF;
								}
								CleanConfig = ActiveRuntime.ActivedConfig;
								memcpy(&ActiveConfig, &cleanparam_EPSON_MICOLOR.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonMicolor_Config));
								OSSemPost(CleaningParamSem);
								
								if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
								{
									buf[0] = 2;
									buf[1] = UART_HEAD_EPSON_JOBEND;
									while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
										OSTimeDly(10);				
								}
								
								for(i=0; i<ActiveConfig.sectionCount; i++)
								{
									if(ActiveConfig.sections[i].type == PreClean)
									{
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
										buf[6] = printer.movSpeed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, 
														 ActiveConfig.sections[i].MoveSpeed);
										
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
										buf[6] = printer.movSpeed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
										if(i == ActiveConfig.sectionCount-1 || (ActiveConfig.sections[i+1].type == FlashOnly || 
																				ActiveConfig.sections[i+1].type == None || ActiveConfig.sections[i+1].type == PreClean))
											RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);
									}
									if(ActiveConfig.sections[i].type == SuckOnly || 
									   ActiveConfig.sections[i].type == SuckAndWipe || 
										   ActiveConfig.sections[i].type == SuckWipeAndFlash)
									{
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
										buf[6] = printer.movSpeed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										if(i == 0 || ActiveConfig.sections[i-1].type != PreClean)
											RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, 
															 ActiveConfig.sections[i].MoveSpeed);
										
										for(j=0; j<ActiveConfig.sections[i].LoopTimes; j++)
										{
											if(CleanConfig == 3 && i == 0 )
											{
												if(IS_FIRST_FILL == True)
												{
													IS_FIRST_FILL = False;
													OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
												}
												else
												{
													if(Last_Fill_Time + 60*1000 <= OSTimeGet())
													{
														OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
													}
													else
													{
														if(cleanparam_EPSON_MICOLOR.Clean_Fill_Safe == True)
														{
															DO_NOT_ANSWER_KEY = True;
															Display_Fill_Inf(60*1000-(OSTimeGet() -Last_Fill_Time));
															DO_NOT_ANSWER_KEY = False;
															OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
															Is_Break = True;
															break;
														}
														OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
													}
												}
											}
											RotateCleanMotorByTime(True, ActiveConfig.sections[i].SuckInkTime*100, ActiveConfig.sections[i].SuckSpeed);
											if(CleanConfig == 3 && i == 0)
											{
												Last_Fill_Time = OSTimeGet();
											}
											if(OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
											{
												if(CleanConfig == 3 && i == 0)
												{
													Is_Break = True;
													break;
												}
											}
											else
											{
												OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
											}
											OSTimeDly(ActiveConfig.sections[i].StayTime*100);
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
											buf[6] = printer.movSpeed;
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											
											RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
											
											if(j<ActiveConfig.sections[i].LoopTimes-1)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
												buf[6] = printer.movSpeed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											}
										}
										if(Is_Break == True)
										{
											RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[0].MoveSpeed);
											Is_Break = False;
											break;
										}
									}
									if(ActiveConfig.sections[i].type == SuckAndWipe || 
									   ActiveConfig.sections[i].type == SuckWipeAndFlash)
									{
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_Start;
										buf[6] = printer.movSpeed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_End;
										buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
									}
									if(ActiveConfig.sections[i].type == SuckOnly || 
									   ActiveConfig.sections[i].type == SuckAndWipe || 
										   ActiveConfig.sections[i].type == SuckWipeAndFlash)
									{
										RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);
									}
									if(ActiveConfig.sections[i].type == FlashOnly || 
									   ActiveConfig.sections[i].type == SuckWipeAndFlash)
									{
										if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 1) != 0 )
										{
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos;
											buf[6] = printer.movSpeed;
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											
											if(ActiveConfig.sections[i].FlashTime > 0)
											{
												buf[0] = 4;
												buf[1] = UART_HEAD_EPSON_JOBSTART;
												buf[2] = 1; //flash Job
												buf[3] = 2;
												while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
													OSTimeDly(10);
												OSTimeDly(100);
												
												lefttime = ActiveConfig.sections[i].FlashTime;
												
												while(lefttime > 0)
												{
													buf[0] = 6;
													buf[1] = UART_HEAD_EPSON_PASSSTART;
													buf[2] = 0;
													buf[3] = 0;
													buf[4] = 0;
													buf[5] = 0;
													UART_SendCMD(UART_HEAD_CHANNEL, buf);
													OSTimeDly(100);
													
													if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 && 
													   ActiveFactory.Carriage_X_FlashPos == ActiveFactory.Carriage_X_FlashPos_1)
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 3);
													else
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
													OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
													FPGA_STOP_FLASH_Safe();
													
													if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
														lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
													else
														break;
													
													OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
													
													if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
														lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
													else
														break;
												}
												
												buf[0] = 2;
												buf[1] = UART_HEAD_EPSON_JOBEND;
												while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
													OSTimeDly(10);				
											}
										}
										if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 &&
										   ActiveFactory.Carriage_X_FlashPos != ActiveFactory.Carriage_X_FlashPos_1)
										{
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos_1;
											buf[6] = printer.movSpeed;
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											
											if(ActiveConfig.sections[i].FlashTime > 0)
											{
												buf[0] = 4;
												buf[1] = UART_HEAD_EPSON_JOBSTART;
												buf[2] = 1; //flash Job
												buf[3] = 2;
												while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
													OSTimeDly(10);
												OSTimeDly(100);
												
												lefttime = ActiveConfig.sections[i].FlashTime;
												
												while(lefttime > 0)
												{
													buf[0] = 6;
													buf[1] = UART_HEAD_EPSON_PASSSTART;
													buf[2] = 0;
													buf[3] = 0;
													buf[4] = 0;
													buf[5] = 0;
													UART_SendCMD(UART_HEAD_CHANNEL, buf);
													OSTimeDly(100);
													
													FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 2);
													OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
													FPGA_STOP_FLASH_Safe();
													
													if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
														lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
													else
														break;
													
													OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
													
													if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
														lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
													else
														break;
												}
												
												buf[0] = 2;
												buf[1] = UART_HEAD_EPSON_JOBEND;
												while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
													OSTimeDly(10);				
											}
										}
									}
								}
								
								status_ReportStatus(STATUS_MOVING, STATUS_SET);		
								
								buf[0] = 7; //Length
								buf[1] = UART_MOVETO_CMD;
								*((__packed INT32S *)&buf[2]) = 0;
								buf[6] = printer.movSpeed;
								while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
									OSTimeDly(100);			
								OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
								
								if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
								{
									buf[0] = 4;
									buf[1] = UART_HEAD_EPSON_JOBSTART;
									buf[2] = 0; //flash Job
									buf[3] = job_info.VSDModel;
									while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
										OSTimeDly(10);
									OSTimeDly(100);
								}
								
								status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
								status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
							}
							else 
#elif defined (EPSON_CLEAN_INTEGRATE_1)
								if (nextband_autoClean)
								{
									struct CleanParaEpsonMicolor_Factory ActiveFactory;
									struct CleanParaEpsonMicolor_Config ActiveConfig;
									struct CleanParaEpsonMicolor_Runtime ActiveRuntime;
									INT16S WiperYHideToWipeDistance_1;
									INT16S CarriageXWipePos_1_Start;
									INT16S CarriageXWipePos_1_End;		
									INT8U CleanConfig = 0;
				INT8U Is_Break = False;
									status_ReportStatus(STATUS_CLEANING, STATUS_SET);
									nextband_autoClean = False;
									
									OSSemPend(CleaningParamSem, 0, &err);
									memcpy(&ActiveFactory, &cleanparam_EPSON_MICOLOR.factory, sizeof(struct CleanParaEpsonMicolor_Factory));
									memcpy(&ActiveRuntime, &cleanparam_EPSON_MICOLOR.Runtime, sizeof(struct CleanParaEpsonMicolor_Runtime));
									WiperYHideToWipeDistance_1=cleanparam_EPSON_MICOLOR.Wiper_Y_HideToWipeDistance_1;
									CarriageXWipePos_1_Start=cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start;
									CarriageXWipePos_1_End=cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End;
									if(ActiveCleanConfig != 0xFF)
									{
										ActiveRuntime.ActivedConfig = ActiveCleanConfig;
										ActiveCleanConfig = 0xFF;
									}
									CleanConfig = ActiveRuntime.ActivedConfig;
									memcpy(&ActiveConfig, &cleanparam_EPSON_MICOLOR.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonMicolor_Config));
									OSSemPost(CleaningParamSem);
									
									if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
									{
										buf[0] = 2;
										buf[1] = UART_HEAD_EPSON_JOBEND;
										while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
											OSTimeDly(10);				
									}
									wiper_down();
									for(i=0; i<ActiveConfig.sectionCount; i++)
									{
										if(ActiveConfig.sections[i].type == PreClean)
										{
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
											buf[6] =ActiveConfig.Carriage_X_Wipe_Speed;
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											
											// RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
					#if defined(COLORFUL_EPSON)
						sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100, CleanConfig);
					#else
						sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100);
					#endif
	
											if(i == ActiveConfig.sectionCount-1 || (ActiveConfig.sections[i+1].type == FlashOnly || 
																					ActiveConfig.sections[i+1].type == None || ActiveConfig.sections[i+1].type == PreClean))
												//RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);
												wiper_down();
										}
										if(ActiveConfig.sections[i].type == SuckOnly || 
										   ActiveConfig.sections[i].type == SuckAndWipe || 
											   ActiveConfig.sections[i].type == SuckWipeAndFlash)
										{
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
											buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
											
											// if(i == 0 || ActiveConfig.sections[i-1].type != PreClean)
											//RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, 
											//   ActiveConfig.sections[i].MoveSpeed);
											
											for(j=0; j<ActiveConfig.sections[i].LoopTimes; j++)
											{
						#if 1
							if(CleanConfig == 3 && i == 0 )
							{
								if(IS_FIRST_FILL == True)
								{
									IS_FIRST_FILL = False;
									OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
								}
								else
								{
									if(Last_Fill_Time + 60*1000 <= OSTimeGet())
									{
										OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
									}
									else
									{
										if(cleanparam_EPSON_MICOLOR.Clean_Fill_Safe == True)
										{
											DO_NOT_ANSWER_KEY = True;
											Display_Fill_Inf(60*1000-(OSTimeGet() -Last_Fill_Time));
											DO_NOT_ANSWER_KEY = False;
											OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
											Is_Break = True;
											break;
										}
										OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
									}
								}
							}
						#endif
												// RotateCleanMotorByTime(True, ActiveConfig.sections[i].SuckInkTime*100, ActiveConfig.sections[i].SuckSpeed);
						#if defined(COLORFUL_EPSON)
							sucker_bytime(ActiveConfig.sections[i].SuckInkTime*100, CleanConfig);
						#else
							sucker_bytime(ActiveConfig.sections[i].SuckInkTime*100);
						#endif

						#if 1
							if(CleanConfig == 3 && i == 0)
							{
								Last_Fill_Time = OSTimeGet();
							}
							if(OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
							{
								if(CleanConfig == 3 && i == 0)
								{
									Is_Break = True;
									break;
								}
							}
							else
							{
								OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
							}
						#endif
												OSTimeDly(ActiveConfig.sections[i].StayTime*100);
												
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												//RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
							#if defined(COLORFUL_EPSON)
							sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100, CleanConfig);
							#else
								sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100);
							#endif
												
												if(j<ActiveConfig.sections[i].LoopTimes-1)
												{
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												}
											}
					#if 1
						if(Is_Break == True)
						{
							wiper_up(ActiveFactory.Wiper_Y_HideToWipeDistance);
							Is_Break = False;
							//break;
						}
					#endif
					}										
										if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 1) != 0 )
										{
											
											if(ActiveConfig.sections[i].type == SuckAndWipe || 
											   ActiveConfig.sections[i].type == SuckWipeAndFlash)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_Start;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												wiper_up(ActiveFactory.Wiper_Y_HideToWipeDistance);
												
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_End;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												wiper_down();
											}
											if(ActiveConfig.sections[i].type == FlashOnly || 
											   ActiveConfig.sections[i].type == SuckWipeAndFlash)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												if(ActiveConfig.sections[i].FlashTime > 0)
												{
													buf[0] = 4;
													buf[1] = UART_HEAD_EPSON_JOBSTART;
													buf[2] = 1; //flash Job
													buf[3] = 2;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
														OSTimeDly(10);
													OSTimeDly(100);
													
													lefttime = ActiveConfig.sections[i].FlashTime;
													
													while(lefttime > 0)
													{
														buf[0] = 6;
														buf[1] = UART_HEAD_EPSON_PASSSTART;
														buf[2] = 0;
														buf[3] = 0;
														buf[4] = 0;
														buf[5] = 0;
														UART_SendCMD(UART_HEAD_CHANNEL, buf);
														OSTimeDly(100);
														
														if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && EPSON_PERMIT_GROUPNUM >= 2 && 
														   ActiveFactory.Carriage_X_FlashPos == ActiveFactory.Carriage_X_FlashPos_1)
															FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 3);
														else
#if (defined( HEAD_2_SWAP)|| defined(COLORFUL_EPSON)|| defined(UV_PRINTER)&&!defined(EPSON_4H))
															FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 2);
#else
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
#endif
														OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
														FPGA_STOP_FLASH_Safe();
														
														if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
															lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
														else
															break;
														
														OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
														
														if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
															lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
														else
															break;
													}
													
													buf[0] = 2;
													buf[1] = UART_HEAD_EPSON_JOBEND;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
														OSTimeDly(10);				
												}
											}
										}
										
										if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && EPSON_PERMIT_GROUPNUM >= 2 )
										{                              				
											if(ActiveConfig.sections[i].type == SuckAndWipe || 
											   ActiveConfig.sections[i].type == SuckWipeAndFlash)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_Start;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												wiper_up(WiperYHideToWipeDistance_1);//WiperYHideToWipeDistance_1
												
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_End;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												wiper_down();
											}
											if((ActiveConfig.sections[i].type == FlashOnly || 
												ActiveConfig.sections[i].type == SuckWipeAndFlash)&&ActiveFactory.Carriage_X_FlashPos != ActiveFactory.Carriage_X_FlashPos_1)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos_1;
						#if defined(COLORFUL_EPSON)
							buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
						#else
							buf[6] = printer.movSpeed;
						#endif
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												if(ActiveConfig.sections[i].FlashTime > 0)
												{
													buf[0] = 4;
													buf[1] = UART_HEAD_EPSON_JOBSTART;
													buf[2] = 1; //flash Job
													buf[3] = 2;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
														OSTimeDly(10);
													OSTimeDly(100);
													
													lefttime = ActiveConfig.sections[i].FlashTime;
													
													while(lefttime > 0)
													{
														buf[0] = 6;
														buf[1] = UART_HEAD_EPSON_PASSSTART;
														buf[2] = 0;
														buf[3] = 0;
														buf[4] = 0;
														buf[5] = 0;
														UART_SendCMD(UART_HEAD_CHANNEL, buf);
														OSTimeDly(100);
														
#if (defined( HEAD_2_SWAP)|| defined(COLORFUL_EPSON) || defined(UV_PRINTER)&&!defined(EPSON_4H))
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
#else
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 2);
#endif
														
														OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
														FPGA_STOP_FLASH_Safe();
														
														if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
															lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
														else
															break;
														
														OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
														
														if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
															lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
														else
															break;
													}
													
													buf[0] = 2;
													buf[1] = UART_HEAD_EPSON_JOBEND;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
														OSTimeDly(10);				
												}
											}
										}
										
									}
									status_ReportStatus(STATUS_MOVING, STATUS_SET);		
									
									buf[0] = 7; //Length
									buf[1] = UART_MOVETO_CMD;
									*((__packed INT32S *)&buf[2]) = 0;
									buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
									while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
										OSTimeDly(100);			
									OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
									
									if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
									{
										buf[0] = 4;
										buf[1] = UART_HEAD_EPSON_JOBSTART;
										buf[2] = 0; //flash Job
										buf[3] = job_info.VSDModel;
										while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
											OSTimeDly(10);
										OSTimeDly(100);
									}
									
									status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
									status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
								}
								else 
#elif defined (EPSON_CLEAN_INTEGRATE_2)
									if (nextband_autoClean)
									{
										struct CleanParaEpsonMicolor_Factory ActiveFactory;
										struct CleanParaEpsonMicolor_Config ActiveConfig;
										struct CleanParaEpsonMicolor_Runtime ActiveRuntime;
										INT8U CleanConfig = 0;
										INT8U Is_Break = False;
										INT16S CarriageXWipePos_1_Start;
										INT16S CarriageXWipePos_1_End;								
										status_ReportStatus(STATUS_CLEANING, STATUS_SET);
										nextband_autoClean = False;
										
										OSSemPend(CleaningParamSem, 0, &err);
										memcpy(&ActiveFactory, &cleanparam_EPSON_MICOLOR.factory, sizeof(struct CleanParaEpsonMicolor_Factory));
										memcpy(&ActiveRuntime, &cleanparam_EPSON_MICOLOR.Runtime, sizeof(struct CleanParaEpsonMicolor_Runtime));
										
										CarriageXWipePos_1_Start=cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_Start;
										CarriageXWipePos_1_End=cleanparam_EPSON_MICOLOR.Carriage_X_WipePos_1_End;
										
										if(ActiveCleanConfig != 0xFF)
										{
											ActiveRuntime.ActivedConfig = ActiveCleanConfig;
											ActiveCleanConfig = 0xFF;
										}
										CleanConfig = ActiveRuntime.ActivedConfig;
										memcpy(&ActiveConfig, &cleanparam_EPSON_MICOLOR.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonMicolor_Config));
										OSSemPost(CleaningParamSem);
										
										if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
										{
											buf[0] = 2;
											buf[1] = UART_HEAD_EPSON_JOBEND;
											while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
												OSTimeDly(10);				
										}
										wiper_down(3);
										for(i=0; i<ActiveConfig.sectionCount; i++)
										{
											if(ActiveConfig.sections[i].type == PreClean)
											{
												
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
												buf[6] =ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												// RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
												sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100,0,0,0);
											}
											if(ActiveConfig.sections[i].type == SuckOnly || 
											   ActiveConfig.sections[i].type == SuckAndWipe || 
												   ActiveConfig.sections[i].type == SuckWipeAndFlash)
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
												buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												// if(i == 0 || ActiveConfig.sections[i-1].type != PreClean)
												//RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, 
												//   ActiveConfig.sections[i].MoveSpeed);
												
												for(j=0; j<ActiveConfig.sections[i].LoopTimes; j++)
												{
													// RotateCleanMotorByTime(True, ActiveConfig.sections[i].SuckInkTime*100, ActiveConfig.sections[i].SuckSpeed);
													if(CleanConfig == 3 && i == 0 )
													{
														if(IS_FIRST_FILL == True)
														{
															IS_FIRST_FILL = False;
															OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
														}
														else
														{
															if(Last_Fill_Time + 60*1000 <= OSTimeGet())
															{
																OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
															}
															else
															{
																if(cleanparam_EPSON_MICOLOR.Clean_Fill_Safe == True)
																{
																	DO_NOT_ANSWER_KEY = True;
																	Display_Fill_Inf(60*1000-(OSTimeGet() -Last_Fill_Time));
																	DO_NOT_ANSWER_KEY = False;
																	OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
																	Is_Break = True;
																	break;
																}
																OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
															}
														}
													}
													sucker_bytime(ActiveConfig.sections[i].SuckInkTime*100,ActiveConfig.sections[i].StayTime*100,ActiveConfig.sections[i].ReleaseTime*100,1);
													if(CleanConfig == 3 && i == 0)
													{
														Last_Fill_Time = OSTimeGet();
													}
													if(OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
													{
														if(CleanConfig == 3 && i == 0)
														{
															Is_Break = True;
															break;
														}
													}
													else
													{
														OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
													}
													//OSTimeDly(ActiveConfig.sections[i].StayTime*100);
													
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													
													//RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
													sucker_bytime(ActiveConfig.sections[i].ReleaseTime*100,0,0,0);
													
													if(j<ActiveConfig.sections[i].LoopTimes-1)
													{
														status_ReportStatus(STATUS_MOVING, STATUS_SET);		
														buf[0] = 7; //Length
														buf[1] = UART_MOVETO_CMD;
														*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
														buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
														while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
															OSTimeDly(100);			
														OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													}
												}
												if(Is_Break == True)
												{
													Is_Break = False;
													break;
												}
											}										
											CONTROL_CLOSE_RELEASE_RELAY(0);						 
											
											if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 1) != 0 )
											{
												if(ActiveConfig.sections[i].type == SuckAndWipe || 
												   ActiveConfig.sections[i].type == SuckWipeAndFlash)
												{
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_Start;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													
													wiper_up(1);
													
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_End;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													wiper_down(1);

													if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 && 
															   ActiveFactory.Carriage_X_FlashPos == ActiveFactory.Carriage_X_FlashPos_1)
													{
														status_ReportStatus(STATUS_MOVING, STATUS_SET);		
														
														buf[0] = 7; //Length
														buf[1] = UART_MOVETO_CMD;
														*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_Start;
														buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
														while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
															OSTimeDly(100);			
														OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
														
														wiper_up(2);
														
														status_ReportStatus(STATUS_MOVING, STATUS_SET);		
														buf[0] = 7; //Length
														buf[1] = UART_MOVETO_CMD;
														*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_End;
														buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
														while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
															OSTimeDly(100);
														OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
														wiper_down(2);
													}
												}
#ifndef EPSON_CLEAN_INTEGRATE_FLASH_UNION										
												if(ActiveConfig.sections[i].type == FlashOnly || 
												   ActiveConfig.sections[i].type == SuckWipeAndFlash)
												{
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos;
													buf[6] = printer.movSpeed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													
													if(ActiveConfig.sections[i].FlashTime > 0)
													{
														buf[0] = 4;
														buf[1] = UART_HEAD_EPSON_JOBSTART;
														buf[2] = 1; //flash Job
														buf[3] = 2;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
															OSTimeDly(10);
														OSTimeDly(100);
														
														lefttime = ActiveConfig.sections[i].FlashTime;
														
														while(lefttime > 0)
														{
															buf[0] = 6;
															buf[1] = UART_HEAD_EPSON_PASSSTART;
															buf[2] = 0;
															buf[3] = 0;
															buf[4] = 0;
															buf[5] = 0;
															UART_SendCMD(UART_HEAD_CHANNEL, buf);
															OSTimeDly(100);
															
															if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 && 
															   ActiveFactory.Carriage_X_FlashPos == ActiveFactory.Carriage_X_FlashPos_1)
																FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 3);
															else
#if (defined( HEAD_2_SWAP)|| defined(UV_PRINTER)&&!defined(EPSON_4H))
																FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 2);
#else
															FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
#endif
															OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
															FPGA_STOP_FLASH_Safe();
															
															if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
																lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
															else
																break;
															
															OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
															
															if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
																lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
															else
																break;
														}
														
														buf[0] = 2;
														buf[1] = UART_HEAD_EPSON_JOBEND;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
															OSTimeDly(10);				
													}
												}
#endif										
											}
											if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 
												&&(ActiveFactory.Carriage_X_FlashPos != ActiveFactory.Carriage_X_FlashPos_1))
											{
												if(ActiveConfig.sections[i].type == SuckAndWipe || 
												   ActiveConfig.sections[i].type == SuckWipeAndFlash)
												{
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_Start;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													
													wiper_up(2);
													
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = CarriageXWipePos_1_End;
													buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													wiper_down(2);
												}
#ifndef EPSON_CLEAN_INTEGRATE_FLASH_UNION										
												if((ActiveConfig.sections[i].type == FlashOnly || 
													ActiveConfig.sections[i].type == SuckWipeAndFlash))
												{
													status_ReportStatus(STATUS_MOVING, STATUS_SET);		
													
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos_1;
													buf[6] = printer.movSpeed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100);			
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													
													if(ActiveConfig.sections[i].FlashTime > 0)
													{
														buf[0] = 4;
														buf[1] = UART_HEAD_EPSON_JOBSTART;
														buf[2] = 1; //flash Job
														buf[3] = 2;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
															OSTimeDly(10);
														OSTimeDly(100);
														
														lefttime = ActiveConfig.sections[i].FlashTime;
														
														while(lefttime > 0)
														{
															buf[0] = 6;
															buf[1] = UART_HEAD_EPSON_PASSSTART;
															buf[2] = 0;
															buf[3] = 0;
															buf[4] = 0;
															buf[5] = 0;
															UART_SendCMD(UART_HEAD_CHANNEL, buf);
															OSTimeDly(100);
															
#if (defined( HEAD_2_SWAP)|| defined(UV_PRINTER)&&!defined(EPSON_4H))
															FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
#else
															FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 2);
#endif
															
															OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
															FPGA_STOP_FLASH_Safe();
															
															if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
																lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
															else
																break;
															
															OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
															
															if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
																lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
															else
																break;
														}
														
														buf[0] = 2;
														buf[1] = UART_HEAD_EPSON_JOBEND;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
															OSTimeDly(10);				
													}
												}
#endif										
											}
#ifdef EPSON_CLEAN_INTEGRATE_FLASH_UNION
											if((ActiveConfig.sections[i].type == FlashOnly || 
												ActiveConfig.sections[i].type == SuckWipeAndFlash))
											{
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos;
												buf[6] = printer.movSpeed;
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												if(ActiveConfig.sections[i].FlashTime > 0)
												{
													buf[0] = 4;
													buf[1] = UART_HEAD_EPSON_JOBSTART;
													buf[2] = 1; //flash Job
													buf[3] = 2;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
														OSTimeDly(10);
													OSTimeDly(100);
													
													lefttime = ActiveConfig.sections[i].FlashTime;
													
													while(lefttime > 0)
													{
														buf[0] = 6;
														buf[1] = UART_HEAD_EPSON_PASSSTART;
														buf[2] = 0;
														buf[3] = 0;
														buf[4] = 0;
														buf[5] = 0;
														UART_SendCMD(UART_HEAD_CHANNEL, buf);
														OSTimeDly(100);
														
														FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 3);
														
														
														OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
														FPGA_STOP_FLASH_Safe();
														
														if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
															lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
														else
															break;
														
														OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
														
														if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
															lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
														else
															break;
													}
													
													buf[0] = 2;
													buf[1] = UART_HEAD_EPSON_JOBEND;
													while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
														OSTimeDly(10);				
												}
											}
#endif
										}
										
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
										buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										sucker_bytime(ActiveConfig.sections[1].ReleaseTime*100,0,0,0);
										
										status_ReportStatus(STATUS_MOVING, STATUS_SET);		
										
										buf[0] = 7; //Length
										buf[1] = UART_MOVETO_CMD;
										*((__packed INT32S *)&buf[2]) = 0;
										buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
										while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
											OSTimeDly(100);			
										OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
										
										CONTROL_CLOSE_RELEASE_RELAY(0);		
										
										if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
										{
											buf[0] = 4;
											buf[1] = UART_HEAD_EPSON_JOBSTART;
											buf[2] = 0; //flash Job
											buf[3] = job_info.VSDModel;
											while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
												OSTimeDly(10);
											OSTimeDly(100);
										}
										status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
										status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
									}
									else 
	#elif defined (EPSON_CLEAN_INTEGRATE_3)
			if (nextband_autoClean)
			{
				#define ERROR_POSITION (10)
				#define X_SAFE_POSITION (4000)
				#define Y_COMEBACK_SPEED (0)
				struct CleanParaEpsonMicolor_Factory ActiveFactory;
				struct CleanParaEpsonMicolor_Config ActiveConfig;
				struct CleanParaEpsonMicolor_Runtime ActiveRuntime;
				INT8U CleanConfig = 0;
				INT8U Is_Break = False;
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);
				nextband_autoClean = False;
				
				OSSemPend(CleaningParamSem, 0, &err);
				memcpy(&ActiveFactory, &cleanparam_EPSON_MICOLOR.factory, sizeof(struct CleanParaEpsonMicolor_Factory));
				memcpy(&ActiveRuntime, &cleanparam_EPSON_MICOLOR.Runtime, sizeof(struct CleanParaEpsonMicolor_Runtime));
				if(ActiveCleanConfig != 0xFF)
				{
					ActiveRuntime.ActivedConfig = ActiveCleanConfig;
					ActiveCleanConfig = 0xFF;
				}
				CleanConfig = ActiveRuntime.ActivedConfig;
				memcpy(&ActiveConfig, &cleanparam_EPSON_MICOLOR.Config[ActiveRuntime.ActivedConfig], sizeof(struct CleanParaEpsonMicolor_Config));
				OSSemPost(CleaningParamSem);
				
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 2;
					buf[1] = UART_HEAD_EPSON_JOBEND;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
						OSTimeDly(10);				
				}
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);	
				buf[0] = 8; //Length            
				buf[1] = UART_MOVEDISTANCE_CMD; 
				buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = ActiveConfig.sections[0].MoveSpeed;
				*((__packed INT32S *)&buf[4]) = ActiveFactory.Wiper_Y_SuckToHideDistance; //move_distance means target position        
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);					       
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop

				status_ReportStatus(STATUS_MOVING, STATUS_SET);	
				buf[0] = 7; //Length                         
				buf[1] = UART_MOVETO_CMD;
				*((__packed INT32S *)&buf[2]) = X_SAFE_POSITION;
				buf[6] = printer.movSpeed;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				
				//if (curPos.y <= ERROR_POSITION)    			// y axle come back origin
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	
					buf[0] = 8; //Length  
					buf[1] = UART_AXISMOVETO_CMD; 
					buf[2] = 2; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
					buf[3] = Y_COMEBACK_SPEED;
					*((__packed INT32S *)&buf[4]) = 0;  //9152; //move_distance means target position        
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);					       
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				}
				
				for(i=0; i<ActiveConfig.sectionCount; i++)
				{
					if(ActiveConfig.sections[i].type == PreClean)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
						buf[6] = printer.movSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						
						RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, ActiveConfig.sections[i].MoveSpeed);
						RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
						RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);
					}
					if(ActiveConfig.sections[i].type == SuckOnly || 
					   	ActiveConfig.sections[i].type == SuckAndWipe || 
						   ActiveConfig.sections[i].type == SuckWipeAndFlash)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_SuckPos;
						buf[6] = printer.movSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						
						if(i == 0 || ActiveConfig.sections[i-1].type == PreClean)
							RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, 
											 ActiveConfig.sections[i].MoveSpeed);
						
						for(j=0; j<ActiveConfig.sections[i].LoopTimes; j++)
						{						
							RotateCleanMotorByTime(True, ActiveConfig.sections[i].SuckInkTime*100, ActiveConfig.sections[i].SuckSpeed);
							
							if(j == ActiveConfig.sections[i].LoopTimes-1)
							{
								RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);
							}
						}
						
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_ReleasePos;
						buf[6] = printer.movSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						RotateCleanMotorByTime(True, ActiveConfig.sections[i].ReleaseTime*100, ActiveConfig.sections[i].SuckSpeed);
						RotateCleanMotor(False, ActiveFactory.Wiper_Y_SuckToHideDistance, ActiveConfig.sections[i].MoveSpeed);

					}
					if(ActiveConfig.sections[i].type == SuckAndWipe || 
					  	 ActiveConfig.sections[i].type == SuckWipeAndFlash)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_Start;
						buf[6] = printer.movSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

						RotateCleanMotor(False, ActiveFactory.Wiper_Y_HideToWipeDistance, ActiveConfig.sections[i].MoveSpeed);
						
						status_ReportStatus(STATUS_MOVING, STATUS_SET);		
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_WipePos_End;
						buf[6] = ActiveConfig.Carriage_X_Wipe_Speed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
					}
					if(ActiveConfig.sections[i].type == SuckOnly || 
					   ActiveConfig.sections[i].type == SuckAndWipe || 
						   ActiveConfig.sections[i].type == SuckWipeAndFlash)
					{
						RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance, ActiveConfig.sections[i].MoveSpeed);
					}
					if(ActiveConfig.sections[i].type == FlashOnly || 
					   ActiveConfig.sections[i].type == SuckWipeAndFlash)
					{
						if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 1) != 0 )
						{
							
							status_ReportStatus(STATUS_MOVING, STATUS_SET);		
							
							buf[0] = 7; //Length
							buf[1] = UART_MOVETO_CMD;
							*((__packed INT32S *)&buf[2]) = ActiveFactory.Carriage_X_FlashPos;
							buf[6] = printer.movSpeed;
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(100);			
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
							
							if(ActiveConfig.sections[i].FlashTime > 0)
							{
								buf[0] = 4;
								buf[1] = UART_HEAD_EPSON_JOBSTART;
								buf[2] = 1; //flash Job
								buf[3] = 2;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
									OSTimeDly(10);
								OSTimeDly(100);
								
								lefttime = ActiveConfig.sections[i].FlashTime;
								
								while(lefttime > 0)
								{
									buf[0] = 6;
									buf[1] = UART_HEAD_EPSON_PASSSTART;
									buf[2] = 0;
									buf[3] = 0;
									buf[4] = 0;
									buf[5] = 0;
									UART_SendCMD(UART_HEAD_CHANNEL, buf);
									OSTimeDly(100);
									
									if((cleanparam_EPSON_MICOLOR.Runtime.HeadMask & 2) != 0  && GetCleanPartCount() >= 2 && 
									   ActiveFactory.Carriage_X_FlashPos == ActiveFactory.Carriage_X_FlashPos_1)
										FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 3);
									else
										FPGA_START_FLASH(ActiveConfig.sections[i].FlashFreqInterval, 1);
									OSTimeDly((ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle)*100);
									FPGA_STOP_FLASH_Safe();
									
									if(lefttime > (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle))
										lefttime -= (ActiveConfig.sections[i].FlashCycle - ActiveConfig.sections[i].FlashIdleInCycle);
									else
										break;
									
									OSTimeDly(ActiveConfig.sections[i].FlashIdleInCycle*100);
									
									if(lefttime > ActiveConfig.sections[i].FlashIdleInCycle)
										lefttime -= ActiveConfig.sections[i].FlashIdleInCycle;
									else
										break;
								}
								
								buf[0] = 2;
								buf[1] = UART_HEAD_EPSON_JOBEND;
								while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
									OSTimeDly(10);				
							}
						}
						
					}
				}
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				
				buf[0] = 7; //Length
				buf[1] = UART_MOVETO_CMD;
				*((__packed INT32S *)&buf[2]) = 0;
				buf[6] = printer.movSpeed;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

				RotateCleanMotor(True, ActiveFactory.Wiper_Y_HideToWipeDistance+ActiveFactory.Wiper_Y_WipeToSuckDistance, ActiveConfig.sections[0].MoveSpeed);
				status_ReportStatus(STATUS_CAPPED, STATUS_SET);
				
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 4;
					buf[1] = UART_HEAD_EPSON_JOBSTART;
					buf[2] = 0; //flash Job
					buf[3] = job_info.VSDModel;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
						OSTimeDly(10);
					OSTimeDly(100);
				}
				
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
				status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
			}
			else 
#endif  
#if defined (EPSON_CLEAN_UPDOWN)
										if (nextband_Clean_InkBox)
										{
											struct CleanParaEpsonAllwin_Factory ActiveFactory;
											struct CleanParaEpsonAllwin_Config ActiveConfig;
											INT8U bAutoCleanInPrint = False;
											
											status_ReportStatus(STATUS_CLEANING, STATUS_SET);
											nextband_Clean_InkBox = False;
											
											OSSemPend(CleaningParamSem, 0, &err);
											memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
											if( ActiveCleanConfig != 3)
											{
												ActiveCleanConfig = 3;
											}
											memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[ActiveCleanConfig], sizeof(struct CleanParaEpsonAllwin_Config));
											
											OSSemPost(CleaningParamSem);
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											buf[0] = 8; //Length						
											buf[1] = UART_AXISMOVETO_CMD; 
											buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
											buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
											*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
											
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
											
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = (float)(100/2.54)*X_BASE_RES;
											buf[6] = PRINTER_MOVE_SLOWEST;
											
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop                
											
											for(i =0; i<GetCleanPartCount(); i++)
											{
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
												CONTROL_CLOSE_RELEASE_RELAY(i);
#endif
												CONTROL_OPEN_SUCKER(i);
											}
											OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
											
											lefttime = ActiveConfig.SuckInkTime;
											INT8U bBreak = FALSE;
											while(lefttime > 650)
											{
												flags = OSFlagPend(mix_FLAG_GRP,CLEAN_FILL_STEP1_STATE,OS_FLAG_WAIT_CLR_ANY|OS_FLAG_CONSUME,650*100,&err);
												if(flags == 0 && err ==  OS_TIMEOUT)
													lefttime -= 650;
												else
												{
													bBreak = TRUE;
													break;  
												}
											}
											if(!bBreak)
												OSTimeDly(lefttime*100);			
											//OSTimeDly(cleanPara.pauseIntervalAfterClean);
											
											for(i =0; i<GetCleanPartCount(); i++)
											{
												CONTROL_CLOSE_SUCKER(i);
#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
												CONTROL_OPEN_RELEASE_RELAY(i);
#endif                    	                
											}
											status_ReportStatus(STATUS_MOVING, STATUS_SET);		
											
											buf[0] = 7; //Length
											buf[1] = UART_MOVETO_CMD;
											*((__packed INT32S *)&buf[2]) = 0;
											buf[6] = PRINTER_MOVE_SLOWEST;
											
											while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
												OSTimeDly(100);			
											OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop                
											
											if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
											{
												//when printing and not pause, it means auto cleaning
												if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
													bAutoCleanInPrint = True;
											}
											if(!bAutoCleanInPrint)
											{ //for speed up. because capping is too slow.
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 8; //Length						
												buf[1] = UART_AXISMOVETO_CMD; 
												buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
												buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
												*((__packed INT32S *)&buf[4]) = ActiveFactory.HeadBox_Z_SuckPos; //move_distance means target position
												
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
												
												status_ReportStatus(STATUS_CAPPED, STATUS_SET);
											}
											
											OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
											if (err == OS_NO_ERR)
											{								
												OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err); //20120914
												if (err == OS_NO_ERR)
												{	
													status_ReportStatus(STATUS_PAUSE, STATUS_SET);
												}
											}
											
											status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
											status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
										}
										else 
#endif
#ifdef DYSS_UV_CLEAN
											if (Power_Off_Mode)
											{
												Power_Off_Mode  = False;
												DYSS_CLEAN_TURN_MOTOR_SENSOR4();
												
												if(PIO_Get(&DYSS_CLEAN_SENSOR1)!= DYSS_CLEAN_SENSOR1_ON)
												{			
													status_ReportStatus(STATUS_MOVING, STATUS_SET); 	
													buf[0] = 7; //Length
													buf[1] = UART_MOVETO_CMD;
													*((__packed INT32S *)&buf[2]) = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;
													buf[6] = printer.movSpeed;
													while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
														OSTimeDly(100); 		
													OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
													DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR1();
												}
												status_ReportStatus(STATUS_MOVING, STATUS_SET);		
												buf[0] = 7; //Length
												buf[1] = UART_MOVETO_CMD;
												*((__packed INT32S *)&buf[2]) = cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
												buf[6] = printer.movSpeed;
												
												while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
													OSTimeDly(100);			
												OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
												
												DYSS_CLEAN_MOVEING_MOTOR_GO_SENSOR2();
												DYSS_CLEAN_TURN_MOTOR_SENSOR5();
												status_ReportStatus(STATUS_SVC_POWER_OFF_MODE, STATUS_SET);
											}
											else 
#endif
												if (nextband_autoSpray)
												{
													nextband_autoSpray = False;
													flags = OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ANY,&err);
													if(flags == 0 && err != 0)
													{    
														status_ReportStatus(STATUS_CLEANING, STATUS_SET);
														
														//                    status_ReportStatus(STATUS_MOVING, STATUS_SET);	
														//                    buf[0] = 7;
														//                    buf[1] = UART_MOVETO_CMD;
														//                    buf[2] = 0;
														//                    buf[3] = 0;
														//                    buf[4] = 0;
														//                    buf[5] = 0;
														//                    buf[6] = printer.movSpeed;
														//
														//                    while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Go home
														//                        OSTimeDly(100);			
														//                    OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
														
														buf[0] = 4;
														buf[1] = UART_HEAD_EPSON_JOBSTART;
														buf[2] = 1; //flash Job
														buf[3] = 2;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
															OSTimeDly(10);
														OSTimeDly(100);
														
														buf[0] = 6;
														buf[1] = UART_HEAD_EPSON_PASSSTART;
														buf[2] = 0;
														buf[3] = 0;
														buf[4] = 0;
														buf[5] = 0;
														UART_SendCMD(UART_HEAD_CHANNEL, buf);
														OSTimeDly(100);
														
														//临时借用clean参数，flash_interval：点火周期-75 us.
														//pauseIntervalAfterClean : 闪喷时间（分钟）。
														FPGA_START_FLASH(75 + cleanPara.flash_interval, 0x3);
														OSTimeDly(cleanPara.pauseIntervalAfterClean);
														FPGA_STOP_FLASH_Safe();
														
														buf[0] = 2;
														buf[1] = UART_HEAD_EPSON_JOBEND;
														while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
															OSTimeDly(10);
														
														OSTimeDly(1);
														status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);
														status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
													}
													else
														status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
												}
												else
												{
													status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
												}
		}
		else if (flags & CMD_CLEAN_MANUAL) //manual clean
		{
			//do nothing
			status_ReportStatus(CMD_CLEAN_MANUAL, STATUS_CLR);			
		}
		else if (flags & CMD_CLEAN_SINGLE)
		{
			//do nothing
			status_ReportStatus(CMD_CLEAN_SINGLE, STATUS_CLR);			
		}
#if defined (HEAD_RICOH_G4)
		if ((flags & CMD_EPSON_OPERATION) && 
			(CMD_Epson_Operate_Type == CMD_RICOH_T_START_IDEL_SPRAY || 
			 CMD_Epson_Operate_Type == CMD_RICOH_T_STOP_IDEL_SPRAY))
		{
			if(CMD_Epson_Operate_Type == CMD_RICOH_T_START_IDEL_SPRAY)
			{
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 4;
					buf[1] = UART_HEAD_EPSON_JOBSTART;
					buf[2] = 1; //flash Job
					buf[3] = 2;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf))
						OSTimeDly(10);
					OSTimeDly(100);
				}
				
				FPGA_START_FLASH_RICOH(0xFF);
			}
			else
			{
				FPGA_STOP_FLASH();
				
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
				{
					buf[0] = 2;
					buf[1] = UART_HEAD_EPSON_JOBEND;
					while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
						OSTimeDly(10);									
					OSTimeDly(100);
				}
				
				printerFlashing = False;
			}
			CMD_Epson_Operate_Type = 0;
			status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
		}
#endif
		if (flags & CMD_EPSON_OPERATION)
		{
			if(CMD_Epson_Operate_Type == CMD_EPSON_T_MACHIME_TEST)
			{
				status_ReportStatus(CMD_PAUSE, STATUS_CLR);
				status_ReportStatus(STATUS_PAUSE, STATUS_CLR);	
				status_ReportStatus(STATUS_PRINT, STATUS_SET);	
				
				//OSTimeDly(2000);
				//Send UART CMD to MOTION to inform encoder_div, speed 				
				buf[0] = 10; //Command length
#ifdef SUPPORT_MOTOR_CONTROL
				buf[0] += 1;
#endif
				buf[1] = UART_PRINTPARA_CMD;
				*((__packed INT32U *)&buf[2]) = 2;
				
				*((__packed INT32U *)&buf[6]) = 1; //for vsd1
				
#ifdef SUPPORT_MOTOR_CONTROL
				*((__packed INT32U *)&buf[6]) = 1; //for vsd1
				buf[10] = 0; //for 720dpi
#endif
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
					OSTimeDly(10);
				
				int passnum =0;
				
				while(1)
				{
#ifdef SUPPORT_MOTOR_CONTROL
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	 //
					status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR); //Just for safety
					
					buf[0] = UART_PRE_PRINT_CMD_LEN;
					buf[1] = UART_PRE_PRINT_CMD;
					
					{
						//The start point of the print area
						*((__packed INT32U *)&buf[2]) = printer.platSpace;
						//The end point of the print area
						*((__packed INT32U *)&buf[6]) = printer.platSpace + PAPER_MEDIA_WIDTH_INCH * (factoryDataEx.m_nXEncoderDPI)/2;
						
						//The points of the movement
						//if(passnum %2 == 0)
						{
							*((__packed INT32U *)&buf[10]) = printer.platSpace;
							*((__packed INT32U *)&buf[14]) = printer.platSpace + PAPER_MEDIA_WIDTH_INCH * (factoryDataEx.m_nXEncoderDPI)/2;
						}
					}
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
						OSTimeDly(20);
					if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,30000,&err)) //20090214
							status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
#endif					
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	 //
					status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR); //Just for safety
					
					buf[0] = UART_PRINT_CMD_LEN;
					buf[1] = UART_PRINT_CMD;
					
					{
						//The start point of the print area
						*((__packed INT32U *)&buf[2]) = printer.platSpace;
						//The end point of the print area
						*((__packed INT32U *)&buf[6]) = printer.platSpace + PAPER_MEDIA_WIDTH_INCH * (factoryDataEx.m_nXEncoderDPI)/2;
						
						//The points of the movement
						if(passnum %2 == 0)
						{
							*((__packed INT32U *)&buf[10]) = printer.platSpace - printer.accSpace;
							*((__packed INT32U *)&buf[14]) = printer.platSpace + PAPER_MEDIA_WIDTH_INCH * (factoryDataEx.m_nXEncoderDPI) + printer.accSpace;
						}
						else
						{
							*((__packed INT32U *)&buf[10]) = printer.platSpace + PAPER_MEDIA_WIDTH_INCH * (factoryDataEx.m_nXEncoderDPI) + printer.accSpace;
							*((__packed INT32U *)&buf[14]) = printer.platSpace - printer.accSpace;
						}
						*((__packed INT32U *)&buf[18]) = epsonCaliSetting.eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep / 4;
					}
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
						OSTimeDly(20);
						if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,30000,&err)) //20090214
							status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
						
						if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CANCEL , OS_FLAG_WAIT_CLR_ANY,&err))
						{
							status_ReportStatus(STATUS_NO_CANCEL, STATUS_SET);
							status_ReportStatus(STATUS_PRINT, STATUS_CLR); //Also need clear some error.			
							CMD_Epson_Operate_Type = 0;
							status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
	#if defined(EPSON_CLEAN_UPDOWN)				
							RegPostMoveOfUncapping(2, 0);
							RegPostCapping();
							StartUnCapping();
    #elif defined(EPSON_CLEAN_INTEGRATE_3_CAP)
                            RegPostMoveOfUncapping(2, 0);
							RegPostCapping();
							StartUnCapping(True);
#endif						
							break;
						}
						passnum++;
				}
			}
	#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
			else if(CMD_Epson_Operate_Type == CMD_EPSON_T_SHEET_CUT)
			{
				Cut_Paper();
			}
	#endif
		}
#if defined (MICOLOR_AUTOFUCTION)
		if (flags & CMD_EPSON_OPERATION)
		{
			if(CMD_Epson_Operate_Type == CMD_EPSON_T_SHEET_CUT)
			{
#ifdef SUPPORT_MOTOR_CONTROL                
				INT32S cut_startPos = GetCutStartPos();
				INT32S cut_endPos = GetCutEndPos();
#else
				
#ifdef DGEN_AUTOFUCTION
                const INT32S cut_startPos = 11300 + PAPER_MEDIA_WIDTH_INCH*X_BASE_RES;
                const INT32S cut_endPos = 100;
#else
				const INT16U cut_startPos = 38350;
				const INT16U cut_endPos = 130;
#endif
				
#endif   
				
#ifdef EPSON_CLEAN_UPDOWN
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
				buf[0] = 8; //Length
				buf[1] = UART_AXISMOVETO_CMD; 
				buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;					
				*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);					       
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 7; //Length
				buf[1] = UART_MOVETO_CMD;
				*((__packed INT32S *)&buf[2]) = cut_startPos;
				buf[6] = printer.movSpeed;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				
				//                buf[0] = 3; //Length
				//                buf[1] = UART_HEAD_EPSON_SHEETCUT;
				//                buf[2] = True;
				//                while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
				//                    OSTimeDly(100);			
				//                OSTimeDly(100);			
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				buf[0] = 7; //Length
				buf[1] = UART_MOVETO_CMD;
				*((__packed INT32S *)&buf[2]) = cut_endPos;
				buf[6] = printer.movSpeed;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				
				//                buf[0] = 3; //Length
				//                buf[1] = UART_HEAD_EPSON_SHEETCUT;
				//                buf[2] = False;
				//                while (!UART_SendCMD(UART_HEAD_CHANNEL, buf)) 
				//                    OSTimeDly(100);
				//                OSTimeDly(100);
				
				status_ReportStatus(STATUS_MOVING, STATUS_SET);
				buf[0] = 7; //Length
				buf[1] = UART_MOVETO_CMD;
				*((__packed INT32S *)&buf[2]) = 0;
				buf[6] = printer.movSpeed;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
					OSTimeDly(100);
				
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
#ifdef EPSON_CLEAN_UPDOWN
				Capping();
#endif

#ifndef DGEN_AUTOFUCTION			
                status_ReportStatus(STATUS_MOVING, STATUS_SET);
                buf[0] = 8; //Length									
                buf[1] = UART_PAPERFEED_CMD; 
                buf[2] = 1;
                buf[3] = printer.feedSpeed;
                *((__packed INT32S *)&buf[4]) = GetStepPerHead()*4;
                while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
                    OSTimeDly(100);
                OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
#endif
                CMD_Epson_Operate_Type = 0;
				status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
			}
			else if(CMD_Epson_Operate_Type == CMD_EPSON_T_LOAD_MEDIA)
			{
				if(!IsLoadMediaSensorValid())
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					
					buf[0] = 8; //Length
					buf[1] = UART_PAPERFEED_CMD; 
					buf[2] = 1;
					buf[3] = printer.feedSpeed;
					*((__packed INT32S *)&buf[4]) = 0;
					status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(10);	
					
					while(!IsLoadMediaSensorValid())
						OSTimeDly(20);	
					
					buf[0] = 2;
					buf[1] = UART_STOPMOVE_CMD;
					while(!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
						OSTimeDly(10);	
					
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				}
				CMD_Epson_Operate_Type = 0;
				status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
			}
#ifdef DGEN_AUTOFUCTION			
		else if(CMD_Epson_Operate_Type == CMD_EPSON_T_DGEN_MOVE_PAPER)
            {
		INT16U i = 0,k = ((float)300.0/(2.54*EPSON_ACTIVE_GROUPNUM))*4 + 0.5;
		OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_SET, &err);
		for(i = 0;i < k;i++)
		{
			status_ReportStatus(STATUS_MOVING, STATUS_SET);
			buf[0] = 9; //Length
			buf[1] = UART_DSP_MOVEDISTANCE_PERCENT; 
			buf[2] = 3; //1,2 X, 3,4 Y, 5,6 Z
			buf[3] = 2; 
			*((__packed INT32S *)&buf[4]) = GetStepPerHead()*EPSON_ACTIVE_GROUPNUM/4;
			buf[8] = LCDMenuConfig_EX.Move_Paper_Speed_Percent;
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			if(OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
				break;
		}
		CMD_Epson_Operate_Type = 0;
		status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
		OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
            }
#endif		
		}
#endif
		else 
#else	
			if (flags & CMD_CLEAN_MANUAL) //manual clean
			{
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);			
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				pos = printer.manualCleanPos;
				buf[0] = 7; //Length
				buf[1] = UART_MOVETO_CMD;
				buf[2] = (INT8U)(pos&0xFF);
				buf[3] = (INT8U)((pos>>8)&0xFF);
				buf[4] = (INT8U)((pos>>16)&0xFF);
				buf[5] = (INT8U)((pos>>24)&0xFF);
				buf[6] = printer.movSpeed;
				
				while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
					OSTimeDly(100);
				
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop			
				OSFlagPend(status_FLAG_GRP, CMD_CLEAN_MANUAL, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting Manual clean clear
				if (!status_IsPrinting())
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	
					buf[0] = 7; //Length
					buf[1] = UART_MOVETO_CMD;
					buf[2] = 0;
					buf[3] = 0;
					buf[4] = 0;
					buf[5] = 0;
					buf[6] = printer.movSpeed;
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Go home
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
				}
			}
			else if (flags & CMD_CLEAN_AUTO)
			{
				//nextband_autoSpray
				
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);
				
				if (nextband_autoClean)
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					buf[0] = 7; //Length
					buf[1] = UART_MOVETO_CMD;
					*((__packed INT32S *)&buf[2]) = printer.autoCleanStartPos;
					buf[6] = printer.movSpeed;
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					CONTROL_OPEN_CLEANER();
					
					buf[6] = printer.cleanSpeed;
					
					for (i = 0; i < cleanPara.autoCleanTimes;i++)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);					
						*((__packed INT32S *)&buf[2]) = printer.autoCleanEndPos;				
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);						
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
						
						status_ReportStatus(STATUS_MOVING, STATUS_SET);					
						*((__packed INT32S *)&buf[2]) = printer.autoCleanStartPos;								
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
							OSTimeDly(100);			
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					}
					
				}
				
				if (nextband_autoSpray)
				{
					nextband_autoSpray = False;
					
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	
					buf[0] = 7;
					buf[1] = UART_MOVETO_CMD;
					buf[2] = 0;
					buf[3] = 0;
					buf[4] = 0;
					buf[5] = 0;
					buf[6] = printer.movSpeed;
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Go home
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
					
					if (nextband_autoClean)
					{
						nextband_autoClean = False;
						CONTROL_CLOSE_CLEANER();
					}
					flash_interval_value = cleanPara.flash_interval; //Backup the para
					flash_onoff = cleanPara.flash;					 //Backup the para
					cleanPara.flash_interval = 1;
					cleanPara.flash = True;
					cleanParaDirty = True;
					
					if (cleanPara.paraFlag != 0)
						OSTimeDly(cleanPara.pauseIntervalAfterBlowInk);
					else
						OSTimeDly(1500);
					cleanPara.flash_interval = flash_interval_value;
					cleanPara.flash = flash_onoff;
					cleanParaDirty = True;
					OSTimeDly(1);
					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
					status_ReportStatus(STATUS_CLEANING, STATUS_CLR);				
				}else if (nextband_autoClean)
				{
					//AutoClean here; //关吸尘器时Delay 0.5S
					//Clean number 必须为偶数,不为偶数,也设为偶数 ????
					//Open cleaner
					nextband_autoClean = False;
					
					//				if (!status_IsPrinting())
					//				{			
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	
					*((__packed INT32S *)&buf[2]) = 0;	
					buf[6] = printer.movSpeed;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Go home
						OSTimeDly(100);	
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop			
					CONTROL_CLOSE_CLEANER();
					if (cleanPara.paraFlag != 0)
						OSTimeDly(cleanPara.pauseIntervalAfterClean);
					else					
						OSTimeDly(1500); 
					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
					status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
					
					//				}else
					//				{
					//					status_ReportStatus(CMD_CLEAN_AUTO, STATUS_CLR);			
					//					status_ReportStatus(STATUS_CLEANING, STATUS_CLR); //先让小车移动0.5秒,然后再关闭吸尘器
					//					OSTimeDly(1000); 
					//					CONTROL_CLOSE_CLEANER();
					//				}
				}else
					status_ReportStatus(STATUS_ERR_INTERNAL_WRONGAUTOCLEAN, STATUS_SET);
			}
			else if (flags & CMD_CLEAN_SINGLE)
			{
				status_ReportStatus(STATUS_CLEANING, STATUS_SET);		
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		
				
				while (!OSFlagAccept(status_FLAG_GRP, CMD_CLEAN_SINGLE, OS_FLAG_WAIT_CLR_ALL,&err))
				{
					//				CONSOL_Printf("clean_single_indexgl = %d, single_index = %d", clean_single_indexgl,single_index);
					if (clean_single_indexgl != single_index)
					{
						status_ReportStatus(STATUS_MOVING, STATUS_SET);						
						single_index = clean_single_indexgl;
						
						if (single_index == -2)
							pos = printer.manualCleanPos;
						else if (single_index == -1)
							pos = printer.manualCleanPos;
						else
							pos = printer.headsCleanPos[single_index];					
						
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						*((__packed INT32S *)&buf[2]) = pos;
						
						if ((single_index == -2)||(single_index == -1))	
							buf[6] = printer.movSpeed;
						else
							buf[6] = printer.cleanSpeed;
						while (!UART_SendCMD(UART_MOTION_CHANNEL, buf))
							OSTimeDly(10);
						OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop
						if ((single_index != -1)&&(single_index != -2))
							CONTROL_OPEN_CLEANER();
						else	
							CONTROL_CLOSE_CLEANER();
					}else
						OSTimeDly(100);
				}
				
				if (!status_IsPrinting())
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	
					buf[0] = 7; //Length
					buf[1] = UART_MOVETO_CMD;
					buf[2] = 0;
					buf[3] = 0;
					buf[4] = 0;
					buf[5] = 0;
					buf[6] = printer.movSpeed;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Go home
						OSTimeDly(100);					
				}
				
				status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
				OSTimeDly(500);
				CONTROL_CLOSE_CLEANER();
		}
		else 
#endif	
				if (flags & CMD_START_MOVE)
				{
#if defined (MICOLOR_AUTOFUCTION)
					if (move_type == 4 && LCDMenuConfig.AutoLoad) //before measure media, load media
					{
						if(!IsLoadMediaSensorValid())
						{
							status_ReportStatus(STATUS_MOVING, STATUS_SET);	
							buf[0] = 8; //Length
							buf[1] = UART_PAPERFEED_CMD; 
							buf[2] = 1;
							buf[3] = printer.feedSpeed;
							*((__packed INT32S *)&buf[4]) = 0;
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
							
							while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
								OSTimeDly(10);	
							
							while(!IsLoadMediaSensorValid())
								OSTimeDly(20);	
							
							buf[0] = 2;
							buf[1] = UART_STOPMOVE_CMD;
							while(!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
								OSTimeDly(10);	
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
						}
					}
					if (move_type == 4 && LCDMenuConfig.AutoCoverCheck && !IsCoverSensorValid())
					{
						continue;
					}
#endif
					
				NewCMD:
					status_ReportStatus(STATUS_MOVING, STATUS_SET);
					
					if (move_type == 0) //Old command
					{			
						if (move_dir == 1) //1 远离原点, 2 接近原点方向, 3 进纸  4 退纸
						{
							buf[0] = 7; //Length						
							buf[1] = UART_MOVETO_CMD;
							if (move_distance == 0)
								*((__packed INT32S *)&buf[2]) = printer.org_oppLimit;
							else
								*((__packed INT32S *)&buf[2]) = curPos.x + move_distance;
#if (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV)||defined(ALLWIN_EPSON_SAME))&&defined(EPSON_BOTTOM_BOARD_V2)&&!defined(RIPSTAR_FLAT_EX)
							buf[6] = 4;
#else
							buf[6] = printer.movSpeed; 
#endif     
						}
						else if (move_dir == 2)
						{
							buf[0] = 7; //Length									
							buf[1] = UART_MOVETO_CMD;
							if (move_distance == 0)
								//*((__packed INT32S *)&buf[2]) = printer.orgLimit;
							{
								*((__packed INT32S *)&buf[2]) = 0;
#if defined(EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)	
								Backhaul_Flag = True;         //open the Backhaul Flag
#endif                                
							}
							else
								*((__packed INT32S *)&buf[2]) = curPos.x - move_distance;
#if (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV)||defined(ALLWIN_EPSON_SAME))&&defined(EPSON_BOTTOM_BOARD_V2)&&!defined(RIPSTAR_FLAT_EX)
							buf[6] = 4;
#else
							buf[6] = printer.movSpeed; 
#endif     
						}	
						else if (move_dir == 3) //进纸 
						{
							buf[0] = 8; //Length									
							buf[1] = UART_PAPERFEED_CMD; 
							buf[2] = 1;
							buf[3] = printer.feedSpeed;
							*((__packed INT32S *)&buf[4]) = move_distance;
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);				
				}
				else if (move_dir == 4) // 退纸
						{
							buf[0] = 8; //Length									
							buf[1] = UART_PAPERFEED_CMD; 
							buf[2] = 0;
							buf[3] = printer.feedSpeed;
							*((__packed INT32S *)&buf[4]) = move_distance;
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
				}
				else
						{
							buf[0] = 8; //Length									
							buf[1] = UART_MOVEDISTANCE_CMD; 
							buf[2] = move_dir; //1,2 X, 3,4 Y, 5,6 Z
				#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
					buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
				#else
					buf[3] = PRINTER_ZAXIS_SPEED;
				#endif
							*((__packed INT32S *)&buf[4]) = move_distance;
							if ( (move_dir != 1) && (move_dir != 2) )
								status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
			#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
					OSFlagAccept(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_WAIT_SET_ANY,&err);
					if(err == OS_NO_ERR && (move_dir == 7 || move_dir == 8))
					{
						if(move_dir == 8)
							status_ReportStatus(STATUS_CAPPED, STATUS_CLR);
						else
							status_ReportStatus(STATUS_CAPPED, STATUS_SET);
					}
			#endif
				}
			}
			else if (move_type == 1) //X/Y/Z go home //Go home. All axes, 1 X 2 Y 3 Z, can be bit or
					{
						buf[0] = 3;								
						buf[1] = UART_GOHOME_CMD; 
						buf[2] = move_dir;//AXIS
						if ((move_dir & 0x1) == 0)
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
			}
			else if (move_type == 3) //X/Y/Z moveto command
					{
						buf[0] = 8; //Length			
						buf[1] = UART_AXISMOVETO_CMD; 
						buf[2] = move_dir; //AXIS
						if(move_dir == 1)
							buf[3] = printer.movSpeed;
						else if(move_dir == 2)
							buf[3] = printer.feedSpeed;
						//buf[3] = 0;
						else
							buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
						
						*((__packed INT32S *)&buf[4]) = move_distance; //move_distance means target position
						if (move_dir != 1)
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
						OSFlagAccept(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_WAIT_SET_ANY,&err);
						if(err == OS_NO_ERR && move_dir == 4)
						{
							if(move_distance == 0)
								status_ReportStatus(STATUS_CAPPED, STATUS_CLR);
							else
								status_ReportStatus(STATUS_CAPPED, STATUS_SET);
						}
					}
					else if (move_type == 4) //measure media
					{
						buf[0] = 3;
						buf[1] = UART_MEASUREMEDIA_CMD;
						buf[2] = measure_media_method;
					}
					else
						CONSOL_Printf("Wrong move command!\r\n");			
					
					while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
						OSTimeDly(10);	
					
					while (1)
					{
#if (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)) && (defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE_3_CAP))
						OS_CPU_SR     cpu_sr = 0;
						INT8U err1, err2, err3, err4;
						
						err1 = err2 = err3 = err4 = 0;
						
						OS_ENTER_CRITICAL();
						OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,&err1);
						OSFlagAccept(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_WAIT_CLR_ANY,&err2);
						OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_CLR_ANY,&err3);
						OSFlagAccept(mix_FLAG_GRP, EPSON_DO_PENDING_MOVE, OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,&err4); //Waiting moving stop		
						OS_EXIT_CRITICAL();
						
						if (err1 == OS_NO_ERR)
						{
							status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
							break;
						}
						else if(err4 == OS_NO_ERR)
							goto NewCMD;
						else if(err2 == OS_NO_ERR && err3 == OS_NO_ERR)
						{
							buf[0] = 2;
							buf[1] = UART_STOPMOVE_CMD;
							if (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
							{
								OSTimeDly(10);	
								continue;
							}
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
							break;
						}
						else			
							OSTimeDly(10);
#else
						
						if (OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,&err))
						{
							status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
							break;
				}
				else if (OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_CLR_ANY,&err))
						{
							buf[0] = 2;
							buf[1] = UART_STOPMOVE_CMD;
							if (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) //Stop move
							{
								OSTimeDly(10);	
								continue;
							}
							OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving stop
							break;
						}
						else			
							OSTimeDly(10);
#endif                    
					}
				}
	}	
}

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

extern volatile INT8U nextband_autoClean;
extern volatile INT8U nextband_autoSpray;
extern struct Struct_JobStart job_info;

INT8U CMD_Epson_Operate_Type = 0;

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
		}

		cleanparam_EPSON_ALLWIN.Config[i].InputAirTime = 50;

		switch(i)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			cleanparam_EPSON_ALLWIN.Config[i].SuckWasteInkTime = 10;
			break;
		}

		cleanparam_EPSON_ALLWIN.Config[i].WipeTimes = 1;
		cleanparam_EPSON_ALLWIN.Config[i].Carriage_X_Wipe_Speed = 7;
		cleanparam_EPSON_ALLWIN.Config[i].FlashFreqInterval = 1000;
		cleanparam_EPSON_ALLWIN.Config[i].FlashTime = 120;
		cleanparam_EPSON_ALLWIN.Config[i].FlashCycle = 40;
		cleanparam_EPSON_ALLWIN.Config[i].FlashIdleInCycle = 10;
	}
	cleanparam_EPSON_ALLWIN.Runtime.ActivedConfig = 2;

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

	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0] = 2500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[0] = 500;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[0] = 4840;
	cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[1] = 3200;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_Start[1] = 4000;
	cleanparam_EPSON_ALLWIN.factory.Carriage_X_WipePos_End[1] = 8840;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos = 2700;
	cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_WipePos = 1150;
	cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos = 1000;
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
	if(Data == 0)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 1;
	else if(Data == 1)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 2;
	else if(Data == 2)
		cleanparam_EPSON_ALLWIN.Runtime.HeadMask = 3;
	else
		ret = False;
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
	if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 1)
		ret = 0;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 2)
		ret = 1;
	else if(cleanparam_EPSON_ALLWIN.Runtime.HeadMask == 3)
		ret = 2;
	else
		ret = -1;
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
	move_distance = 0;//move_distance means target position
	move_type = 3;
	status_ReportStatus(CMD_START_MOVE, STATUS_SET);

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
		OS_EXIT_CRITICAL();
		return False;
	}
}
#endif


#if defined(EPSON_CLEAN_INTEGRATE_1)||defined(RICOH_CLEAN_PRESS)
void wiper_down(void)
{
	INT8U buf[32];
	INT8U err;
	status_ReportStatus(STATUS_MOVING, STATUS_SET);
	buf[0] = 8; //Length
	buf[1] = UART_AXISMOVETO_CMD;
	buf[2] = 8; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
	buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;
	*((__packed INT32S *)&buf[4]) = 0;
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

void sucker_bytime(INT32U ms)//极性反转
{
	CONTROL_OPEN_SUCKER(0);
	OSTimeDly(ms);
	CONTROL_CLOSE_SUCKER(0);
}
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
	if(head_mask ==1)
		head_mask = 3;
	else if(head_mask ==2)
		head_mask = 0xc;
	else
		head_mask = 0xF;
	return head_mask;
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
		flags = OSFlagPend(status_FLAG_GRP, CMD_CLEAN_MANUAL|CMD_CLEAN_SINGLE|CMD_CLEAN_AUTO|CMD_START_MOVE|CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,0,&err); //Cleaning is request
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop

		flags = OSFlagAccept(status_FLAG_GRP, CMD_CLEAN_MANUAL|CMD_CLEAN_SINGLE|CMD_CLEAN_AUTO|CMD_START_MOVE|CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,&err);
		if (flags == 0)
		{
			continue;
		}

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		if (flags & CMD_CLEAN_AUTO)
		{
#if defined (RICOH_CLEAN_PRESS)
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
#endif
				if (nextband_autoSpray)
				{
					nextband_autoSpray = False;
					flags = OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ANY,&err);
					if(flags == 0 && err != 0)
					{
						status_ReportStatus(STATUS_CLEANING, STATUS_SET);

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
						break;
					}
					passnum++;
				}
			}
		}
		else
#endif
			if (flags & CMD_START_MOVE)
			{

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
						buf[6] = printer.movSpeed;
					}
					else if (move_dir == 2)
					{
						buf[0] = 7; //Length
						buf[1] = UART_MOVETO_CMD;
						if (move_distance == 0)
							//*((__packed INT32S *)&buf[2]) = printer.orgLimit;
						{
							*((__packed INT32S *)&buf[2]) = 0;
						}
						else
							*((__packed INT32S *)&buf[2]) = curPos.x - move_distance;
						buf[6] = printer.movSpeed;
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
						buf[3] = PRINTER_ZAXIS_SPEED;
						*((__packed INT32S *)&buf[4]) = move_distance;
						if ( (move_dir != 1) && (move_dir != 2) )
							status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
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
				}
			}
	}
}

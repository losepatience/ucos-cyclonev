#include "os_includes.h"
#include "printers.h"
#include "job.h"
#include "FPGA.h"
#include "hugebuf.h"
#include "status.h"
#include "miscellance.h"
#include "printers.h"
#include "global.h"
#include "flash.h"
#include "consol.h"
#include "tasks.h"
#include "control.h"
#include "lcd.h"


#include "Calibration.h"

#include <FPGA.h>
#ifndef FPGA_SET_DATASEG
#define rFPGA_DATASEG	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xE))
#define FPGA_SET_DATASEG(data)	(rFPGA_DATASEG = data)  //only for EPSON Gen5
#endif


#ifdef UV_PRINTER
#include "uv.h"
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#include "display.h"
#endif

#if defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)
#include "motion.h"
#endif

#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER) 
#define CAL_PRINT_UV_Y_STEP_PERCENT (0.5f)
#define DYSS_BASE_STEP (epsonCaliSetting_runtime.stepPass.BaseStep)
//#define DYSS_BASE_STEP (99999)
#define X_PRINT_UV_SPEED 4
#define Y_MOVE_UV_SPEED 2
//#define UV_LIGHT_HEAD0_OFFSET (10.0f/2.54f)*X_BASE_RES
#define UV_LIGHT_HEAD0_OFFSET (Ripstar_UV_Setting.Front_Light_Head0_Length/2.54)*X_BASE_RES
//#define UV_LIGHT (2.5f/2.54f)*X_BASE_RES
#define UV_LIGHT (Ripstar_UV_Setting.UV_Light_Length/2.54f)*X_BASE_RES

struct Print_Info
{
	INT32S PrintFire_Start;
	INT32S PrintFire_End;	
	INT32S UV_MOVE_Start;
	INT32S UV_MOVE_End;		
} print_Info = {0};

#endif

#if defined(MANUFACTURER_DYSS)
INT32S LAST_PRT_Y_POSI = 0;
#endif

#if defined(HEAD_RICOH_G4)
struct sShakeInfo_Ricoh shakeInfo_Ricoh = {4,1,100};
#endif

volatile INT8U nextband_autoClean = False;
volatile INT8U nextband_autoSpray = False;
#ifdef FAST_CLEAN
volatile INT8U nextband_autoCleanUseFlash = False;		//以闪喷的方式进行清洗(快速清洗)
#endif

static DataHeaderType data_header;
struct Struct_JobStart job_info;

INT32U cur_step_distance;

extern volatile INT8U USBDMA_manualStop;
INT8U security_CheckSWUILanguageSetting(INT8U language, INT8U *str, INT8U isEP0);

//#define DEBUG
#define UPDATEDSP_WAITTIME 20000 //timeout, 20 seconds

//without DSP /jump the white sapce
#if defined(SUPPORT_MOTOR_CONTROL)
extern struct EPR_MOTION_PARAM motionParam;
INT8U GenerateMotionPrePrintCMD(INT8U *buf, INT32U PreFireNum, INT32U PostFireNum)
{
	INT32S startPoint, endPoint, next_startPoint, next_endPoint;
	INT32S fire_StartPoint, fire_EndPoint;	

	INT8U direction, next_direction = data_header.Inf.band_inf.next_dir;

	INT32S platSpace, accSpace, orgLimit, org_oppLimit;

	switch(job_info.baseDPI)
	{
	case 1:
		platSpace = printer.platSpace * 540 / 720;
		orgLimit = printer.orgLimit * 540 / 720;
		org_oppLimit = printer.org_oppLimit * 540 / 720;
		break;
	case 2:
		platSpace = printer.platSpace * 2;
		orgLimit = printer.orgLimit * 2;
		org_oppLimit = printer.org_oppLimit * 2;
		break;
	default:
		platSpace = printer.platSpace ;
		orgLimit = printer.orgLimit;
		org_oppLimit = printer.org_oppLimit;
		break;
	}
	//1:正向(远离原点, 位置值增大) 2:反向(接近原点, 位置值减小)

	direction = data_header.Inf.band_inf.dir;

	if ((direction != 1) && (direction != 2))
	{
		CONSOL_Printf("Wrong direction value in band info!\r\n");
		status_ReportStatus(STATUS_FTA_INTERNAL_BANDDIRECTION, STATUS_SET);
		return False;
	}

	if (direction == 2)
		direction = 0;

	startPoint = data_header.Inf.band_inf.band_X + platSpace;	
	endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width + platSpace;

	if (direction)
	{
		startPoint += g_adjustInf.bidirectionValue;	
		endPoint += g_adjustInf.bidirectionValue;
	}

	fire_StartPoint = startPoint;
	fire_EndPoint = endPoint;

	if(fire_StartPoint != fire_EndPoint)
	{
		if( direction)
		{
			fire_StartPoint -= PreFireNum *job_info.encoder_div;	
			fire_EndPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			fire_StartPoint -= PostFireNum *job_info.encoder_div;	
			fire_EndPoint += PreFireNum *job_info.encoder_div;
		}
	}

	if(!direction)
	{
		endPoint = fire_StartPoint;
		fire_StartPoint = fire_EndPoint;
		fire_EndPoint = endPoint;
	}

	next_startPoint = data_header.Inf.band_inf.next_band_X+platSpace;
	next_endPoint = data_header.Inf.band_inf.next_band_X + data_header.Inf.band_inf.next_band_width+platSpace;

	if (next_direction == 2)
		next_direction = 0;

	if (next_direction)
	{
		next_startPoint += g_adjustInf.bidirectionValue;	
		next_endPoint += g_adjustInf.bidirectionValue;
	}

	if(next_startPoint != next_endPoint)
	{
		if( next_direction)
		{
			next_startPoint -= PreFireNum *job_info.encoder_div;	
			next_endPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			next_startPoint -= PostFireNum *job_info.encoder_div;	
			next_endPoint += PreFireNum *job_info.encoder_div;
		}
	}

	if(!next_direction)
	{
		endPoint = next_startPoint;
		next_startPoint = next_endPoint;
		next_endPoint = endPoint;
	}

	buf[0] = UART_PRE_PRINT_CMD_LEN;
	buf[1] = UART_PRE_PRINT_CMD;

	{
		//The start point of the print area
		*((__packed INT32U *)&buf[2]) = fire_StartPoint;
		//The end point of the print area
		*((__packed INT32U *)&buf[6]) = fire_EndPoint;

		//The points of the next pass
		*((__packed INT32U *)&buf[10]) = next_startPoint;
		*((__packed INT32U *)&buf[14]) = next_endPoint;
	}
	return True;
}
#elif defined(EPSON_PRT_FLASH)
INT8U PRT_FLASH_PrePrint(INT32U PreFireNum, INT32U PostFireNum)
{
	INT32S startPoint, endPoint, next_startPoint, next_endPoint;
	INT32S fire_StartPoint, fire_EndPoint;	

	INT8U direction, next_direction = data_header.Inf.band_inf.next_dir;

	INT32S platSpace, accSpace, orgLimit, org_oppLimit;

	platSpace = printer.platSpace ;
	accSpace = printer.accSpace;
	orgLimit = printer.orgLimit;
	org_oppLimit = printer.org_oppLimit;

	direction = data_header.Inf.band_inf.dir;

	if ((direction != 1) && (direction != 2))
	{
		CONSOL_Printf("Wrong direction value in band info!\r\n");
		status_ReportStatus(STATUS_FTA_INTERNAL_BANDDIRECTION, STATUS_SET);
		return False;
	}

	if (direction == 2)
		direction = 0;

	startPoint = data_header.Inf.band_inf.band_X + platSpace;	
	endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width + platSpace;

	if (direction)
	{
		startPoint += g_adjustInf.bidirectionValue;	
		endPoint += g_adjustInf.bidirectionValue;
	}

	fire_StartPoint = startPoint;
	fire_EndPoint = endPoint;

	if(fire_StartPoint != fire_EndPoint)
	{
		if( direction)
		{
			fire_StartPoint -= PreFireNum *job_info.encoder_div;	
			fire_EndPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			fire_StartPoint -= PostFireNum *job_info.encoder_div;	
			fire_EndPoint += PreFireNum *job_info.encoder_div;
		}
	}

	if(!direction)
	{
		endPoint = fire_StartPoint;
		fire_StartPoint = fire_EndPoint;
		fire_EndPoint = endPoint;
	}

	next_startPoint = data_header.Inf.band_inf.next_band_X+platSpace;
	next_endPoint = data_header.Inf.band_inf.next_band_X + data_header.Inf.band_inf.next_band_width+platSpace;

	if (next_direction == 2)
		next_direction = 0;

	if (next_direction)
	{
		next_startPoint += g_adjustInf.bidirectionValue;	
		next_endPoint += g_adjustInf.bidirectionValue;
	}

	if(next_startPoint != next_endPoint)
	{
		if( next_direction)
		{
			next_startPoint -= PreFireNum *job_info.encoder_div;	
			next_endPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			next_startPoint -= PostFireNum *job_info.encoder_div;	
			next_endPoint += PreFireNum *job_info.encoder_div;
		}
	}

	if(!next_direction)
	{
		endPoint = next_startPoint;
		next_startPoint = next_endPoint;
		next_endPoint = endPoint;
	}

	StartPrePrint(fire_StartPoint, fire_EndPoint,next_startPoint, next_endPoint);

	return True;
}
#endif

#if defined(HEAD_RICOH_G4)
INT16U Current_shake_0_shake_count = 0;
void FPGAPosi_OpenSShake(void * data) FAST_FUNC_SECTION
{
	INT8U UartCMD[32];
	INT32U PreFireNum ,PostFireNum;
	PreFireNum = EPSON_PRE_EXTRA_FIRE_NUM;
	PostFireNum = EPSON_POST_EXTRA_FIRE_NUM;
#ifndef CLOSE_SSHAKE    	
	rFPGA_COMMAND = rFPGA_COMMAND_BGN_SSHAKE;
#endif
	if (data_header.Inf.band_inf.next_band_width != 0)
	{
		FireCount = data_header.Inf.band_inf.next_band_width / job_info.encoder_div;
		FireCountEx = data_header.Inf.band_inf.next_band_width / job_info.encoder_div + PreFireNum+PostFireNum;
	}
	else
	{
		PreFireNum = 0;
		PostFireNum = 0;
		FireCount = 0;
		FireCountEx = 0;
	}
	UartCMD[0] = 6;
	UartCMD[1] = UART_HEAD_EPSON_PASSSTART;
	UartCMD[2] = (INT8U)FireCount ;
	UartCMD[3] = (INT8U)(FireCount>>8);
	UartCMD[4] = (INT8U)FireCountEx;
	UartCMD[5] = (INT8U)(FireCountEx>>8);
	UART_SendCMD(UART_HEAD_CHANNEL, UartCMD);
	PopPendingConfig();
}


static INT8U SShake_direction = 0;
static INT32S SShake_position = 0;
static INT8U SShake_option = 0;
void FPGAPosi_ClosePrint(void * data) FAST_FUNC_SECTION
{
	INT32U Position;
	INT8U Option;
	INT8U bPending = False;

	rFPGA_COMMAND = rFPGA_COMMAND_END_BAND;

	if(GetPendingConfig(0, &Position, &Option))
	{
		if(Option == SShake_option)
		{
			if( (SShake_direction && SShake_position + XORIGIN_OFFSET > Position) ||
					(!SShake_direction && SShake_position + XORIGIN_OFFSET < Position) )
			{
				bPending = True;
			}
		}
	}

	if(bPending)
	{
		PopPendingConfig();
		RegPendingConfig(0, SShake_position + XORIGIN_OFFSET, SShake_option, FPGAPosi_OpenSShake, 0);
	}
	else
	{
		SetTargetPosi(0, SShake_position + XORIGIN_OFFSET, SShake_option);
		ConfigFPGAPosiIT(0, FPGAPosi_OpenSShake, 0);
	}
}

static INT32S ClosePrint_position = 0;
void FPGAPosi_OpenPrint(void * data) FAST_FUNC_SECTION
{
	INT32U Position;
	INT8U Option;
	INT8U bPending = False;

	FPGA_BEGIN_BAND();   //FPGA transfer first data to print head

	if(GetPendingConfig(0, &Position, &Option))
	{
		if(Option == SShake_option)
		{
			if( (SShake_direction && ClosePrint_position + XORIGIN_OFFSET > Position) ||
					(!SShake_direction && ClosePrint_position + XORIGIN_OFFSET < Position) )
			{
				bPending = True;
			}
		}
	}

	if(bPending)
	{
		PopPendingConfig();
		RegPendingConfig(0, ClosePrint_position + XORIGIN_OFFSET, SShake_option, FPGAPosi_ClosePrint, 0);
	}
	else
	{
		SetTargetPosi(0, ClosePrint_position + XORIGIN_OFFSET, SShake_option);
		ConfigFPGAPosiIT(0, FPGAPosi_ClosePrint, 0);
	}
}

static INT32S OpenPrint_position = 0;
void FPGAPosi_OpenFixedSShake(void * data) FAST_FUNC_SECTION
{
	INT32U Position;
	INT8U Option;
	INT8U bPending = False;
#ifndef CLOSE_SSHAKE    
	if(Current_shake_0_shake_count == 100)
		rFPGA_COMMAND = rFPGA_COMMAND_BGN_SSHAKE_100;
	else
		rFPGA_COMMAND = rFPGA_COMMAND_BGN_SSHAKE_250;
	//current_pos_x = rFPGA_RICOH_XPRTCOOR_L | (rFPGA_RICOH_XPRTCOOR_H << 16);
#endif    
	if(GetPendingConfig(0, &Position, &Option))
	{
		if(Option == SShake_option)
		{
			if( (SShake_direction && OpenPrint_position + XORIGIN_OFFSET > Position) ||
					(!SShake_direction && OpenPrint_position + XORIGIN_OFFSET < Position) )
			{
				bPending = True;
			}
		}
	}

	if(bPending)
	{
		PopPendingConfig();
		RegPendingConfig(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option, FPGAPosi_OpenPrint, 0);
	}
	else
	{
		SetTargetPosi(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option);
		ConfigFPGAPosiIT(0, FPGAPosi_OpenPrint, 0);
	}
}

const float vsd_speed[4] = {71.43, 35.71, 41.67, 50.0}; //us
const float cycletime_sshake = 62.53; //us.
const float time_between_sshake_print = 70.0; //ms.
const float time_between_print_sshake = 50.0; //ms.
INT8U MonitorPosiStartSShake(INT8U direction, INT32S fire_StartPoint, INT32S fire_EndPoint,
		INT8U encoder_div,  INT8U VSDModel)
{
	INT32S before_point;
	float time = 0 ;
	INT32U Position;
	INT8U Option;
	INT8U bPending = False;
	SShake_direction = direction;
	if(direction)
	{
		OpenPrint_position = fire_StartPoint - 10;
		ClosePrint_position = fire_EndPoint + 10;
		SShake_position = fire_EndPoint + 100;
		SShake_option = 5;
	}
	else
	{
		OpenPrint_position = fire_EndPoint + 10;
		ClosePrint_position = fire_StartPoint - 10;
		SShake_position = fire_StartPoint - 100;
		SShake_option = 9;
	}
	if(Current_shake_0_shake_count!=0)
	{
		time =Current_shake_0_shake_count * cycletime_sshake;
		time = time/vsd_speed[VSDModel-1];
		before_point = (INT32S) time;
		before_point *= 2; //for safe.
		before_point *= encoder_div;
		if(direction)
		{
			before_point = fire_StartPoint - before_point;
			SetTargetPosi(0, before_point + XORIGIN_OFFSET, 5);
		}
		else
		{
			before_point = fire_EndPoint + before_point;
			SetTargetPosi(0, before_point + XORIGIN_OFFSET, 9);
		}
		ConfigFPGAPosiIT(0, FPGAPosi_OpenFixedSShake, 0);
		EnableFPGAPosiIT(0);
	}
	else
	{
		if(GetPendingConfig(0, &Position, &Option))
		{
			if(Option == SShake_option)
			{
				if( (SShake_direction && OpenPrint_position + XORIGIN_OFFSET > Position) ||
						(!SShake_direction && OpenPrint_position + XORIGIN_OFFSET < Position) )
				{
					bPending = True;
				}
			}
		}

		if(bPending)
		{
			PopPendingConfig();
			RegPendingConfig(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option, FPGAPosi_OpenPrint, 0);
		}
		else
		{
			SetTargetPosi(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option);
			ConfigFPGAPosiIT(0, FPGAPosi_OpenPrint, 0);
		}
	}
}

static INT32S SShake_Fix_position = 0;
void FPGAPosi_CloseSShake(void * data) FAST_FUNC_SECTION
{
	INT32U Position;
	INT8U Option;
	INT8U bPending = False;

	rFPGA_COMMAND = rFPGA_COMMAND_END_SSHAKE;
	if(Current_shake_0_shake_count!=0)
	{
		if(GetPendingConfig(0, &Position, &Option))
		{
			if(Option == SShake_option)
			{
				if( (SShake_direction && SShake_Fix_position > Position) ||
						(!SShake_direction && SShake_Fix_position < Position) )
				{
					bPending = True;
				}
			}
		}

		if(bPending)
		{
			PopPendingConfig();
			RegPendingConfig(0, SShake_Fix_position, SShake_option, FPGAPosi_OpenFixedSShake, 0);
		}
		else
		{
			SetTargetPosi(0, SShake_Fix_position, SShake_option);
			ConfigFPGAPosiIT(0, FPGAPosi_OpenFixedSShake, 0);
		}
	}
	else
	{
		if(GetPendingConfig(0, &Position, &Option))
		{
			if(Option == SShake_option)
			{
				if( (SShake_direction && OpenPrint_position + XORIGIN_OFFSET > Position) ||
						(!SShake_direction && OpenPrint_position + XORIGIN_OFFSET < Position) )
				{
					bPending = True;
				}
			}
		}

		if(bPending)
		{
			PopPendingConfig();
			RegPendingConfig(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option, FPGAPosi_OpenPrint, 0);
		}
		else
		{
			SetTargetPosi(0, OpenPrint_position + XORIGIN_OFFSET, SShake_option);
			ConfigFPGAPosiIT(0, FPGAPosi_OpenPrint, 0);
		}
	}
}
INT8U MonitorPosiEndSShake(INT8U direction, INT32S fire_StartPoint, INT32S fire_EndPoint,
		INT8U encoder_div,  INT8U VSDModel)
{
	INT32S before_point;
	float time = 0;
	if(Current_shake_0_shake_count!=0)
	{
		time = Current_shake_0_shake_count * cycletime_sshake;
		time = time/vsd_speed[VSDModel-1];
		before_point = (INT32S) time;
		before_point *= 2; //for safe.
		before_point *= encoder_div;
		if(direction)
		{
			SShake_Fix_position = fire_StartPoint - before_point + XORIGIN_OFFSET;
			SetTargetPosi(0, SShake_Fix_position - 100, 5);
		}
		else
		{
			SShake_Fix_position = fire_EndPoint + before_point + XORIGIN_OFFSET;
			SetTargetPosi(0, SShake_Fix_position + 100, 9);
		}
		ConfigFPGAPosiIT(0, FPGAPosi_CloseSShake, 0);
		EnableFPGAPosiIT(0);
	}
	else
	{
		before_point = 50;	
		if(direction)
		{
			SShake_Fix_position = fire_StartPoint - before_point + XORIGIN_OFFSET;
			SetTargetPosi(0, SShake_Fix_position, 5);
		}
		else
		{
			SShake_Fix_position = fire_EndPoint + before_point + XORIGIN_OFFSET;
			SetTargetPosi(0, SShake_Fix_position, 9);
		}
		//SShake_Fix_posi[lb_num++]  = SShake_Fix_position;
		ConfigFPGAPosiIT(0, FPGAPosi_CloseSShake, 0);
		EnableFPGAPosiIT(0);
	}

	SShake_direction = direction;
	if(direction)
	{
		SShake_position = fire_EndPoint + 100;
		ClosePrint_position = fire_EndPoint + 10;
		SShake_option = 5;
	}
	else
	{
		SShake_position = fire_StartPoint - 100;
		ClosePrint_position = fire_StartPoint - 10;
		SShake_option = 9;
	}
	if(direction)
	{
		OpenPrint_position = fire_StartPoint - 10;
		ClosePrint_position = fire_EndPoint + 10;
		SShake_position = fire_EndPoint + 100;
		SShake_option = 5;
	}
	else
	{
		OpenPrint_position = fire_EndPoint + 10;
		ClosePrint_position = fire_StartPoint - 10;
		SShake_position = fire_StartPoint - 100;
		SShake_option = 9;
	}
}

#endif

#ifdef COORD_NEW_UV 
void UV_POSI_INIT(INT32S Fire_on,INT32S Fire_off,INT8U dir)
{
	Fire_on -= (Ripstar_UV_Setting.Advance_Open_Length/2.54*X_BASE_RES);
	Fire_off += (Ripstar_UV_Setting.Delay_Close_Length/2.54*X_BASE_RES); 
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();
	UV_POSI.STOP_UV = False;

	UV_POSI.Front_Light_OPEN = Fire_on - (Ripstar_UV_Setting.Front_Light_Head0_Length - Ripstar_UV_Setting.Head_Region_length + Ripstar_UV_Setting.UV_Light_Length)/2.54*X_BASE_RES;
	UV_POSI.Front_Light_CLOSE = Fire_off - (Ripstar_UV_Setting.Front_Light_Head0_Length)/2.54*X_BASE_RES;

	UV_POSI.Back_Light_OPEN = Fire_on + (Ripstar_UV_Setting.Back_Light_Head0_Length + Ripstar_UV_Setting.Head_Region_length)/2.54*X_BASE_RES;
	UV_POSI.Back_Light_CLOSE = Fire_off + (Ripstar_UV_Setting.Back_Light_Head0_Length + Ripstar_UV_Setting.UV_Light_Length)/2.54*X_BASE_RES;


	if(UV_POSI.Front_Light_OPEN < 0)
		UV_POSI.Front_Light_OPEN = 0;
	if(UV_POSI.Back_Light_OPEN < 0)
		UV_POSI.Back_Light_OPEN = 0;
	OS_EXIT_CRITICAL();
}
#endif


#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U GenerateMotionPrintCMD(INT8U *buf, INT8U bBlankBand, INT32U PreFireNum, INT32U PostFireNum)
#else
INT8U GenerateMotionPrintCMD(INT8U *buf, INT8U bBlankBand)
#endif
{
	INT32S startPoint, endPoint, next_startPoint, next_endPoint;
	INT32S fire_StartPoint, fire_EndPoint;	

	INT8U direction, next_direction = data_header.Inf.band_inf.next_dir;

	INT32S platSpace, accSpace, orgLimit, org_oppLimit;

	OS_CPU_SR cpu_sr;

#ifdef SUPPORT_MOTOR_CONTROL
	switch(job_info.baseDPI)
	{
	case 1:
		platSpace = printer.platSpace * 540 / 720;
		accSpace = printer.accSpace *540 / 720;
		orgLimit = printer.orgLimit * 540 / 720;
		org_oppLimit = printer.org_oppLimit * 540 / 720;
		break;
	case 2:
		platSpace = printer.platSpace * 2;
		accSpace = printer.accSpace * 2;
		orgLimit = printer.orgLimit * 2;
		org_oppLimit = printer.org_oppLimit * 2;
		break;
	default:
		platSpace = printer.platSpace ;
		accSpace = printer.accSpace;
		orgLimit = printer.orgLimit;
		org_oppLimit = printer.org_oppLimit;
		break;
	}
#else
	platSpace = printer.platSpace ;
	accSpace = printer.accSpace;
	orgLimit = printer.orgLimit;
	org_oppLimit = printer.org_oppLimit;
#endif	
	//1:正向(远离原点, 位置值增大) 2:反向(接近原点, 位置值减小)

	direction = data_header.Inf.band_inf.dir;

	if ((direction != 1) && (direction != 2))
	{
		CONSOL_Printf("Wrong direction value in band info!\r\n");
		status_ReportStatus(STATUS_FTA_INTERNAL_BANDDIRECTION, STATUS_SET);
		return False;
	}

	if (direction == 2)
		direction = 0;
	if (next_direction == 2)
		next_direction = 0;

#ifdef Y_ENCODER_ENABLE		
	//	if (next_direction == 2) //20090328 add, this is wrong, why???
	//		next_direction = 0;
#endif		

	//1:正向(远离原点, 位置值增大) 0:反向(接近原点, 位置值减小)

#ifdef UV_PRINTER	
	uv_PrintDir = direction;
#endif	
	//	startPoint = data_header.Inf.band_inf.band_X + printer.platSpace + job_info.dir_space;	???????????????????
	startPoint = data_header.Inf.band_inf.band_X + platSpace;	
	endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width + platSpace; // + printer.platSpace?
	//	endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width;

	//#ifdef Y_ENCODER_ENABLE
	if (direction)
	{
		startPoint += g_adjustInf.bidirectionValue;	
		endPoint += g_adjustInf.bidirectionValue;
	}
	//#endif	
	if (cleanPara.autoClean_passInterval == 0xFF)
		cleanPara.autoClean_passInterval--;

	if (cleanPara.autoClean_passInterval%2 != 0)
		cleanPara.autoClean_passInterval++;

	if (cleanPara.longflash_passInterval%2 != 0)
		cleanPara.longflash_passInterval++; 		
#ifdef AUTO_CLEAN_BYTIME
#ifdef EPSON_CLEAN_UPDOWN
	OS_ENTER_CRITICAL();
	if(cleanparam_EPSON_ALLWIN.AutoClean_passInterval <= (Clean_Wait_Time/60000)
			&&cleanparam_EPSON_ALLWIN.bAutoClean == True)
	{
		Clean_Wait_Time = 0;
		nextband_autoClean = True;		
	}
	OS_EXIT_CRITICAL();
#else
	OS_ENTER_CRITICAL();
	if(cleanparam_EPSON_MICOLOR.AutoClean_timeInterval <= (Clean_Wait_Time/60000)
			&& cleanparam_EPSON_MICOLOR.bAutoClean == True)
	{
		Clean_Wait_Time = 0;
		nextband_autoClean = True;		
	}
	OS_EXIT_CRITICAL();
#endif      //end DGEN_AUTOFUCTION

#else
#if (!(defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)))||defined(MANUFACTURER_MICOLOR_EPSON) || defined(MANUFACTURER_COLORFUL)
#ifdef RIGHT_ORIGIN //不应该是右原点，而是原点和清洗口相对
	if ((cleanPara.autoClean_passInterval != 0) && (cleanPara.autoCleanTimes != 0) && (printedBands != 0)
			&& ( ((printedBands)%cleanPara.autoClean_passInterval == 0) ) ) //Auto clean
		nextband_autoClean = True;	
#else
	if ((cleanPara.autoClean_passInterval != 0) && (cleanPara.autoCleanTimes != 0)
			&& ((printedBands)%2 !=0)
			&& ( (printedBands%cleanPara.autoClean_passInterval == cleanPara.autoClean_passInterval - 1) ) ) //Auto clean
		nextband_autoClean = True;		
#endif
#endif
#endif
#ifdef FAST_CLEAN
	//如果是快速清洗，则改清洗为闪喷
	if(nextband_autoClean == True)
		nextband_autoCleanUseFlash = True;
#endif
#ifndef HEAD_EPSON_GEN5
#ifdef RIGHT_ORIGIN //不应该是右原点，而是原点和清洗口相对
	if ((cleanPara.longflash_passInterval != 0) && (printedBands != 0)
			&& ( (printedBands%cleanPara.longflash_passInterval == 0) ) ) //Auto Spay
		nextband_autoSpray = True;	
#else
	if ((cleanPara.longflash_passInterval != 0)
			&& ((printedBands)%2 !=0)
			&& ( ((printedBands)%cleanPara.longflash_passInterval == cleanPara.longflash_passInterval - 1) ) ) //Auto Spray
		nextband_autoSpray = True;		
#endif		
#endif

	fire_StartPoint = startPoint;
	fire_EndPoint = endPoint;

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#ifdef MANUFACTURER_DYSS
	if(job_info.encoder_div > 2)
	{
		if( direction)
		{
			fire_StartPoint -= PreFireNum *2;	
			fire_EndPoint += PostFireNum *2;
		}
		else
		{
			fire_StartPoint -= PostFireNum *2;	
			fire_EndPoint += PreFireNum *2;
		}
	}
	else
#endif
	{
		if( direction)
		{
			fire_StartPoint -= PreFireNum *job_info.encoder_div;	
			fire_EndPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			fire_StartPoint -= PostFireNum *job_info.encoder_div;	
			fire_EndPoint += PreFireNum *job_info.encoder_div;
		}
		//for epson,we assert extra fire number < accSpace. current extra fire number is 116+114 << about 2000.
	}
#endif

#if defined(HEAD_RICOH_G4)
	Current_shake_0_shake_count = shakeInfo_Ricoh.Small_shake_0_shake_count;

	if((rFPGA_STATUS & 0x7) == 4)
		MonitorPosiEndSShake(direction, fire_StartPoint, fire_EndPoint, job_info.encoder_div,  job_info.VSDModel);
	else
		MonitorPosiStartSShake(direction, fire_StartPoint, fire_EndPoint, job_info.encoder_div,  job_info.VSDModel);

#endif

	startPoint -= accSpace;
	endPoint += accSpace;
#ifdef MANUFACTURER_DYSS
	if(startPoint < 0)
		startPoint = 0;
	if(endPoint > DYSS_MAX_X_PLAT_WIDTH*X_BASE_RES)
		endPoint = DYSS_MAX_X_PLAT_WIDTH*X_BASE_RES;
#endif     
	//	CONSOL_Printf("Band width: %d", data_header.Inf.band_inf.band_width);
	//	CONSOL_Printf("Next band width: %d\r\n", data_header.Inf.band_inf.next_band_width);

	if (startPoint < orgLimit)
	{
		CONSOL_Printf("Start point is out of the printer range!\r\n");
		status_ReportStatus(STATUS_FTA_INTERNAL_OLIMIT, STATUS_SET); //If this error, it is bug
		return False;
	}

	if (endPoint > org_oppLimit)
	{
		CONSOL_Printf("End point is out of the printer range!\r\n");
		status_ReportStatus(STATUS_FTA_INTERNAL_OPPLIMIT, STATUS_SET);
		return False;
	}

	if( direction != next_direction)
	{
		//		next_startPoint = data_header.Inf.band_inf.next_band_X;
		//		next_endPoint = data_header.Inf.band_inf.next_band_X + data_header.Inf.band_inf.next_band_width;
		next_startPoint = data_header.Inf.band_inf.next_band_X+platSpace;
		next_endPoint = data_header.Inf.band_inf.next_band_X + data_header.Inf.band_inf.next_band_width+ platSpace;

		if (next_direction)
		{
			next_startPoint += g_adjustInf.bidirectionValue;	
			next_endPoint += g_adjustInf.bidirectionValue;
		}

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#ifdef MANUFACTURER_DYSS
		if(job_info.encoder_div > 2)
		{
			if( next_direction)
			{
				next_startPoint -= PreFireNum *2;	
				next_endPoint += PostFireNum *2;
			}
			else
			{
				next_startPoint -= PostFireNum *2;	
				next_endPoint += PreFireNum *2;
			}
		}
		else
#endif
		{
			if( next_direction)
			{
				next_startPoint -= PreFireNum *job_info.encoder_div;	
				next_endPoint += PostFireNum *job_info.encoder_div;
			}
			else
			{
				next_startPoint -= PostFireNum *job_info.encoder_div;	
				next_endPoint += PreFireNum *job_info.encoder_div;
			}
			//for epson,we assert extra fire number < accSpace. current extra fire number is 116+114 << about 2000.
		}
#endif

#ifdef COORD_NEW_UV
		UV_POSI_INIT(next_startPoint, next_endPoint,next_direction);
#endif
		next_startPoint -= accSpace;
		next_endPoint += accSpace;

#ifdef COORD_NEW_UV
		if( direction == True)
		{
			if(UV_POSI.Back_Light_CLOSE > next_endPoint)
			{
				next_endPoint = UV_POSI.Back_Light_CLOSE;
			}
			if(UV_POSI.Front_Light_OPEN < next_startPoint)
			{
				next_startPoint = UV_POSI.Front_Light_OPEN;
			}
		}else
		{
			if(UV_POSI.Back_Light_CLOSE > next_endPoint)
			{
				next_endPoint = UV_POSI.Back_Light_CLOSE;
			}
			if(UV_POSI.Front_Light_OPEN < next_startPoint)
			{
				next_startPoint = UV_POSI.Front_Light_OPEN;
			}
		}
#endif

#if MANUFACTURER_DYSS
		if(next_startPoint < 0)
			next_startPoint = 0;	
		if( next_endPoint >  DYSS_MAX_X_PLAT_WIDTH*X_BASE_RES) 
			next_endPoint = DYSS_MAX_X_PLAT_WIDTH*X_BASE_RES; 
#endif 
		if( direction == True)
		{
			if( endPoint < next_endPoint)
				endPoint = next_endPoint;
			if( startPoint >= endPoint)
				status_ReportStatus(STATUS_FTA_INTERNAL_PRINTPOINT,STATUS_SET);
		}
		else
		{

			if( startPoint > next_startPoint )
				startPoint = next_startPoint;

			//exchange startpoint and endpoint
			next_startPoint = startPoint; //next_startPoint just use as a temparary space
			startPoint = endPoint;
			endPoint = next_startPoint;	
			if (startPoint <= endPoint)
				status_ReportStatus(STATUS_FTA_INTERNAL_PRINTPOINT,STATUS_SET);
		}
	}
	else if(direction == 0)
	{
		//exchange startpoint and endpoint
		next_startPoint = startPoint; //next_startPoint just use as a temparary space
		startPoint = endPoint;
		endPoint = next_startPoint;	
		if (startPoint <= endPoint)
			status_ReportStatus(STATUS_FTA_INTERNAL_PRINTPOINT,STATUS_SET);
	}

	buf[0] = UART_PRINT_CMD_LEN;
	buf[1] = UART_PRINT_CMD;

#ifdef COORD_NEW_UV 
	if (!bBlankBand)
		UV_POSI_INIT(fire_StartPoint, fire_EndPoint,direction);
	if( direction == True)
	{
		if(UV_POSI.Back_Light_CLOSE > endPoint)
			endPoint = UV_POSI.Back_Light_CLOSE;
		if(UV_POSI.Front_Light_OPEN < startPoint)
			startPoint = UV_POSI.Front_Light_OPEN;
	}else
	{
		if(UV_POSI.Back_Light_CLOSE > startPoint)
			startPoint = UV_POSI.Back_Light_CLOSE;
		if(UV_POSI.Front_Light_OPEN < endPoint)
			endPoint = UV_POSI.Front_Light_OPEN;
	}
#endif     
	if (bBlankBand)
	{
		//The start point of the print area
		*((__packed INT32U *)&buf[2]) = 0;
		//The end point of the print area
		*((__packed INT32U *)&buf[6]) = 0;

		//The points of the movement
		*((__packed INT32U *)&buf[10]) = curPos.x; //cheney: not used for ARM movement
		*((__packed INT32U *)&buf[14]) = 0;
		if (data_header.Inf.band_inf.step_distance == 0)
			status_ReportStatus(STATUS_FTA_INTERNAL_SW1, STATUS_SET);
	}else
	{
		//The start point of the print area
		*((__packed INT32U *)&buf[2]) = fire_StartPoint;
		//The end point of the print area
		*((__packed INT32U *)&buf[6]) = fire_EndPoint;

		//The points of the movement
		*((__packed INT32U *)&buf[10]) = startPoint;
		*((__packed INT32U *)&buf[14]) = endPoint;
	}

#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER) 
	print_Info.PrintFire_Start = fire_StartPoint;
	print_Info.PrintFire_End = fire_EndPoint;
#endif

#if defined(MANUFACTURER_DYSS)  
	int id = 0;
	static float i=0;
	static int start = 0;
	int offset =0;
	if(g_adjustInf.passStepValue!=0)
		offset =(Y_PLAT_LENGTH*192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)/g_adjustInf.passStepValue;
	if(data_header.Inf.band_inf.band_Y <= 16)
	{
		if(g_adjustInf.passStepValue!=0)
			start = (curPos.y*192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)/g_adjustInf.passStepValue;
		i=0;
	}
	if (g_adjustInf.passAdvance != 0)
	{

		if(job_info.YDir ==0)
		{
			if(data_header.Inf.band_inf.band_Y + start<=96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution+offset)
				id =-1;
			else
			{
				id = (data_header.Inf.band_inf.band_Y + start -96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution-offset)/(96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution);
				//if((data_header.Inf.band_inf.band_Y+ start -96*job_info.displayResolution-offset)%(96*job_info.displayResolution)==0&&data_header.Inf.band_inf.nextBandY!=0)
				//   id--;
			}

		}
		else
		{
			if(start - data_header.Inf.band_inf.nextBandY <=96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution+offset)
				id =-1;
			else
			{
				id = (start - data_header.Inf.band_inf.nextBandY -96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution-offset)/(96*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution);
				//  if(data_header.Inf.band_inf.nextBandY+ start - 96*job_info.displayResolution)%(96*job_info.displayResolution)==0&&data_header.Inf.band_inf.nextBandY!=0)
				//      id++;
			}
		}
		if(id==-1)
		{
			cur_step_distance = (int)(((float)(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y))
					* (g_adjustInf.passStepValue)/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+ data_header.Inf.band_inf.shiftY + i);
			if(data_header.Inf.band_inf.band_Y != data_header.Inf.band_inf.nextBandY)			  
				i=(((float)(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y))
						* (g_adjustInf.passStepValue)/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+ data_header.Inf.band_inf.shiftY + i) -
					cur_step_distance;
		}
		else
		{
			cur_step_distance = (int)((float)(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y)
					* g_adjustInf.passStepValue/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+ 
					(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y)
					* (LCDMenuConfig_EX.add[id]*g_adjustInf.passStepValue/192)/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+
					data_header.Inf.band_inf.shiftY + i);
			if(data_header.Inf.band_inf.band_Y != data_header.Inf.band_inf.nextBandY)			 
				i=((float)(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y)
						* g_adjustInf.passStepValue/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+ 
						(data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y)
						* (LCDMenuConfig_EX.add[id]*g_adjustInf.passStepValue/192)/(192*epsonCaliSetting.printerSetting.YInterleaveNum*job_info.displayResolution)+
						data_header.Inf.band_inf.shiftY + i) - cur_step_distance;

		}
		if(data_header.Inf.band_inf.band_Y == data_header.Inf.band_inf.nextBandY)
			cur_step_distance =0;
	}
	else
	{
		//  int step = epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
		int pass = epsonCaliSetting_runtime.stepPass.BaseStep / data_header.Inf.band_inf.step_distance;
		if(pass==0)
			pass =1;
		if(data_header.Inf.band_inf.band_Y == 0)
			i=0;
		if(data_header.Inf.band_inf.nextBandY <=96*epsonCaliSetting.printerSetting.YInterleaveNum)
			id = -1;
		else
			id = (data_header.Inf.band_inf.nextBandY - 96*epsonCaliSetting.printerSetting.YInterleaveNum)/(96*epsonCaliSetting.printerSetting.YInterleaveNum);
		if((data_header.Inf.band_inf.nextBandY - 96*epsonCaliSetting.printerSetting.YInterleaveNum)%(96*epsonCaliSetting.printerSetting.YInterleaveNum)==0)
			id--;
		// i = ((((data_header.Inf.band_inf.nextBandY )%(192*job_info.displayResolution))*4)/(192*job_info.displayResolution))%4;
		if(id<0)
			cur_step_distance = data_header.Inf.band_inf.step_distance;
		else
		{

			cur_step_distance = (int)(data_header.Inf.band_inf.step_distance+epsonCaliSetting_runtime.stepPass.BaseStep*LCDMenuConfig_EX.add[id]/192/pass +i);
		}
		if(id>=0)
			i = data_header.Inf.band_inf.step_distance +epsonCaliSetting_runtime.stepPass.BaseStep*LCDMenuConfig_EX.add[id]/192/pass +i- cur_step_distance;
		else
			i = data_header.Inf.band_inf.step_distance - cur_step_distance;
	}
#else
	//The move distance after print this band
	//#ifdef Y_ENCODER_ENABLE	
	if (g_adjustInf.passAdvance != 0)
		cur_step_distance = (data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y)
			* g_adjustInf.passStepValue/g_adjustInf.passAdvance + data_header.Inf.band_inf.shiftY;
	else
		cur_step_distance = data_header.Inf.band_inf.step_distance;
	//#else
	//	cur_step_distance = data_header.Inf.band_inf.step_distance;		
	//#endif
#endif
#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER)
	INT8U err;
	if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		if(cur_step_distance > DYSS_BASE_STEP * CAL_PRINT_UV_Y_STEP_PERCENT)
		{
			*((__packed INT32U *)&buf[18]) = 0;
			OSFlagPost(mix_FLAG_GRP_2,DYSS_UV_LIGHT_CAL,OS_FLAG_SET,&err);
		}
		else
			*((__packed INT32U *)&buf[18]) = cur_step_distance;
	}
	else
	{
		*((__packed INT32U *)&buf[18]) = cur_step_distance;   
	}
#else
	*((__packed INT32U *)&buf[18]) = cur_step_distance;
#endif    

	//	CONSOL_Printf("Start Point: %d", startPoint);
	//	CONSOL_Printf("End   Point: %d\r\n", endPoint);

#if defined(DEBUG_Y_ENCODER)
	status_EP0Buf.statusCode[5] = data_header.Inf.band_inf.nextBandY;
	status_EP0Buf.statusCode[6] = data_header.Inf.band_inf.band_Y;
	status_EP0Buf.statusCode[7] = g_adjustInf.passStepValue;
	status_EP0Buf.statusCode[8] = data_header.Inf.band_inf.nextBandY - data_header.Inf.band_inf.band_Y;
	status_EP0Buf.statusCode[9] = data_header.Inf.band_inf.shiftY;
	status_EP0Buf.step_distance = cur_step_distance;
	status_EP0Buf.passAdvance = g_adjustInf.passAdvance;
#endif
	return True;	
}

#if defined(TEST_FIRE_BY_FPGA)
INT32S X_posi = 0;
void FPGAPosi_Test_IntHandler(void * data)
{
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();
	rFPGA_EPSON_RegAddr = EPSON_REGADDR_X_PRT_COOR;
	X_posi = rFPGA_EPSON_RegPort;
	rFPGA_EPSON_RegAddr = EPSON_REGADDR_POSI_STAT;
	rFPGA_EPSON_RegPortL;
	OS_EXIT_CRITICAL();

	return;
}
#endif

#ifdef DEBUG_FPGA_BUS
INT16U FPGA_DATA_CHECKSUM = 0;
INT16U * pStart = NULL;
INT32U start_size, align_size;
#endif

#if 0 //liyangdefined(FPGA_MAINTAIN_COOR)
INT8U IsDelaySendData()
{
	//unplement...
	return False;
}

void DelaySendData(INT32U PreFireNum, INT32U PostFireNum, INT8U b_first_pass)
{
	//unplement...
}
#endif

INT16U *CurHugeBufDataAddr = NULL;
#define DEN5_FIX_FIRE_INTERVAL 8
#define DEN5_FIX_FIRE 4088
void Put_Fpga_Info(INT8U new,INT32U data,INT8U ISerror)
{
	FPGA_SET_DATASEG(data); 
	if(new)
		FPGA_SET_DATASEG(data); 
	if(!ISerror)
	{
		FPGA_SET_DATASEG(8);
		return;	 
	}
	switch(job_info.VSDModel)
	{
	case 1: //VSD1
		FPGA_SET_DATASEG(4);
		break;
	case 2: //VSD2
		FPGA_SET_DATASEG(5);
		break;
	default:
	case 3: //VSD3
		FPGA_SET_DATASEG(6);
		break;
	case 4: //VSD4
		FPGA_SET_DATASEG(7);
		break;
	}

}
void GEN5E_Change_Memory(INT32U real_start)
{
	INT32S Cur_Band_Size = data_header.data_bytescnt - real_start*EPSON_DATA_PER_FIRE;
	CurHugeBufDataAddr = (INT16U *)((INT32U)CurHugeBufDataAddr + real_start*EPSON_DATA_PER_FIRE);
	if ((INT32U)CurHugeBufDataAddr >= HUGEBUF_END_ADDR)
		CurHugeBufDataAddr = (INT16U *)((INT32U)CurHugeBufDataAddr - (HUGEBUF_END_ADDR - HUGEBUF_START_ADDR));

	while(Cur_Band_Size >= (DEN5_FIX_FIRE_INTERVAL + 1) * EPSON_DATA_PER_FIRE)
	{
		if((INT32U)CurHugeBufDataAddr + EPSON_DATA_PER_FIRE > HUGEBUF_END_ADDR)
		{
			int len = HUGEBUF_END_ADDR - (INT32U)CurHugeBufDataAddr;
			int pSrc = ((INT32U)CurHugeBufDataAddr + DEN5_FIX_FIRE_INTERVAL*EPSON_DATA_PER_FIRE) - (HUGEBUF_END_ADDR - HUGEBUF_START_ADDR);
			memcpy((void*)CurHugeBufDataAddr,(void*)pSrc, len);
			memcpy((void*)HUGEBUF_START_ADDR, (void*)(pSrc +len), EPSON_DATA_PER_FIRE - len);
		}
		else
		{
			if((INT32U)CurHugeBufDataAddr + DEN5_FIX_FIRE_INTERVAL*EPSON_DATA_PER_FIRE >= HUGEBUF_END_ADDR)
				memcpy((void*)CurHugeBufDataAddr,(void*)((INT32U)CurHugeBufDataAddr + DEN5_FIX_FIRE_INTERVAL*EPSON_DATA_PER_FIRE - (HUGEBUF_END_ADDR - HUGEBUF_START_ADDR)),EPSON_DATA_PER_FIRE);
			else if((INT32U)CurHugeBufDataAddr + (DEN5_FIX_FIRE_INTERVAL + 1)*EPSON_DATA_PER_FIRE > HUGEBUF_END_ADDR)
			{
				int len = (HUGEBUF_END_ADDR - ((INT32U)CurHugeBufDataAddr + (DEN5_FIX_FIRE_INTERVAL)*EPSON_DATA_PER_FIRE));
				memcpy((void*)CurHugeBufDataAddr,(void*)((INT32U)CurHugeBufDataAddr + DEN5_FIX_FIRE_INTERVAL*EPSON_DATA_PER_FIRE), len);
				memcpy((void*)((INT32U)CurHugeBufDataAddr + len),(void*)HUGEBUF_START_ADDR, EPSON_DATA_PER_FIRE - len);
			}
			else
				memcpy((void*)CurHugeBufDataAddr,(void*)((INT32U)CurHugeBufDataAddr + DEN5_FIX_FIRE_INTERVAL*EPSON_DATA_PER_FIRE),EPSON_DATA_PER_FIRE);
		}

		CurHugeBufDataAddr = (INT16U *)((INT32U)CurHugeBufDataAddr + (DEN5_FIX_FIRE + DEN5_FIX_FIRE_INTERVAL)*EPSON_DATA_PER_FIRE);
		if ((INT32U)CurHugeBufDataAddr >= HUGEBUF_END_ADDR)
			CurHugeBufDataAddr = (INT16U *)((INT32U)CurHugeBufDataAddr - (HUGEBUF_END_ADDR - HUGEBUF_START_ADDR));
		Cur_Band_Size -= (DEN5_FIX_FIRE + DEN5_FIX_FIRE_INTERVAL)*EPSON_DATA_PER_FIRE;
	}
}
void PushVSDCMD(INT8U b_first_pass)
{
#if defined(HEAD_EPSON_GEN5) && defined(SIM_EPSON_MACHINE)
	if (factoryData.HeadType == HeadNo_Epson_Gen5)
	{
		INT32U currentFireCount = 0;

		if(b_first_pass)
		{
			currentFireCount += 1;
			FPGA_SET_DATASEG(currentFireCount); //prefill print data.
#ifdef TEST_F_CMD
			FPGA_SET_DATASEG(TESTED_F_CMD);
#else
			switch(job_info.VSDModel)
			{
			case 1: //VSD1
				FPGA_SET_DATASEG(10);
				break;
			case 2: //VSD2
				FPGA_SET_DATASEG(12);
				break;
			default:
			case 3: //VSD3
				FPGA_SET_DATASEG(13);
				break;
			case 4: //VSD4
				FPGA_SET_DATASEG(14);
				break;
			}
#endif							
		}
		currentFireCount += 1;
		FPGA_SET_DATASEG(currentFireCount); //up to middle-voltage.
#ifdef TEST_F_CMD
		FPGA_SET_DATASEG(TESTED_F_CMD);
#else							
		FPGA_SET_DATASEG(1);
#endif						
		if(job_info.VSDModel == 2)
		{
			currentFireCount += EPSON_PREPRINT_SHAKE_NUM;
			FPGA_SET_DATASEG(currentFireCount); //shake.
#ifdef TEST_F_CMD
			FPGA_SET_DATASEG(TESTED_F_CMD);
#else							
			FPGA_SET_DATASEG(3);
#endif							
		}
		else
		{
			currentFireCount += EPSON_PREPRINT_SHAKE_NUM*2;
			FPGA_SET_DATASEG(currentFireCount); //shake.
#ifdef TEST_F_CMD
			FPGA_SET_DATASEG(TESTED_F_CMD);
#else							
			FPGA_SET_DATASEG(11);
#endif							
		}
		if(data_header.Inf.band_inf.band_width/job_info.encoder_div % 2  == 1)
			currentFireCount += (EPSON_MIN_DATA_UNIT - 1) + data_header.Inf.band_inf.band_width/job_info.encoder_div;
		else
			currentFireCount += EPSON_MIN_DATA_UNIT + data_header.Inf.band_inf.band_width/job_info.encoder_div;
		INT32U cur = currentFireCount ;
			while(cur > 0)//回绕发生后的第一次需要填2个坐标
			{
				if(cur > 0x10000)
				{
					FPGA_SET_DATASEG(0); //normal printing.
					if(cur != currentFireCount)
						FPGA_SET_DATASEG(0); //normal printing.
					cur -= 0x10000;
				}
				else
				{
					FPGA_SET_DATASEG(cur); //normal printing.
					if(cur != currentFireCount)
						FPGA_SET_DATASEG(cur); //normal printing.
					cur = 0;
				}

#ifdef TEST_F_CMD
				FPGA_SET_DATASEG(TESTED_F_CMD);
#else							
				switch(job_info.VSDModel)
				{
				case 1: //VSD1
					FPGA_SET_DATASEG(4);
					break;
				case 2: //VSD2
					FPGA_SET_DATASEG(5);
					break;
				default:
				case 3: //VSD3
					FPGA_SET_DATASEG(6);
					break;
				case 4: //VSD4
					FPGA_SET_DATASEG(7);
					break;
				}
#endif		
			}

		currentFireCount += 1;
		FPGA_SET_DATASEG(currentFireCount); //down to 0x1E.
#ifdef TEST_F_CMD
		FPGA_SET_DATASEG(TESTED_F_CMD);
#else							
		FPGA_SET_DATASEG(2);
#endif						
		currentFireCount += 1;
		FPGA_SET_DATASEG(currentFireCount); //begin shake between pass.
#ifdef TEST_F_CMD
		FPGA_SET_DATASEG(TESTED_F_CMD | FPGA_LAST_DATASEG);
#else						
		FPGA_SET_DATASEG(9 | FPGA_LAST_DATASEG);
#endif						
		//        currentFireCount += 1;
		//        FPGA_SET_DATASEG(currentFireCount); //only for safe.
		//        FPGA_SET_DATASEG(0);
	}
#endif				
}

void SetFPGAReg(INT32U PreFireNum, INT32U PostFireNum)
{
#if defined(TEST_FIRE_BY_FPGA) || ( defined(FPGA_MAINTAIN_COOR) || (defined(SUPPORT_MOTOR_CONTROL)))
	{
		OS_CPU_SR cpu_sr;
		INT32U reg_ctrl;
		INT32S startPoint, endPoint;
		switch(job_info.encoder_div)
		{
		case 1:
			reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_NONE;
			break;
		case 2:
			reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_HALF;
			break;
		case 3:
			reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_THREE;
			break;
		case 4:
			reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_QUAD;
			break;
		}
		switch(data_header.Inf.band_inf.band_X % job_info.encoder_div)
		{
		case 0:
			reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_0;
			break;
		case 1:
			reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_1;
			break;
		case 2:
			reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_2;
			break;
		case 3:
			reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_3;
			break;
		}
		if( data_header.Inf.band_inf.dir == 1)
			reg_ctrl |= ER_CoorCtrl_EN_DIR_POS;
		else
			reg_ctrl |= ER_CoorCtrl_EN_DIR_NEG;
		switch(job_info.VSDModel)
		{
		case 1:
			reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD1;
			break;
		case 2:
			reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD2;
			break;
		case 3:
			reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD3;
			break;
		case 4:
			reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD4;
			break;
		}
		switch(job_info.baseDPI)
		{
			//                        case 2:
			//                            reg_ctrl &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			//                            reg_ctrl |= (ER_CoorCtrl_SMOOTH_DIVIDER_1440 | ER_CoorCtrl_SMOOTH_MULTI_1440);
			//                            break;
		case 0:
		default:
			reg_ctrl &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			reg_ctrl |= (ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720);
			break;
		case 1:
			reg_ctrl &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			reg_ctrl |= (ER_CoorCtrl_SMOOTH_DIVIDER_540 | ER_CoorCtrl_SMOOTH_MULTI_540);
			break;
		}
		switch(job_info.baseDPI)
		{
		case 1:
			startPoint = data_header.Inf.band_inf.band_X + printer.platSpace * 540 / 720 + XORIGIN_OFFSET;	
			endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width -
				job_info.encoder_div + printer.platSpace * 540 / 720 + XORIGIN_OFFSET;
			break;
		case 2:
			startPoint = data_header.Inf.band_inf.band_X + printer.platSpace * 2 + XORIGIN_OFFSET;	
			endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width -
				job_info.encoder_div + printer.platSpace * 2 + XORIGIN_OFFSET;
			break;
		default:
			startPoint = data_header.Inf.band_inf.band_X + printer.platSpace + XORIGIN_OFFSET;	
			endPoint = data_header.Inf.band_inf.band_X + data_header.Inf.band_inf.band_width -
				job_info.encoder_div + printer.platSpace + XORIGIN_OFFSET;
			break;
		}
		if( data_header.Inf.band_inf.dir == 1)
		{
			startPoint += g_adjustInf.bidirectionValue;	
			endPoint += g_adjustInf.bidirectionValue;
		}
		if( data_header.Inf.band_inf.dir == 1)
		{
			startPoint -= PreFireNum *job_info.encoder_div;	
			endPoint += PostFireNum *job_info.encoder_div;
		}
		else
		{
			startPoint -= PostFireNum *job_info.encoder_div;	
			endPoint += PreFireNum *job_info.encoder_div;
		}

		OS_ENTER_CRITICAL();
#ifdef HEAD_EPSON_GEN5
		SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, reg_ctrl | ER_CoorCtrl_FIRE);
		SetEpsonRegInt(EPSON_REGADDR_X_FIRE_START, startPoint);
		SetEpsonRegInt(EPSON_REGADDR_X_FIRE_END, endPoint);
#elif defined(HEAD_RICOH_G4)
#ifdef ONE_BIT_DATA
		if(RIPSTAR_PRT_BIT == 2)
			reg_ctrl |= ER_CoorCtrl_SINGLE_DOT_MODE;
#endif		
		if(job_info.VSDModel == 3)
			rFPGA_RICOH_COORCTRL_L = (INT16U)(reg_ctrl | ER_CoorCtrl_FIRE);
		else
			rFPGA_RICOH_COORCTRL_L = (INT16U)(reg_ctrl | ER_CoorCtrl_FIRE | ER_CoorCtrl_CHECK_FREQ);
		rFPGA_RICOH_COORCTRLSTAT_H = (INT16U)(reg_ctrl >> 16);
		rFPGA_RICOH_FIRESTART_L	= (INT16U)startPoint;
		rFPGA_RICOH_FIREEND_L = (INT16U)endPoint;
		rFPGA_RICOH_FIRESTART_END_H = (INT16U)(((startPoint>>16)&0xFF) | ((endPoint>>8)&0xFF00));
#endif
		OS_EXIT_CRITICAL();
#if defined(TEST_FIRE_BY_FPGA)
		if( data_header.Inf.band_inf.dir == 1)
		{
			INT16U mask, bit;

			bit = ((INT16U)5)<<(0*4);
			mask = (0xF)<<(0*4);

			OS_ENTER_CRITICAL();
			rFPGA_EPSON_RegAddr = EPSON_REGADDR_X_POSI_0;
			rFPGA_EPSON_RegPort = endPoint;
			rFPGA_EPSON_RegAddr = EPSON_REGADDR_POSI_CTRL;
			rFPGA_EPSON_RegPortL &= ~mask;
			rFPGA_EPSON_RegPortL |= bit;
			OS_EXIT_CRITICAL();
		}
		else
		{
			INT16U mask, bit;

			bit = ((INT16U)9)<<(0*4);
			mask = (0xF)<<(0*4);

			OS_ENTER_CRITICAL();
			rFPGA_EPSON_RegAddr = EPSON_REGADDR_X_POSI_0;
			rFPGA_EPSON_RegPort = startPoint;
			rFPGA_EPSON_RegAddr = EPSON_REGADDR_POSI_CTRL;
			rFPGA_EPSON_RegPortL &= ~mask;
			rFPGA_EPSON_RegPortL |= bit;
			OS_EXIT_CRITICAL();
		}
		ConfigFPGAPosiIT(0, FPGAPosi_Test_IntHandler, NULL);
#endif
	}

#endif
}

void BeginFPGABand(INT32U PreFireNum, INT32U PostFireNum, INT8U b_first_pass)
{
#if defined(HEAD_EPSON_GEN5) && (defined(SUPPORT_MOTOR_CONTROL) || defined(FPGA_MAINTAIN_COOR) || defined(EPSON_PRT_FLASH))
	if(IsDelaySendData())
	{
		DelaySendData(PreFireNum, PostFireNum, b_first_pass);
	}
	else
	{
#endif

#if defined(HEAD_EPSON_GEN5)
		FPGA_SendData(True, PreFireNum); //Begin send data to FPGA FIFO
#else
		FPGA_SendData(True); //Begin send data to FPGA FIFO
#endif					

		while (g_FPGADMA_sendPkg < 3)
			OSTimeDly(2);			

		SetFPGAReg(PreFireNum, PostFireNum);

		PushVSDCMD(b_first_pass);   

#ifndef HEAD_RICOH_G4
		FPGA_BEGIN_BAND();   //FPGA transfer first data to print head
#endif

#if defined(HEAD_EPSON_GEN5) && (defined(SUPPORT_MOTOR_CONTROL) || defined(FPGA_MAINTAIN_COOR) || defined(EPSON_PRT_FLASH))
	}
#endif
}
#ifdef MICOLOR_AUTOFUCTION
INT8U Media_Lack = False;
INT8U Cover_up = False;
INT8U Media_Not_Fixed = False;
void Check_Print_Autofuction(void)
{
	if(IsOutOfMedia() && LCDMenuConfig.AutoLackMediaCheck)
		Media_Lack = True;
	else
		Media_Lack = False;

	if(LCDMenuConfig.AutoCoverCheck && !IsCoverSensorValid())
		Cover_up = True;
	else
		Cover_up = False;

	if(LCDMenuConfig.AutoFixCheck && !IsMediaFixed())
		Media_Not_Fixed = True;
	else
		Media_Not_Fixed = False;
}
#endif
INT8U BE_FLASH = False;
void wait_cancel_Resume(void)
{
	INT8U err;
	OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PRINT, OS_FLAG_WAIT_SET_ALL, &err);
	if (err == OS_NO_ERR)
	{								
		OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL, &err); //20081103
		if (err == OS_NO_ERR)
		{	
			OS_CPU_SR  cpu_sr;	
			status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR);	
			OS_ENTER_CRITICAL();
			status_ReportStatus(STATUS_MOVING, STATUS_CLR);		
#ifdef UV_PRINTER								
			uv_PrintPause();
#endif	

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
#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
			RegPostCapping(); //waiting action completed
#endif

#endif

			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
			status_ReportStatus(STATUS_PAUSE, STATUS_SET);
			OS_EXIT_CRITICAL();	

		}
		OSFlagPend(status_FLAG_GRP, CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err);

		while(OSFlagPend(status_FLAG_GRP, STATUS_PAUSE|CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL,10, &err),err != OS_NO_ERR)
		{
			if(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,&err))
			{
				err = OS_TASK_NOT_EXIST;
				break;
			}
		}
		if(OS_TASK_NOT_EXIST != err)
		{
			BE_FLASH = True;
		}
	}
}
INT8U MediaType = 0;

#ifdef EPSON_FLASH_IDLE
extern void motion_wipe_split(INT8U Wipe_Speed,INT8U Wipe_Times,INT32S Wiper_Pos,
		INT32S Z_WipePos,INT32S X_WipePos_Start,INT32S X_Wipe_Distance,INT8U head_num);
extern void WIPE_TATE_RICOH(INT8U Wipe_Speed,INT8U Wipe_Times,INT32S Wiper_Pos,
		INT32S Z_WipePos,INT32S X_WipePos_Start);
extern INT8U flash_idle_on;
#endif

#if defined(EPSON_FLASH_IDLE)&&defined(EPSON_CLEAN_UPDOWN)
void flash_idle_wiper(void)
{	
	INT8U cmdbuf[32],i,err;
	struct CleanParaEpsonAllwin_Factory ActiveFactory;
	struct CleanParaEpsonAllwin_Config ActiveConfig;
	struct CleanParaEpsonAllwin_Runtime ActiveRuntime;
	INT8U bAutoCleanInPrint = False;
	INT16U Z_FlashPos = 0;
	INT8U Allwin_ActivedConfig = 0;

	status_ReportStatus(STATUS_CLEANING, STATUS_SET);

	OSSemPend(CleaningParamSem, 0, &err);
	memcpy(&ActiveFactory, &cleanparam_EPSON_ALLWIN.factory, sizeof(struct CleanParaEpsonAllwin_Factory));
	memcpy(&ActiveRuntime, &cleanparam_EPSON_ALLWIN.Runtime, sizeof(struct CleanParaEpsonAllwin_Runtime));
	memcpy(&ActiveConfig, &cleanparam_EPSON_ALLWIN.Config[2], sizeof(struct CleanParaEpsonAllwin_Config));
	OSSemPost(CleaningParamSem);

	for(i =0; i<2; i++)
	{
		if(1)
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
			cmdbuf[0] = 20; //Length						
			cmdbuf[1] = UART_STARTSWIPE_CMD; 
			cmdbuf[2] = ActiveConfig.Carriage_X_Wipe_Speed; 
			cmdbuf[3] = ActiveConfig.WipeTimes; 
			*((__packed INT32S *)&cmdbuf[4]) =  ActiveFactory.WiperPos_Y[i];
			*((__packed INT32S *)&cmdbuf[8]) =  ActiveFactory.HeadBox_Z_WipePos;
			*((__packed INT32S *)&cmdbuf[12]) =  ActiveFactory.Carriage_X_WipePos_Start[i];
			*((__packed INT32S *)&cmdbuf[16]) =  (ActiveFactory.Carriage_X_WipePos_End[i] - 
					ActiveFactory.Carriage_X_WipePos_Start[i]);                            
			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
				OSTimeDly(100);			

			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
#endif    
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		    
			cmdbuf[0] = 7; //Length
			cmdbuf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&cmdbuf[2]) = 0;
			cmdbuf[6] = 7;

			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

		}
	}
	FPGA_START_FLASH_ALLWIN(1000,0xf);
	OSTimeDly(500);
	FPGA_STOP_FLASH();
	OSTimeDly(100);
	status_ReportStatus(STATUS_CLEANING, STATUS_CLR);
}
#endif		

#if defined(RICOH_3H)&&defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER)
void End_Prt_Do_Uv(void)
{
	INT8U err;
	INT8U cmdbuf[32];
	if(uv_PrintDir == 1 && cur_step_distance <= DYSS_BASE_STEP * CAL_PRINT_UV_Y_STEP_PERCENT)
	{
		INT32U HEAD23_OFFSET = 0;

		if(COLOR_NUMBER_CALIBRATION == 4)
			HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - factoryData.m_fHeadXColorSpace*X_BASE_RES;
		else 
			HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - 2*factoryData.m_fHeadXColorSpace*X_BASE_RES;

		if(data_header.Inf.band_inf.dir == 1)
		{
			print_Info.UV_MOVE_End =print_Info.PrintFire_Start - HEAD23_OFFSET - UV_LIGHT;
			print_Info.UV_MOVE_Start = print_Info.PrintFire_End - UV_LIGHT_HEAD0_OFFSET;
		}
		else
		{
			print_Info.UV_MOVE_End = print_Info.PrintFire_End - UV_LIGHT_HEAD0_OFFSET;
			print_Info.UV_MOVE_Start = print_Info.PrintFire_Start - HEAD23_OFFSET - UV_LIGHT;
		}
		DisableFPGAPosiIT(0);


		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

		status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
		cmdbuf[0] = 7; //Length
		cmdbuf[1] = UART_MOVETO_CMD;
		*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_Start;
		cmdbuf[6] = X_PRINT_UV_SPEED;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

		Front_Light_on();
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
		cmdbuf[0] = 7; //Length
		cmdbuf[1] = UART_MOVETO_CMD;
		*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_End;
		cmdbuf[6] = X_PRINT_UV_SPEED;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

		status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
		cmdbuf[0] = 7; //Length
		cmdbuf[1] = UART_MOVETO_CMD;
		*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_Start;
		cmdbuf[6] = X_PRINT_UV_SPEED;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
			OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

		Front_Light_off();

		EnableFPGAPosiIT(0);
	}
}
#endif

void Parser_Task(void* data)
{
	INT8U err;
	INT8U cmdbuf[32];
	INT8U blankBand = False;
	INT32U i;

	INT32U fpga_wrreq_num, fpga_rdreq_num;
	INT32U MAX_WHITE_TIME = 0;

	OS_CPU_SR cpu_sr;


	//	INT32U dspFWLen = DSP_FileLen;	
	//	INT8U * dspFWData = (INT8U *)&DSP_File;
	//	INT8U len;
	//	INT32U FPGA_Status = 0;

	INT32U dspFWLenSave, dspFWLen;	
	INT32U chksum = 0;
	INT32U addr;
	INT8U * dspFWData;
	INT8U len;
	INT8U oldPercent;
	INT8U ui;
	INT32U speed;


#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	INT32U PreFireNum = 0, PostFireNum = 0;
	OS_FLAGS flags;
	INT16U tmpu;
#endif
	INT8U b_first_pass = FALSE;
	INT8U b_UpdateDSP = False;

	while (1)
	{
		err = !OS_NO_ERR;
		//		CONSOL_Printf("1\r\n");
		BE_FLASH = False; 
		while (!HugeBuf_GetInfHead(&data_header))
		{
			wait_cancel_Resume();
			OSFlagPend(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,100,&err);
			if (err == OS_NO_ERR) //Cancel Job
			{
				//for 老化
				flags = OSFlagAccept(status_FLAG_GRP, CMD_EPSON_OPERATION, OS_FLAG_WAIT_SET_ANY,&err);
				if (flags & CMD_EPSON_OPERATION && CMD_Epson_Operate_Type == CMD_EPSON_T_MACHIME_TEST)
					continue;
				else
					break;
			}
		}
		//		CONSOL_Printf("2\r\n");
		if (err != OS_NO_ERR) //Not cancel
		{
			//CONSOL_Printf("Data Size %d\r\n", data_header.data_bytescnt);
			INT32U least_data = data_header.data_bytescnt;
#ifdef 	EPSON_4H					
			if (least_data > 120 * 1024 * 1024) 
				least_data = 120 *1024 * 1024;
#endif				
			while (chksum = HugeBuf_GetSize(), chksum < least_data)
			{
				//CONSOL_Printf("Buf Size %d\r\n", chksum);
				wait_cancel_Resume();
				OSFlagPend(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,100,&err);
				if (err == OS_NO_ERR) //Cancel Job
					break;
			}
		}
		//		CONSOL_Printf("3\r\n");
#ifdef FUNC_MAINTAIN
		if(bStatus_Maintain & 0x01)
		{
			UIAction_CancelPrint(0);
		}
		if ((err == OS_NO_ERR) ||(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,&err)) || (bStatus_Maintain & 0x01))//Cancel Job
#else
			if ((err == OS_NO_ERR) ||(OSFlagAccept(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,&err)))//Cancel Job
#endif
			{
				//			CONSOL_Printf("Job canceled!");
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
				if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					OSFlagPost(mix_FLAG_GRP, EPSON_CALI_CANCEL,OS_FLAG_SET,&err);
					OSFlagPend(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL, 0, &err);
					OSFlagPost(mix_FLAG_GRP, EPSON_CALI_CANCEL,OS_FLAG_CLR,&err);
				}
#endif			
#if defined(TEST_FIRE_BY_FPGA) || defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)
				OS_ENTER_CRITICAL();
#ifdef  HEAD_EPSON_GEN5
				SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL,
						~(ER_CoorCtrl_FIRE | ER_CoorCtrl_FLASH) & ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
#else
				tmpu = rFPGA_RICOH_COORCTRL_L;
				tmpu &= ~ER_CoorCtrl_FIRE;
				rFPGA_RICOH_COORCTRL_L = tmpu;
#endif
				OS_EXIT_CRITICAL();
#endif
				HugeBuf_CancelJob();
				status_ReportStatus(STATUS_PRINT, STATUS_CLR); //Also need clear some error.			
				continue;
			}		

		switch (data_header.header_type)
		{
#ifndef BYHX_WTITE_BOARD_TOOL		
			//Do not add varables here, otherwise it will cause compiler exit abnormal
		case JOB_START_HEAD:
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			flags = OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err);
			if(flags != EPSON_CALI_RUN && err != OS_NO_ERR && ((factoryData.m_nBitFlag & FDF_EPSON_LCD) == 0))
#endif			
				if(!security_CheckSWUILanguageSetting((INT8U)(data_header.Inf.job_inf.language&0xF), data_header.Inf.job_inf.languageStr, False))
					status_ReportStatus(STATUS_FTA_UILANGUAGE_NOT_MATCH,STATUS_SET);

			status_EP0Buf.percent = 0;	
			status_ReportStatus(CMD_PAUSE, STATUS_CLR);
			status_ReportStatus(STATUS_PAUSE, STATUS_CLR);	
			status_ReportStatus(STATUS_PRINT, STATUS_SET);	
#ifdef RIPSTAR_FLAT_EX
YZStat: 

			OSFlagPend(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,0,&err);
#if defined (RIPSTAR_FLAT_EX)&&(defined (MANUFACTURER_DYSS)||defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)||defined(FULGENCY_FUN))
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared,
			status_ReportStatus(STATUS_MOVING, STATUS_SET);			
			cmdbuf[0] = 8; //Length			
			cmdbuf[1] = UART_AXISMOVETO_CMD; 
			cmdbuf[2] = 4; //AXIS
			cmdbuf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;  
#ifdef FULGENCY_FUN			
			*((__packed INT32S *)&cmdbuf[4]) = Z_SAFE_POSITION; //move_distance means target position
#else
			*((__packed INT32S *)&cmdbuf[4]) = LCDMenuConfig_EX.OriginZ*Z_BASE_RES + Z_PLAT_LENGTH; //move_distance means target position
#endif			
			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
				OSTimeDly(10);	
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	

			OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
			if (err == OS_NO_ERR)
				goto YZStat;

#endif		
			if(flags == EPSON_CALI_RUN && !LCDMenuConfig_EX.PRINT_Y_CONTINUE) 
			{
				status_ReportStatus(STATUS_MOVING, STATUS_SET);			
				cmdbuf[0] = 8; //Length			
				cmdbuf[1] = UART_AXISMOVETO_CMD; 
				cmdbuf[2] = 2; //AXIS
				cmdbuf[3] = 4;  
				*((__packed INT32S *)&cmdbuf[4]) = LCDMenuConfig_EX.OriginY*DYSS_Y_PWM_RATIO + Y_PLAT_LENGTH; //move_distance means target position
				while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
					OSTimeDly(10);	
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	

				OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
				if (err == OS_NO_ERR)
					goto YZStat;
			}

#endif

#ifdef FULGENCY_FUN
			if(Y_GOHOME_Dirty >= Y_GOHOME_WAITE)
				UV_CTR_PRT_Y_HOME_CLEAR();
			if(Y_GOHOME_Dirty >= Y_GOHOME_IDLE)
				Y_GOHOME_Dirty = Y_NORMAL;
#endif
#ifdef COORD_NEW_UV
#if defined(MANUFACTURER_DYSS)||defined(ORIC_FUNCTION)
			uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24)/10;
#endif
			Configure_Uv_Power(uv_UVMode);
#endif

#ifdef MANUFACTURER_DYSS
			if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_AUTO)
				OpenPlateFan();
#endif

			if (data_header.data_bytescnt != 0)
			{
				status_ReportStatus(STATUS_FTA_INTERNAL_JOBSTARTHEADER, STATUS_SET);
				//Should seach next header here
				break;
			}
#ifdef INK_PASSWORD
			if(CheckInkVolume())
				break;
			FlushInkVolume(0, True);
#endif

#if defined (MICOLOR_AUTOFUCTION)
			if(LCDMenuConfig.AutoLoad && !IsLoadMediaSensorValid())
			{
				CMD_Epson_Operate_Type = CMD_EPSON_T_LOAD_MEDIA;
				status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
				OSFlagPend(status_FLAG_GRP, CMD_EPSON_OPERATION, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared,
			}
#endif
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared,

#ifdef EPSON_FLASH_IDLE
			if(flash_idle_on == True)
			{

				False_Idle_Off();
				cleanPara.flash = False;
#if defined(EPSON_CLEAN_UPDOWN)			
				//flash_idle_wiper();
#endif
			}
#endif

#if defined(HEAD_RICOH_G4)&&defined(RICOH_CLEAN_PRESS)
			False_Idle_Off();
#endif

			//Set printing status, it is different from band printing flag

			job_info.encoder_div = data_header.Inf.job_inf.encoder_div;
			job_info.dir_space = data_header.Inf.job_inf.dir_space;
			job_info.jobHeight = data_header.Inf.job_inf.jobHeight;
			job_info.speed = data_header.Inf.job_inf.speed;	
			job_info.blankBandDryTimeMS = data_header.Inf.job_inf.blankBandDryTimeMS;

			job_info.YDir = data_header.Inf.job_inf.YDir;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			job_info.VSDModel = data_header.Inf.job_inf.VSDModel;
			if(job_info.VSDModel == 0)
				job_info.VSDModel = CaliPrintSettingFromUSB.VSDModel; //for safe.
			job_info.baseDPI = data_header.Inf.job_inf.baseDPI;
			job_info.PassNumber = data_header.Inf.job_inf.PassNumber;
			job_info.displayResolution = data_header.Inf.job_inf.displayResolution;
			MediaType = LCDMenuConfig.SheetType;
#endif
#if defined(HEAD_RICOH_G4)&&defined(RICOH_VSD2) 
			if(job_info.VSDModel == 2)
			{
				job_info.VSDModel = 3;
#ifndef MANUFACTURER_DYSS			
				job_info.speed = 4;
#endif
			}
#endif			
			printedBands = 0;

#if defined(HEAD_RICOH_G4)&&defined(ONE_BIT_DATA)
			if(job_info.VSDModel == 3)
				RIPSTAR_PRT_BIT = 2;
			else
				RIPSTAR_PRT_BIT = 1;
#endif

#ifdef DYSS_1270
			if((job_info.VSDModel == 1 &&(job_info.encoder_div == 2 ||job_info.encoder_div == 4))||
					(job_info.VSDModel != 1 &&(job_info.encoder_div == 1 ||job_info.encoder_div == 2)))
			{}//is ok
			else
				status_ReportStatus(STATUS_FTA_DPI_NOT_SUPPORT, STATUS_SET);
#endif
			if (factoryData.HeadType == HeadNo_Epson_Gen5 ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L ||
					factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin )
			{
				cmdbuf[0] = 4;
				cmdbuf[1] = UART_HEAD_EPSON_JOBSTART;
				cmdbuf[2] = 0; //Normal Job
#ifdef CONVERSION_BOARD				
				if(RIPSTAR_PRT_BIT == 2)
					cmdbuf[3] = job_info.VSDModel|0x80;
				else
#endif					
					cmdbuf[3] = job_info.VSDModel;
				while (!UART_SendCMD(UART_HEAD_CHANNEL, cmdbuf))  //Change jetting condition
					OSTimeDly(10);
			}
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
			OSFlagPend(mix_FLAG_GRP_2, HEAD_WAVE_SET_FINISH, OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME, 10000, &err);
#endif
			//OSTimeDly(2000);
			//Send UART CMD to MOTION to inform encoder_div, speed 				
			cmdbuf[0] = 10; //Command length
#ifdef SUPPORT_MOTOR_CONTROL
			cmdbuf[0] += 1;
#endif
			cmdbuf[1] = UART_PRINTPARA_CMD;
#ifdef MANUFACTURER_DYSS
			if(job_info.encoder_div > 2)
				*((__packed INT32U *)&cmdbuf[2]) = 2;
			else
#endif  	    
				*((__packed INT32U *)&cmdbuf[2]) = job_info.encoder_div;

			//				if ( ((factoryData.HeadType == HeadNo_Konica_KM_512_L) || ( factoryData.HeadType == HeadNo_Konica_KM1024_L))
			//				   &&((fwInfo.mt_version & 0xFFFF) >= 0x0003)
			//				   &&(job_info.speed != PRINT_SPEED_OVER_HIGH)
			//				   &&((fwInfo.hd_version & 0xFFFFFF) >= 0x00020101) )
			//					*((__packed INT32U *)&cmdbuf[6]) = (job_info.speed | 0x80000000 | 0x25800); //速度增加600；原先为6K
			//				else
			*((__packed INT32U *)&cmdbuf[6]) = job_info.speed;

#ifdef SUPPORT_MOTOR_CONTROL
			*((__packed INT32U *)&cmdbuf[6]) = job_info.VSDModel;
			cmdbuf[10] = job_info.baseDPI;
#endif

			g_DSP_FireNumber = 0;

			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
				OSTimeDly(10);

			if ((fwInfo.mt_version & 0xFFFF) > 0x0001) //后16位为真正的版本号
			{
				cmdbuf[0] = 22;
				cmdbuf[1] = UART_DSP_SET_YDIR; //0x46
				cmdbuf[2] = job_info.YDir & 0xFF;
				for (i = 3; i< 22; i++)
					cmdbuf[i] = 0;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
					OSTimeDly(10);					
			}

			nextband_autoClean = False;

#ifndef HEAD_EPSON_GEN5				
			//#warning "ricoh need new implement for longflash_beforeprint"
			//				if (cleanPara.longflash_beforeprint) //每个作业开始打印前是否闪喷一次
			//					nextband_autoSpray = True;
			//				else
			//					nextband_autoSpray = False;
#endif

#ifdef GZ_KEY_USED			
#define TRY_TIME_GZ_KEY_SHAKEHAND 10
#define WAITING_TIME_GZ_KEY_SHAKEHAND  100 //ms
			//though PM send shakehand cmd before send Job head,
			// before USB Cmd handle task may handle it on time, Parser task receive this cmd.
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			{

			}
			else
			{
				for(i=0; i<TRY_TIME_GZ_KEY_SHAKEHAND; i++)
				{
					if (g_shakehandPassed == False)
						OSTimeDly(WAITING_TIME_GZ_KEY_SHAKEHAND);					
					else
						break;
				}
				if(i==TRY_TIME_GZ_KEY_SHAKEHAND)
					status_ReportStatus(STATUS_FTA_SHAKEHAND_ERR, STATUS_SET);
			}
#endif
			debug_int2 = 0;

			b_first_pass = TRUE;

#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
			LCD_SetMainInfoEx(E_IT_PassNum, (void*)&job_info.PassNumber);
			LCD_SetMainInfoEx(E_IT_Resolution, (void*)&job_info.displayResolution);
#endif

#if defined(HEAD_RICOH_G4)&&!defined(MANUFACTURER_DYSS)
			headBusUsedByPrintData = True;
			while (printerFlashing)
				OSTimeDly(1); //Wait FPGA out of flash state
			for( i=0; i<shakeInfo_Ricoh.Large_shake_1_unit_count; i++)
			{
				rFPGA_COMMAND = rFPGA_COMMAND_BGN_LSHAKE_1;
				OSTimeDly(25);
			}
			OSTimeDly(90);
			for( i=0; i<shakeInfo_Ricoh.Large_shake_0_unit_count; i++)
			{
				rFPGA_COMMAND = rFPGA_COMMAND_BGN_LSHAKE_0;
				OSTimeDly(25);
			}
#endif

#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
			UnCapping(); //waiting action completed
#endif
			//#if (defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1))
			//                SlowLeaveXOrigin(); //waiting action completed
			//#endif
#if defined (DGEN_AUTOFUCTION)
			if((OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)&&LCDMenuConfig_EX.Color_Bar_Flash_On)
				printer.platSpace = PRINTER_PLAT_SPACE - LCDMenuConfig_EX.Color_Bar_POSI*X_BASE_RES;
			else
				printer.platSpace = PRINTER_PLAT_SPACE;	
#endif

#ifdef FULGENCY_FUN
			UV_CTR_PRT_START_SET();
			UV_CTR_PRT_FINISH_CLEAR();
#endif
#ifdef ALLWIN_EPSON_SAME
			if(pause_cmd_from_dsp == 3)
			{
				status_ReportStatus(CMD_PAUSE, STATUS_SET);
				status_ReportStatus(STATUS_PAUSE, STATUS_SET);	
			}
#endif
			break;
		case BAND_HEAD:			

			g_FPGA_FireNumber = 0;
			g_DSP_FireNumber = 0;

			g_FPGADMA_sendPkg = 0;

			debug_int2 ++;

			PreFireNum = EPSON_PRE_EXTRA_FIRE_NUM;
			PostFireNum = EPSON_POST_EXTRA_FIRE_NUM;
			if (data_header.Inf.band_inf.compress_data_bytescnt != 0)
			{
				curBandByteLeft = data_header.Inf.band_inf.compress_data_bytescnt;
				FireCount = data_header.Inf.band_inf.band_width / job_info.encoder_div;
				FireCountEx = data_header.Inf.band_inf.band_width / job_info.encoder_div + PreFireNum+PostFireNum;
			}
			else
			{
				PreFireNum = 0;
				PostFireNum = 0;
				curBandByteLeft = 0;
				FireCount = 0;
				FireCountEx = 0;
			}

			if (curBandByteLeft == 0)
				blankBand = True;
			else
				blankBand = False;


#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			if (!blankBand && b_first_pass)
			{
				if(LCDMenuConfig.AutoStep)
				{
					status_ReportStatus(STATUS_MOVING, STATUS_SET);	

					cmdbuf[0] = 8; //Length
					cmdbuf[1] = UART_MOVEDISTANCE_CMD;
					cmdbuf[2] = 3;
					cmdbuf[3] = printer.feedSpeed;
					*((__packed INT32S *)&cmdbuf[4]) = data_header.Inf.band_inf.step_distance;

					while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
						OSTimeDly(10);	

					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				}
			}
#if defined(EPSON_CLEAN_UPDOWN)||defined(DYSS_UV_CLEAN)
			if (nextband_autoClean)
			{
				ActiveCleanConfig = cleanparam_EPSON_ALLWIN.AutoCleanConfig;
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
			}
#elif defined(MICOLOR_AUTOFUCTION)
			Check_Print_Autofuction();
			if(Media_Lack || Cover_up ||Media_Not_Fixed)
			{
				if(OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE|CMD_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err))
				{
					//directly send go home. else will delay one pass.
					move_dir = 2;
					move_distance = 0;
					move_type = 0;
#ifdef UV_PRINTER								
					uv_PrintPause();
#endif						
					status_ReportStatus(CMD_START_MOVE, STATUS_SET);
					status_ReportStatus(STATUS_PAUSE, STATUS_SET);
					OSFlagPend(status_FLAG_GRP, CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err);
				}
			}
#if defined(MANUFACTURER_MICOLOR_EPSON) || defined(MANUFACTURER_COLORFUL)
			if (nextband_autoClean)
			{
				ActiveCleanConfig = cleanparam_EPSON_MICOLOR.AutoCleanConfig;
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
			}
#endif		  
#endif
#else
			if ((nextband_autoClean) || (nextband_autoSpray))
				status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
#endif
			//				CONSOL_Printf("4\r\n");
			//				CONSOL_Printf("FLAG = 0x%x\r\n", status_FLAG_GRP->OSFlagFlags);
CheckStat:           

			OSFlagAccept(status_FLAG_GRP, STATUS_PAUSE, OS_FLAG_WAIT_SET_ALL, &err); //20081103

#ifdef HEAD_RICOH_G4		
			INT8U PAUSE_DYSS = False;
#endif

			if (err == OS_NO_ERR)
			{
				BE_FLASH = True;
#ifdef HEAD_RICOH_G4
				PAUSE_DYSS	= True;
				DisableFPGAPosiIT(0);
#endif			
			}
#ifdef HEAD_RICOH_G4
			else
				PAUSE_DYSS	= False;
#endif		


			OSFlagPend(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,0,&err);
#ifdef HEAD_RICOH_G4
			if(PAUSE_DYSS == True)
				EnableFPGAPosiIT(0);
#endif		
			//Waiting all error/init/clean/pause cleared, 	
			//				CONSOL_Printf("5\r\n");				

			if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_CANCEL, OS_FLAG_WAIT_CLR_ALL,&err))//Cancel Job
			{
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
				if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				{
					OSFlagPost(mix_FLAG_GRP, EPSON_CALI_CANCEL,OS_FLAG_SET,&err);
					OSFlagPend(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL, 0, &err);
					OSFlagPost(mix_FLAG_GRP, EPSON_CALI_CANCEL,OS_FLAG_CLR,&err);
				}
#endif		
				status_ReportStatus(CMD_PAUSE, STATUS_CLR);
				status_ReportStatus(STATUS_PAUSE, STATUS_CLR);	
				HugeBuf_CancelJob();
				status_ReportStatus(STATUS_PRINT, STATUS_CLR); //Also need clear some error.			
				break;
			}
#ifdef MICOLOR_AUTOFUCTION
			Check_Print_Autofuction();
			if(Media_Lack || Cover_up ||Media_Not_Fixed)
			{
				//directly send go home. else will delay one pass.
				move_dir = 2;
				move_distance = 0;
				move_type = 0;
#ifdef UV_PRINTER								
				uv_PrintPause();
#endif						
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				status_ReportStatus(STATUS_PAUSE, STATUS_SET);
				OSFlagPend(status_FLAG_GRP, CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err);
				goto CheckStat;
			}
#endif

#if defined (EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
			if(OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				UnCapping(); //waiting action completed
			OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
			if (err == OS_NO_ERR)
				goto CheckStat;
#endif

			//#if defined (RIPSTAR_FLAT_EX)&&defined (MANUFACTURER_DYSS)
#if 0
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared,
			status_ReportStatus(STATUS_MOVING, STATUS_SET);			
			cmdbuf[0] = 8; //Length			
			cmdbuf[1] = UART_AXISMOVETO_CMD; 
			cmdbuf[2] = 4; //AXIS
			cmdbuf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;  
			*((__packed INT32S *)&cmdbuf[4]) = LCDMenuConfig_EX.OriginZ*Z_BASE_RES ; //move_distance means target position
			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
				OSTimeDly(10);	
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
			OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
			if (err == OS_NO_ERR)
				goto CheckStat;		
#endif

#if (defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3))
			//check if current position is near origin or not.
			if (!blankBand)
				SlowLeaveXOrigin(); //waiting action completed
			OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
			if (err == OS_NO_ERR)
				goto CheckStat;
#ifdef MICOLOR_AUTOFUCTION
			Check_Print_Autofuction();
			if(Media_Lack || Cover_up ||Media_Not_Fixed)
			{
				//directly send go home. else will delay one pass.
				move_dir = 2;
				move_distance = 0;
				move_type = 0;
#ifdef UV_PRINTER								
				uv_PrintPause();
#endif						
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				status_ReportStatus(STATUS_PAUSE, STATUS_SET);
				OSFlagPend(status_FLAG_GRP, CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err);
				goto CheckStat;
			}
#endif
#endif

#if defined(EPSON_CLEAN_UPDOWN)||defined(EPSON_CLEAN_INTEGRATE_FLASH_UNION)          
			if((LCDMenuConfig.cfg_bit&AUTO_FLASH_BEFORE_PRINT) && (BE_FLASH == True || b_first_pass == True))
				Flash_Before_Print();
#endif  

#if defined(SUPPORT_MOTOR_CONTROL)
			if(GenerateMotionPrePrintCMD(cmdbuf, PreFireNum, PostFireNum))
			{
				status_ReportStatus(STATUS_MOVING, STATUS_SET);	 //
				while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
					OSTimeDly(20);
				if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,30000,&err)) //20090214
				{
					status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
					CONSOL_Printf("!!!!!!\r\n");
					HugeBuf_Flush(curBandByteLeft);
					break;					
				}
				else
				{
					OSFlagAccept(status_FLAG_GRP, CMD_PAUSE|STATUS_PAUSE, OS_FLAG_WAIT_SET_ANY, &err);
					if (err == OS_NO_ERR)
						goto CheckStat;
#ifdef MICOLOR_AUTOFUCTION
					Check_Print_Autofuction();
					if(Media_Lack || Cover_up ||Media_Not_Fixed)
					{
						//directly send go home. else will delay one pass.
						move_dir = 2;
						move_distance = 0;
						move_type = 0;
#ifdef UV_PRINTER
						uv_PrintPause();
#endif						
						status_ReportStatus(CMD_START_MOVE, STATUS_SET);
						status_ReportStatus(STATUS_PAUSE, STATUS_SET);
						OSFlagPend(status_FLAG_GRP, CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err);
						goto CheckStat;
					}
#endif
				}
			}
			else
			{
				CONSOL_Printf("!!!!!!\r\n");
				HugeBuf_Flush(curBandByteLeft);
				break;					
			}
#elif defined(EPSON_PRT_FLASH)
			PRT_FLASH_PrePrint(PreFireNum, PostFireNum);
#endif

#ifndef HEAD_RICOH_G4

			cmdbuf[0] = 6;
			cmdbuf[1] = UART_HEAD_EPSON_PASSSTART;
			cmdbuf[2] = (INT8U)FireCount ;
			cmdbuf[3] = (INT8U)(FireCount>>8);
			cmdbuf[4] = (INT8U)FireCountEx;
			cmdbuf[5] = (INT8U)(FireCountEx>>8);
			UART_SendCMD(UART_HEAD_CHANNEL, cmdbuf);
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			if (!GenerateMotionPrintCMD(cmdbuf, blankBand, PreFireNum, PostFireNum))
#else
				if (!GenerateMotionPrintCMD(cmdbuf, blankBand))
#endif				
				{
					CONSOL_Printf("!!!!!!\r\n");
					HugeBuf_Flush(curBandByteLeft);
					break;					
				}

			//Set moving status, main task will clear this status after
			//it receive moving complete COM command, or motion error	

			status_ReportStatus(STATUS_MOVING, STATUS_SET);	 //
			if (blankBand)
				status_ReportStatus(STATUS_MOVING_FLASH, STATUS_SET);
			else
				status_ReportStatus(STATUS_MOVING_FLASH, STATUS_CLR); //Just for safety

			if (!blankBand)
			{
#ifndef HEAD_RICOH_G4
				//for ricoh, between two pass, there are small shake already. FPGA can't support flash at the same time.
				//Maybe, when coming in pause, we ought to close small shake and start flash.
				headBusUsedByPrintData = True;
				//FPGA_STOP_FLASH();   //Stop flashing
				//OSTimeDly(10);  //Can not delete this delay
				while (printerFlashing)
					OSTimeDly(1); //Wait FPGA out of flash state
#endif

				//BeginFPGABand(PreFireNum, PostFireNum, b_first_pass);///
				// always send prefill data to fix spray line bugs
				BeginFPGABand(PreFireNum, PostFireNum, True);///

#if (defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3))
				if( !cleanparam_EPSON_MICOLOR.bAutoSpray) //if bAutoSpray is true, the begin of every pass must add a prefill command.
#endif
					b_first_pass = FALSE;
			}

			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
				OSTimeDly(20);

			if (blankBand)
				OSTimeDly(job_info.blankBandDryTimeMS);

#ifdef SUPPORT_MOTOR_CONTROL
			MAX_WHITE_TIME = data_header.Inf.band_inf.step_distance/motionParam.axis_param[AXIS_ID_Y -1].accParam[MAX_Y_SPEED_LEVEL - 1].EndFreq*1000 + 5000;
#else
			MAX_WHITE_TIME = data_header.Inf.band_inf.step_distance/20 + 5000;
#endif

			//				OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,0,&err); //Waiting moving flag cleared, 
			if(MAX_WHITE_TIME >= 30000)
			{
				err = OS_TIMEOUT;
				while(MAX_WHITE_TIME > 65000)
				{
					if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,65000,&err)) //20090214
					{
						if(err == OS_TIMEOUT)
							MAX_WHITE_TIME -= 65000;
					}
					else
					{
						err = OS_NO_ERR;
						break;
					}
				}
				if(err == OS_TIMEOUT)
				{
					if(MAX_WHITE_TIME != 0)
					{
						if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,MAX_WHITE_TIME,&err)) //20090214
							status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
					}
					else
					{
						status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
					}
				}

			}
			else
			{
				if (!OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ANY,30000,&err)) //20090214
					status_ReportStatus(STATUS_ERR_INTERNAL_MOVING_TIMEOUT, STATUS_SET);
			}

			//#ifdef SUPPORT_MOTOR_CONTROL
			//                OS_ENTER_CRITICAL();
			//                i = ReadEpsonRegShort(EPSON_REGADDR_COOR_STAT);
			//                OS_EXIT_CRITICAL();
			//
			//                if(i & ER_CoorStat_OverFreq)
			//					status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG1, STATUS_SET);
			//#endif

#ifdef HEAD_RICOH_G4            
			rFPGA_RICOH_COORCTRL_L &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			rFPGA_RICOH_COORCTRL_L |= (ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720);
			rFPGA_RICOH_COORCTRLSTAT_H = (rFPGA_RICOH_COORCTRLSTAT_H|0x8000);
#endif   

#if 0 
			if(job_info.PassNumber == 4)
				OSTimeDly(300);
			else if(job_info.PassNumber == 3)
				OSTimeDly(400);
			else if(job_info.PassNumber == 2)
				OSTimeDly(500);
#endif

#ifdef COORD_NEW_UV
#ifndef MANUFACTURER_TATE_EPSON_UV
			UV_POSI.STOP_UV = True;
			Front_Light_off();
			Back_Light_off();
#endif
#endif

#if defined(MANUFACTURER_DYSS) 
			LAST_PRT_Y_POSI =  curPos.y - Y_PLAT_LENGTH;
#endif

#if defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER) 
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
				if(cur_step_distance > DYSS_BASE_STEP * CAL_PRINT_UV_Y_STEP_PERCENT)
				{
					INT32U HEAD23_OFFSET = 0;
#ifdef RICOH_3H			 
					if(COLOR_NUMBER_CALIBRATION == 4)
						HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - factoryData.m_fHeadXColorSpace*X_BASE_RES;
					else 
						HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - 2*factoryData.m_fHeadXColorSpace*X_BASE_RES;
#else
					if(COLOR_NUMBER_CALIBRATION == 4)
						HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - 3*factoryData.m_fHeadXColorSpace*X_BASE_RES;
					else 
						HEAD23_OFFSET = UV_LIGHT_HEAD0_OFFSET - 5*factoryData.m_fHeadXColorSpace*X_BASE_RES;
#endif

					if(data_header.Inf.band_inf.dir == 1)
					{
						print_Info.UV_MOVE_End =print_Info.PrintFire_Start - HEAD23_OFFSET - UV_LIGHT;
						print_Info.UV_MOVE_Start = print_Info.PrintFire_End - UV_LIGHT_HEAD0_OFFSET;
					}
					else
					{
						print_Info.UV_MOVE_End = print_Info.PrintFire_End - UV_LIGHT_HEAD0_OFFSET;
						print_Info.UV_MOVE_Start = print_Info.PrintFire_Start - HEAD23_OFFSET - UV_LIGHT;
					}
					DisableFPGAPosiIT(0);


					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

					status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
					cmdbuf[0] = 7; //Length
					cmdbuf[1] = UART_MOVETO_CMD;
					*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_Start;
					cmdbuf[6] = X_PRINT_UV_SPEED;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

					Front_Light_on();
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
					cmdbuf[0] = 7; //Length
					cmdbuf[1] = UART_MOVETO_CMD;
					*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_End;
					cmdbuf[6] = X_PRINT_UV_SPEED;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

					status_ReportStatus(STATUS_MOVING, STATUS_SET);		      
					cmdbuf[0] = 7; //Length
					cmdbuf[1] = UART_MOVETO_CMD;
					*((__packed INT32U *)&cmdbuf[2]) = print_Info.UV_MOVE_Start;
					cmdbuf[6] = X_PRINT_UV_SPEED;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

					Front_Light_off();

					OSFlagPost(mix_FLAG_GRP_2,DYSS_UV_LIGHT_CAL,OS_FLAG_CLR,&err);			
					status_ReportStatus(STATUS_MOVING, STATUS_SET);		
					cmdbuf[0] = 8; //Length									
					cmdbuf[1] = UART_MOVEDISTANCE_CMD; 
					cmdbuf[2] = 3; //1,2 X, 3,4 Y, 5,6 Z
					cmdbuf[3] = Y_MOVE_UV_SPEED;
					*((__packed INT32S *)&cmdbuf[4]) = cur_step_distance;
					while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
						OSTimeDly(100);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop

					EnableFPGAPosiIT(0);
				}

#endif            
			//#ifdef Y_ENCODER_ENABLE
			OS_ENTER_CRITICAL();

			g_yEncoderInf.bandIndex = data_header.Inf.band_inf.bandIndex;
			g_yEncoderInf.encoderPos = curPos.y;
			g_yEncoderInf.bandY = data_header.Inf.band_inf.band_Y;
			g_yEncoderInf.nextBandY = data_header.Inf.band_inf.nextBandY;
			g_yEncoderInf.bandResY = data_header.Inf.band_inf.bandResY;
			g_yEncoderInf.fStepTotal += cur_step_distance;
			g_yEncoderInf.checksum = (INT32U)g_yEncoderInf.bandIndex;
			g_yEncoderInf.checksum ^=  (INT32U)g_yEncoderInf.encoderPos;
			g_yEncoderInf.checksum ^=  (INT32U)g_yEncoderInf.bandY;
			g_yEncoderInf.checksum ^=  (INT32U)g_yEncoderInf.nextBandY;
			g_yEncoderInf.checksum ^=  (INT32U)g_yEncoderInf.bandResY;
			g_yEncoderInf.checksum ^=  *((INT32U *)&(g_yEncoderInf.fStepTotal));

			OS_EXIT_CRITICAL();
			//#endif					
#if (defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3))
			if(data_header.Inf.band_inf.dir == 1)
				printedBands++;
#else
			printedBands++;
#endif

			status_EP0Buf.percent = (data_header.Inf.band_inf.band_Y*100/job_info.jobHeight+0.5);

			if (g_USBTransStoped)
			{
				g_USBTransStoped = False;
				HugeBuf_StartOneReceive();//Try to trigger a DMA1 transfer, if DMA1 working, this function will do nothing
			}	

			if (curBandByteLeft > 0)
			{
				CONSOL_Printf("Data too large: curBandByteLeft: %x, > 0\r\n", curBandByteLeft);
#ifndef HEAD_RICOH_G4
				CONSOL_Printf("FPGA info: Status %d, FireNum %d, ReadNumL %x, H %x, WriteNumL %x, H %x \r\n",
						rFPGA_STATUS, rFPGA_DBG_FireNum, rFPGA_DBG_ReadNum_L,
						rFPGA_DBG_ReadNum_H, rFPGA_DBG_WriteNum_L, rFPGA_DBG_WriteNum_H);
#endif
				status_ReportStatus(STATUS_ERR_INTERNAL_PRINTDATA, STATUS_SET);
			}
			HugeBuf_FlushCurBand();

			if (!blankBand)
			{
#ifndef HEAD_RICOH_G4
				FPGA_END_BAND();
				OSTimeDly(1);
#endif
				headBusUsedByPrintData = False;
			}

			//				CONSOL_Printf("6\r\n");
			//OSTimeDly(3000);
#ifdef INK_PASSWORD
			i = HandShakeInkDot(data_header.Inf.band_inf.dotCount);
			switch(i)
			{
			case INKDOT_CHK_NO_PASS:
			default:
				status_ReportStatus(STATUS_FTA_SWVERSION_ERR_INK_PWD, STATUS_SET);
				break;
			case INKDOT_CHK_OLD_SW:
				break;
			case INKDOT_CHK_PASS:
			case INKDOT_CHK_NO_LIMIT:						
				FlushInkVolume(data_header.Inf.band_inf.dotCount, False);
				break;
			}
#else
			//cheney: follow Tony suggest: 墨量统计对所有FW版本都有效。
			i = HandShakeInkDot(data_header.Inf.band_inf.dotCount);
			switch(i)
			{
			case INKDOT_CHK_NO_PASS:
			default:
			case INKDOT_CHK_OLD_SW:
				break;
			case INKDOT_CHK_PASS:
			case INKDOT_CHK_NO_LIMIT:						
				FlushInkVolume(data_header.Inf.band_inf.dotCount, False);
				break;
			}
#endif

			break;
		case JOB_END_HEAD:	
			status_EP0Buf.percent = 100;
#ifdef UV_PRINTER				
			uv_PrintEndJob();
#endif

#if defined(RICOH_3H)&&defined(MANUFACTURER_DYSS) &&defined(UV_PRINTER)
			End_Prt_Do_Uv();
#endif
#ifdef FULGENCY_FUN
			UV_CTR_PRT_START_CLEAR();
			UV_CTR_PRT_FINISH_SET();
#endif

#ifdef MANUFACTURER_DYSS
			if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_AUTO)
				ClosePlateFan();

			cmdbuf[0] = 2; //Length									
			cmdbuf[1] = UART_DSP_STOP_PRINT; 
			while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
				OSTimeDly(100);			
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#if defined(HEAD_RICOH_G4)
			if((rFPGA_STATUS & 0x7) == 4)
			{
				rFPGA_COMMAND = rFPGA_COMMAND_END_SSHAKE;
			}
			DisableFPGAPosiIT(0); 			
#endif
			if (factoryData.HeadType == HeadNo_Epson_Gen5 ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 ||
					factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L ||
					factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin)
			{
				cmdbuf[0] = 2;
				cmdbuf[1] = UART_HEAD_EPSON_JOBEND;
				while (!UART_SendCMD(UART_HEAD_CHANNEL, cmdbuf))
					OSTimeDly(10);				
			}
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			{
				OS_ENTER_CRITICAL();	
				USBDMA_manualStop = False;
				HugeBuf_Reset(); 	
				HugBuf_InitReceiveSize();
				HugeBuf_StartOneReceive();  	
				OS_EXIT_CRITICAL(); 	
			}
#if defined (MICOLOR_AUTOFUCTION)
			else
			{
				if(LCDMenuConfig.AutoCut)
				{
					CMD_Epson_Operate_Type = CMD_EPSON_T_SHEET_CUT;
					status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
					OSFlagPend(status_FLAG_GRP, CMD_EPSON_OPERATION, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
				}
			}
#endif
#endif			
#if defined(TEST_FIRE_BY_FPGA) || defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)
			OS_ENTER_CRITICAL();
#ifdef  HEAD_EPSON_GEN5
			SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, ~(ER_CoorCtrl_FIRE | ER_CoorCtrl_FLASH) & ReadEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
#else
			tmpu = rFPGA_RICOH_COORCTRL_L;
			tmpu &= ~ER_CoorCtrl_FIRE;
			rFPGA_RICOH_COORCTRL_L = tmpu;
#endif
			OS_EXIT_CRITICAL();
#endif

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
			status_ReportStatus(STATUS_WAR_OVER_SPEED_NOT_SUPPORT, STATUS_CLR);

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
#if defined (EPSON_CLEAN_UPDOWN) ||  defined(EPSON_CLEAN_INTEGRATE_3_CAP)
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

				OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving flag cleared, 
			}
#elif defined(FULGENCY_FUN)
			if(Y_GOHOME_Dirty == Y_GO_LIMIT)
			{
				move_dir = 3; //Go limit
				move_distance = 0;
				move_type = 0;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				OSTimeDly(10);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving flag cleared, 
			}
			else if(Y_GOHOME_Dirty == Y_GO_HOME)
			{
				move_dir = 4; //Go home
				move_distance = 0;
				move_type = 0;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				OSTimeDly(10);			
				OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving flag cleared, 
			}
			else 
			{	
				if(!LCDMenuConfig_EX.PRINT_Y_CONTINUE)
				{
					move_dir = 4; //Go home
					move_distance = 0;
					move_type = 0;
					status_ReportStatus(CMD_START_MOVE, STATUS_SET);
					OSTimeDly(10);			
					OSFlagPend(status_FLAG_GRP, STATUS_MOVING|CMD_START_MOVE, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving flag cleared, 
				}
			}
			Y_GOHOME_Dirty = Y_GOHOME_IDLE;
#endif

			g_shakehandPassed = False;

			status_ReportStatus(STATUS_PRINT, STATUS_CLR); //Also need clear some error.

#ifdef INK_PASSWORD
			FlushInkVolume(0, True);
			if(CheckInkVolume())
				break;
#else
			//cheney: follow Tony suggest: 墨量统计对所有FW版本都有效。
			FlushInkVolume(0, True);
#endif

#ifdef HEAD_RICOH_G4
			headBusUsedByPrintData = False;
#endif

			b_first_pass = FALSE;

			break;
#endif				
		case UPDATER_HEAD:

			b_UpdateDSP = False;

			OSFlagPend(status_FLAG_GRP,STATUS_UPDATING,OS_FLAG_CLR,0,&err);
			//				CONSOL_Printf("Begin updating...");
			status_EP0Buf.percent = 0;
			status_ReportStatus(STATUS_UPDATING, STATUS_SET);

			if (data_header.Inf.updater_inf.FWNum == 0)
			{
				status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
				goto end;				
			}				
			addr = data_header.Inf.updater_inf.SubAddressTable[0];
			for (ui = 1; ui < data_header.Inf.updater_inf.FWNum;ui++)
			{
				if (addr >= data_header.Inf.updater_inf.SubAddressTable[ui])
				{
					status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
					goto end;
				}
				else
					addr =  data_header.Inf.updater_inf.SubAddressTable[ui];

			}
			if (data_header.Inf.updater_inf.data_bytescnt == 0)
			{
				status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA,STATUS_SET);
				//					CONSOL_Printf("1\r\n");
				goto end;
			}

			addr = (INT32U)HugeBuf_GetCodeAddr(data_header.Inf.updater_inf.data_bytescnt);

			if (addr == 0)
			{
				status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA,STATUS_SET);
				//					CONSOL_Printf("2\r\n");
				goto end;
			}

			//				CONSOL_Printf("addr = %x, count = %d\r\n", addr, data_header.Inf.updater_inf.data_bytescnt);

			chksum = 0;
			//                                if(data_header.Inf.updater_inf.data_bytescnt%2 != 0)
			//                                {
			//                                  *((INT8U *)(addr + data_header.Inf.updater_inf.data_bytescnt)) = 0;
			//                                }

			for (i = 0; i < (data_header.Inf.updater_inf.data_bytescnt+1)/2; i++) //Here we check all data, not only ARM FW.
			{
				chksum +=  *((INT16U *)addr);
				addr  += sizeof(INT16U);
			}

			if (chksum != data_header.Inf.updater_inf.checksum)
			{
				status_ReportStatus(STATUS_UPDATEERR_CHECKSUM,STATUS_SET);
				//					CONSOL_Printf("Download data check sum error!");
				goto end;
			}

			addr = (INT32U)HugeBuf_GetCodeAddr(data_header.Inf.updater_inf.data_bytescnt);

#if defined(SUPPORT_MOTOR_CONTROL)
			OSFlagPost (mix_FLAG_GRP, MOTION_URGENT_STOP_CMD, OS_FLAG_SET, &err);
#if defined(EPSON_BOTTOM_BOARD_V3)
			Heater_Urgent_Stop();
#endif
#endif

			for (ui = 0; ui < data_header.Inf.updater_inf.FWNum; ui++)
			{
				switch(data_header.Inf.updater_inf.FWId[ui])
				{
				case 0x20: //Main FW
					//							CONSOL_Printf("Updating Main FW...\r\n");
					if (data_header.Inf.updater_inf.Fileflag != 'BYA2' )
					{
						status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
						goto end;				
					}
					if(data_header.Inf.updater_inf.FileVersion != 2)
					{
						status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
						goto end;				
					}

					/*XXX*/if(0)//if (!Flash_UpdateCode(&data_header.Inf.updater_inf, ui))
					{
						CONSOL_Printf("Updating Main FW Failed!\r\n");
						goto end;
					}
					break;
				case 0x23: //Main FPGA FW
					//							CONSOL_Printf("Updating Main FPGA FW...\r\n");
					if(data_header.Inf.updater_inf.FileVersion != 2)
					{
						status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
						goto end;				
					}
					//                            if(data_header.Inf.updater_inf.FileVersion != 2 && data_header.Inf.updater_inf.SubSizeTable[ui] != 0 )
					//                            {
					//                                addr = ((INT32U)HugeBuf_GetCodeAddr(data_header.Inf.updater_inf.data_bytescnt) +
					//                                        data_header.Inf.updater_inf.SubAddressTable[ui]);
					//                                EnSecurity((INT8U *)addr, (INT8U *)addr, data_header.Inf.updater_inf.SubSizeTable[ui]);
					//                            }
					/*XXX*/if(0)//if (!Flash_UpdateFPGA(&data_header.Inf.updater_inf, ui))
					{
						CONSOL_Printf("Updating Main FPGA FW Failed!\r\n");
						goto end;
					}
					break;
				case 0x21: //Motion FW
					//							CONSOL_Printf("Updating Motion FW...\r\n");						
					//                            if(data_header.Inf.updater_inf.FileVersion != 2)
					//                            {
					//                                status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
					//                                goto end;				
					//                            }
					if(data_header.Inf.updater_inf.FileVersion == 2 && data_header.Inf.updater_inf.SubSizeTable[ui] != 0 )
					{
						addr = ((INT32U)HugeBuf_GetCodeAddr(data_header.Inf.updater_inf.data_bytescnt) +
								data_header.Inf.updater_inf.SubAddressTable[ui]);
						DeSecurity((INT8U *)addr, (INT8U *)addr, data_header.Inf.updater_inf.SubSizeTable[ui]);
					}
					dspFWLenSave = dspFWLen = data_header.Inf.updater_inf.SubSizeTable[ui];
					dspFWData = (INT8U *)addr;//(INT8U *)(addr + data_header.Inf.updater_inf.SubAddressTable[ui]); copy zhb
					oldPercent = 0;

					if (dspFWLen != 0)
					{
						//								CONSOL_Printf("DSP FW Updating begin! Len = %d\r\n", dspFWLen);
						OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_SET, &err);

						cmdbuf[0] = 2;						
						cmdbuf[1] = UART_UPDATE_DSP_BEGIN;

						while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
							OSTimeDly(10);

						OSFlagPend (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_CLR, UPDATEDSP_WAITTIME, &err);
						if (err != OS_NO_ERR)
						{
							status_ReportStatus(STATUS_UPDATEERR_DSP_BEGIN_TIMEOUT,STATUS_SET);
							//									CONSOL_Printf("Updating motion timeout error!\r\n");
						}else
						{						
							while (dspFWLen != 0)
							{
								OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_SET, &err);						
								if (dspFWLen < 26)
									len = dspFWLen;
								else
									len = 26;
								cmdbuf[0] = len+2;
								cmdbuf[1] = UART_UPDATE_DSP_DATA;
								for (i = 0; i < len; i++)
									cmdbuf[i+2] = *dspFWData++;
								while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
									OSTimeDly(10);
								OSFlagPend (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_CLR, UPDATEDSP_WAITTIME, &err);
								if (err != OS_NO_ERR)
								{
									status_ReportStatus(STATUS_UPDATEERR_DSP_DATA_TIMEOUT,STATUS_SET);
									//											CONSOL_Printf("Updating motion timeout error!\r\n");
									break;
								}
								dspFWLen -= len;
								status_EP0Buf.percent = 100 * (ui+1)/data_header.Inf.updater_inf.FWNum - 100/data_header.Inf.updater_inf.FWNum*dspFWLen/dspFWLenSave;
								if (oldPercent != status_EP0Buf.percent)
								{
									oldPercent = status_EP0Buf.percent;
									//											CONSOL_Printf("%d%%\r\n", status_EP0Buf.percent);							
								}
							}						

							if (err == OS_NO_ERR)
							{							
								OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_SET, &err);						
								cmdbuf[0] = 2;
								cmdbuf[1] = UART_UPDATE_DSP_END;

								while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf))
									OSTimeDly(10);

								OSFlagPend (mix_FLAG_GRP, MISC_FLAGS_UPDATEDSP, OS_FLAG_CLR, UPDATEDSP_WAITTIME, &err);
								if (err != OS_NO_ERR)
								{
									status_ReportStatus(STATUS_UPDATEERR_DSP_END_TIMEOUT,STATUS_SET);
									//											CONSOL_Printf("Updating motion timeout error!\r\n");
								}else
								{
									//status_EP0Buf.percent = 100;
									//status_ReportStatus(STATUS_UPDATE_SUCCESS, STATUS_SET);
									//											CONSOL_Printf("DSP FW Update success!!");
									b_UpdateDSP = True;
								}
							}
						}
					}
					break;
				case 0x22: //Head FW
					//							CONSOL_Printf("Updating Head Board FW(do nothing)\r\n");	
					{
						INT32U nFWLenSave, nFWLen;	
						INT8U * pFWData;
						INT8U cmdBegin;
						if(data_header.Inf.updater_inf.FileVersion == 2 && data_header.Inf.updater_inf.SubSizeTable[ui] != 0 )
						{
							addr = ((INT32U)HugeBuf_GetCodeAddr(data_header.Inf.updater_inf.data_bytescnt) + 
									data_header.Inf.updater_inf.SubAddressTable[ui]);
							DeSecurity((INT8U *)addr, (INT8U *)addr, data_header.Inf.updater_inf.SubSizeTable[ui]);
						}

						if (((ARM7FwHeader_t *)addr)->FwFlag == MAKE_4CHAR_CONST_BE('H', 'B', 'V', '2'))
						{
							cmdBegin = UART_UPDATE_HB_BEGIN;
						}
						else if(((ARM7FwHeader_t *)addr)->FwFlag == MAKE_4CHAR_CONST_BE('A', 'P', 'V', '1'))
						{
							cmdBegin = UART_UPDATE_AP_BEGIN;
						}
						else
						{
							status_ReportStatus(STATUS_WAR_HB_FW_VERSION, STATUS_SET);	// soft will clear after confirm
							break;
						}

						nFWLenSave = nFWLen = data_header.Inf.updater_inf.SubSizeTable[ui];
						oldPercent = 0;

						do		// for break;
						{
							OS_FLAGS flgUpdate = MISC_FLAGS_UPDATE_HB;
							OS_FLAGS flgUError = MISC_UPDATE_HB_ERROR;

							INT8U cmdData  = UART_UPDATE_HB_DATA;
							INT8U cmdEnd   = UART_UPDATE_HB_END;

							INT8U nUartChnl = UART_HEAD_CHANNEL;

							if (nFWLen == 0) break;

							BOOL bErrOnUpdate = FALSE;
							nFWLen = nFWLenSave;
							pFWData = (INT8U *)addr;//(INT8U *)(addr + data_header.Inf.updater_inf.SubAddressTable[ui]);

							// CONSOL_Printf("Head board FW Updating begin! Len = %d\r\n", nFWLen);
							OSFlagPost (mix_FLAG_GRP, flgUpdate | flgUError, OS_FLAG_SET, &err);

							cmdbuf[0] = 10;						
							cmdbuf[1] = cmdBegin;
							memcpy(&cmdbuf[2], &(((ARM7FwHeader_t *)addr)->FwID), 8);	// only have means for ARM code							

							while (!UART_SendCMD(nUartChnl, cmdbuf))
								OSTimeDly(10);

							OS_FLAGS flgWait = OSFlagPend(mix_FLAG_GRP, flgUpdate | flgUError, OS_FLAG_WAIT_CLR_ANY, UPDATEHB_WAITTIME, &err);
							if ((err != OS_NO_ERR) || ((flgWait & flgUError) != 0))
							{
								if (err != OS_NO_ERR)
								{
									status_ReportStatus(STATUS_UPDATEERR_HB_BEGIN_TIMEOUT, STATUS_SET);
									// CONSOL_Printf("Updating head-board begin timeout error!\r\n");
								}
								bErrOnUpdate = TRUE;
								break;
							}

							while (nFWLen != 0)
							{
								OSFlagPost (mix_FLAG_GRP, flgUpdate, OS_FLAG_SET, &err);
								len = (nFWLen < MAX_CMD_LEN - LEN_OFFSET_INCMD - 2) ? nFWLen : (MAX_CMD_LEN - LEN_OFFSET_INCMD - 2);
								cmdbuf[0] = len+2;
								cmdbuf[1] = cmdData;
								memcpy(&cmdbuf[2], pFWData, len);
								while (!UART_SendCMD(nUartChnl, cmdbuf))
									OSTimeDly(10);
								flgWait = OSFlagPend (mix_FLAG_GRP, flgUpdate | flgUError, OS_FLAG_WAIT_CLR_ANY, UPDATEHB_WAITTIME, &err);
								if ((err != OS_NO_ERR) || ((flgWait & flgUError) != 0))
								{
									if (err != OS_NO_ERR)
									{
										status_ReportStatus(STATUS_UPDATEERR_HB_DATA_TIMEOUT, STATUS_SET);
										// CONSOL_Printf("Updating head-board data timeout error!\r\n");
									}
									bErrOnUpdate = TRUE;
									break;
								}

								nFWLen -= len;
								pFWData += len;
								status_EP0Buf.percent = 100 * ui / data_header.Inf.updater_inf.FWNum
									+ 100 * (nFWLenSave - nFWLen) / data_header.Inf.updater_inf.FWNum / nFWLenSave;
								if (oldPercent != status_EP0Buf.percent)
								{
									oldPercent = status_EP0Buf.percent;
									status_ReportStatus(STATUS_UPDATING, STATUS_SET);
									// CONSOL_Printf("%d%%\r\n", status_EP0Buf.percent);							
								}
							}
							if (bErrOnUpdate) break;

							OSFlagPost (mix_FLAG_GRP, flgUpdate, OS_FLAG_SET, &err);						
							cmdbuf[0] = 2;
							cmdbuf[1] = cmdEnd;

							while (!UART_SendCMD(nUartChnl, cmdbuf))
								OSTimeDly(10);

							flgWait = OSFlagPend (mix_FLAG_GRP, flgUpdate | flgUError, OS_FLAG_WAIT_CLR_ANY, UPDATEHB_WAITTIME, &err);
							if ((err != OS_NO_ERR) || ((flgWait & flgUError) != 0))
							{
								if (err != OS_NO_ERR)
								{
									status_ReportStatus(STATUS_UPDATEERR_HB_END_TIMEOUT, STATUS_SET);
									// CONSOL_Printf("Updating motion timeout error!\r\n");
								}
								bErrOnUpdate = TRUE;
								break;
							}
							status_EP0Buf.percent = 100 * ui / data_header.Inf.updater_inf.FWNum
								+ 100 / data_header.Inf.updater_inf.FWNum;
							status_ReportStatus(STATUS_UPDATING, STATUS_SET);
							// CONSOL_Printf("Headboard FW Update success!!");
						} while (FALSE);
						OSTimeDly(10);
						break;		
					}	
				default:
					status_ReportStatus(STATUS_UPDATEERR_INTERNAL_DATA, STATUS_SET);
					goto end;
				}
				status_EP0Buf.percent = 100*(ui+1)/data_header.Inf.updater_inf.FWNum;
			}
			status_EP0Buf.percent = 100;
#if (defined(SUPPORT_MOTOR_CONTROL)&&!defined(MOTION_Y_DSP))
			if(b_UpdateDSP)
				status_ReportStatus(STATUS_UPDATEERR_TRY_AGAIN, STATUS_SET);
			else
#endif
				status_ReportStatus(STATUS_UPDATE_SUCCESS, STATUS_SET);
end:
			//				CONSOL_Printf("Total %d\r\n",data_header.Inf.updater_inf.FWNum);

			if (!   HugeBuf_Flush(data_header.data_bytescnt))
				CONSOL_Printf("Flush updating data failed");
			//				CONSOL_Printf("Flush updating data: %d", data_header.data_bytescnt);
#if UPDATEDSP_WAITTIME != 0				
			OSFlagPend(status_FLAG_GRP,STATUS_UPDATING,OS_FLAG_CLR,10000,&err);
#endif			

			if (err != OS_NO_ERR)
				status_ReportStatus(STATUS_UPDATING, STATUS_CLR);

			break;
		default:
			status_ReportStatus(STATUS_WAR_UNKNOWNHEADERTYPE, STATUS_SET);
			break;	
		}			

	}//Task body	
}


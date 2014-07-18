#ifndef IIC_Key_Board_LCD
#define PG_KEY_BOARD
#endif

#include "os_includes.h"
#include "board.h"
#include "global.h"
#include "keylcd.h"
#include "miscellance.h"
#include "consol.h"
#include "control.h"
#include "status.h"
#include "key.h"
#include "display.h"
#include "calibration.h"

#include "usb.h"
#include "commpipe.h"

#include "old_apis.h"
//#define FAST_MOVE_REQ

//#define DEBUG_CAL_MOTIONFPGA_FIRE
//For debug pupose, EINT11 is used to calculate DSP fire signal to FPGA, rising edge trigger
//For debug pupose, EINT13 is used to calculate FPGA fire signal, falling edge trigger
//EINT16 is LVDS lock signal, both edge trigger.
//Need change FPGA code

//Key board interrupt: EINT8/GPG0

static OS_EVENT *I2C_KEY_LCD;

static const Pin KBResetPin = PIN_KB_CTRL_KB_RST;
static const Pin KBIrqPin = PIN_KB_CTRL_KB_IRQ; //cheney: not used for ALLWIN EPSON HW first version.

//#ifdef DEBUG_CAL_MOTIONFPGA_FIRE
//static const Pin FPGAMotionXDirPin = PIN_FPGA_ARM_XDIR; //EINT11
//static const Pin FPGAMotionXPulsePin = PIN_FPGA_ARM_XPULSE; // EINT13
//#endif

static INT8U keystatus;

extern INT8U MediaMeasured;
#ifdef ALLWIN_EPSON_SAME
INT8U pause_cmd_from_dsp = 0;
#endif

const Pin LVDSLockPin = PIN_LVDS_LOCK_EINT;

void RESET_KB(void)
{
	PIO_Clear(&KBResetPin);
	OSTimeDly(100);
	PIO_Set(&KBResetPin);
	OSTimeDly(100);
}

void KB_Irq_Interrupt(const Pin * pin)
{
	INT8U err;
	//if (PIO_Get(pin)==0)
	if (PIO_Get(pin)==1)
		OSFlagPost(mix_FLAG_GRP, IIC_KEY_FLAG, OS_FLAG_SET, &err);
}

void LVDS_LOCK_Interrupt(const Pin * pin)
{
	if (PIO_Get(pin) == 1)
	{
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_CLR);
		//			CONSOL_Printf("LVDS is Relocked!.r\n");
	}	
	else
	{
		//remove for slient
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_SET);
		CONSOL_Printf("LVDS is Unlocked!\r\n");	
	}
}

#ifdef IIC_Key_Board_LCD
#if defined( EPSON_LCD)
INT8U TEST_IIC_LED()
{
	INT8U i;
	INT8U sub[2];
	INT8U err;
	
	OSSemPend(IIC_KeyboardSem, 0, &err);	
	for (i = 0; i < 30; i++)
	{
		sub[0] = sub[1] = 0;
		
		OSTimeDly(3);
		IIC_WriteReg(0x17, 0x55); 
		OSTimeDly(3);
		IIC_ReadReg(0x17, sub,1);
		OSTimeDly(3);
		IIC_WriteReg(0x17, 0xAA); 
		OSTimeDly(3);
		IIC_ReadReg(0x17, sub+1,1);
		if (sub[0] == 0x55 && sub[1] == 0xAA)
			break;    		
	}
	OSSemPost(IIC_KeyboardSem);           /* Release semaphore */    
	
	if (i >= 30)
		return False;
	else
		return True;    
}

#endif
#endif


INT8U KB_Init_Again(void)
{
	OS_CPU_SR cpu_sr;
	INT8U err, buf[8];
	
	RESET_KB();
	
#ifdef IIC_Key_Board_LCD
#if defined( EPSON_LCD)
	if(TEST_IIC_LED())
	{
		OSSemPend(IIC_KeyboardSem, 0, &err);	
		OSTimeDly(3);
		buf[0] = 0x77;
		buf[1] = 7;
		IIC_WriteRegs(0xC, buf, 2); 
		OSTimeDly(3);
		memset(buf, 0, 8);
		IIC_WriteRegs(0x10, buf, 8); 
		OSTimeDly(3);
		OSSemPost(IIC_KeyboardSem);           /* Release semaphore */    
		return TRUE;
	}
	else
#endif
#endif
		return FALSE;
}

//GPG11 is the KB reset, EINT8 is the KB interrupt
INT8U KB_Init()
{
	OS_CPU_SR cpu_sr;
	INT8U err, buf[8];
	
	RESET_KB();
	
	OS_ENTER_CRITICAL();
	
	PIO_ConfigureIt(&KBIrqPin, KB_Irq_Interrupt);
	PIO_EnableIt(&KBIrqPin);
	//#ifdef DEBUG_CAL_MOTIONFPGA_FIRE
	//  PIO_ConfigureIt(&FPGAMotionXDirPin, FPGA_EINT11_Interrupt);
	//  PIO_ConfigureIt(&FPGAMotionXPulsePin, FPGA_EINT13_Interrupt);
	//  PIO_EnableIt(&FPGAMotionXDirPin);
	//  PIO_EnableIt(&FPGAMotionXPulsePin);
	//#endif	
#ifndef DEBUG_NO_HEADBOARD  
#ifndef EPSON_BOTTOM_BOARD_V3
	PIO_ConfigureIt(&LVDSLockPin, LVDS_LOCK_Interrupt);
	PIO_EnableIt(&LVDSLockPin);
#endif
#endif        
	OS_EXIT_CRITICAL();	
	
#ifdef IIC_Key_Board_LCD
#if defined( EPSON_LCD)
	if(TEST_IIC_LED())
	{
		OSSemPend(IIC_KeyboardSem, 0, &err);	
		OSTimeDly(3);
		buf[0] = 0x77;
		buf[1] = 7;
		IIC_WriteRegs(0xC, buf, 2); 
		OSTimeDly(3);
		memset(buf, 0, 8);
		IIC_WriteRegs(0x10, buf, 8); 
		OSTimeDly(3);
		OSSemPost(IIC_KeyboardSem);           /* Release semaphore */    
		return TRUE;
	}
	else
#endif
#endif
		return FALSE;
}

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
//EPSON_CLEAN_UPDOWN nedd the following code.
INT8U UIAction_Move(void * pData, INT8U dir, INT32S distance)
{
	U8 err;
	if(dir == 1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
#if defined( EPSON_CLEAN_UPDOWN)
			RegPostMoveOfUncapping(dir | 0x20, distance);
			StartUnCapping();
#else //MANUFACTURER_MICOLOR_EPSON
			move_dir = dir;
			move_distance = distance;
			move_type = 3;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
#endif
			return True;
		}
		else
			return False;
	}
	else if(dir >= 2 && dir <= 4)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			move_dir = dir;
#if RIPSTAR_FLAT_EX
			if(dir == 2 )
			{
#ifdef MANUFACTURER_DYSS
				{         	
					int i = 0;
					int offset =0;
					int position=0;
					float add =0.0;
					int step = epsonCaliSetting.eprCaliSetting.stepPass[0].BaseStep;
					int passnum = GetLCDMenuPassNum(LCDMenuConfig.Step_PassIndex);
					step += epsonCaliSetting.eprCaliSetting.stepPass[0].PassStep[passnum-1] * passnum;             	
					if(distance>=step/2)
					{
						position = (distance-step/2)/(step/2);
						offset=(distance-step/2)%(step/2);
						for(i=0;i<position;i++)
							add+= LCDMenuConfig_EX.add[i]*step/192/2;
						add +=((float)offset/(step/2))*LCDMenuConfig_EX.add[position]*step/192/2;
						
					}
					distance+=(int)add;
				}
#endif
				move_distance = distance + Y_PLAT_LENGTH;
			}
			else
#endif            
				move_distance = distance;
			move_type = 3;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
			return True;
		}
		else
			return False;
	}
	else
		return False;
}

INT8U UIAction_Move_Start(void * pData, INT8U dir, INT32S distance)
{
	U8 err;
	if(dir == 1 || dir == 2)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
#if defined( EPSON_CLEAN_UPDOWN)
			RegPostMoveOfUncapping(dir, distance);
			StartUnCapping();
#else //MANUFACTURER_MICOLOR_EPSON
			move_dir = dir;
			move_distance = distance;
			move_type = 0;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
			return True;
		}
		else
			return False;
	}
	else if(dir >= 3 && dir <= 6)
	{
#if defined(MANUFACTURER_ALLWIN_2H_ROLL_UV)||defined(MANUFACTURER_ALLWIN_4H_ROLL_UV)||defined(MANUFACTURER_ZHONGYE_ROLL_UV)
		if(dir == 3)
			dir = 4;
		else if(dir == 4)
			dir = 3;
#endif

#if defined (MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)      
		if(dir == 5)
			dir = 6;
		else if(dir == 6)
			dir = 5;
#endif
		
#if defined(RIPSTAR_SIX_AXIS_MODEL)&&defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
	if(dir == 5 || dir == 6)
		move_dir +=4;
#endif
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			move_dir = dir;
			move_distance = distance;
			move_type = 0;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
			return True;
		}
		else
			return False;
	}
	else
		return False;
}

INT8U UIAction_Move_End(void * pData)
{
	U8 err;
	
	if (OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		return True;
	}
	else
		return False;
}

INT8U UIAction_X_GoHome(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#if defined( EPSON_CLEAN_UPDOWN)
		if(absv(curPos.x) < PRINTER_X_ORIGIN_PERMITTED_ERROR)
		{
			StartCapping();
		}
		else
		{
			RegPostMoveOfUncapping(2, 0);
			RegPostCapping();
			StartUnCapping();
		}
#else //MANUFACTURER_MICOLOR_EPSON
		move_dir = 2;
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		return True;
	}
	else
		return False;
}
INT8U UIAction_X_GoHome_by_key(void * pData)
{
	U8 err;
	if((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ||( (OSFlagAccept(status_FLAG_GRP, CMD_PAUSE |STATUS_PRINT|STATUS_PAUSE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&& (OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ))			
	{
#if defined( EPSON_CLEAN_UPDOWN) || defined(EPSON_CLEAN_INTEGRATE_3_CAP)
		if(absv(curPos.x) < PRINTER_X_ORIGIN_PERMITTED_ERROR)
		{
			StartCapping();
		}
		else
		{
			RegPostMoveOfUncapping(2, 0);
			RegPostCapping();
	#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
			StartUnCapping(False);
	#else
			StartUnCapping();
	#endif
		}
#else //MANUFACTURER_MICOLOR_EPSON
		move_dir = 2;
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		return True;
	}
	else
		return False;
}
#ifdef EPSON_FLASH_IDLE
INT8U postCapping_flash = False;
INT8U UIAction_FLAH_IDLE(void * pData, int Index)
{
	if(Index != 0)
	{
		UIAction_X_GoHome_by_key(0);
		return True;
	}
	U8 err;
	if((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ||(OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ) 		
	{
#if defined(EPSON_CLEAN_UPDOWN)
#ifdef FULGENCY_FUN
		if(curPos.x < (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos +10) && curPos.x > (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos -10))
		{
			if(cleanPara.flash == True)
			{
				False_Idle_Off();
				cleanPara.flash = False;
				move_dir = 4;
				move_distance = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;//move_distance means target position
				move_type = 3;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
			}
			else
			{
				move_dir = 4;
				move_distance = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;//move_distance means target position
				move_type = 3;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				cleanPara.flash = True;
			}
		}
		else
		{
			if(curPos.x > 0)
				RegPostMoveOfUncapping(2, curPos.x - cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos);
			else
				RegPostMoveOfUncapping(1, cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos - curPos.x);
			postCapping_flash = True;
			
			move_dir = 4;
			move_distance = Z_SAFE_POSITION;//move_distance means target position
			move_type = 3;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
			
			cleanPara.flash = True;	
		}
#else
		if(curPos.x < (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos +10) && curPos.x > (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos -10))
		{
			OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
			
			move_dir = 4;
			move_distance = cleanparam_EPSON_ALLWIN.HeadBox_Z_FlashPos;//move_distance means target position
			move_type = 3;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		}
		else
		{
			if(curPos.x > 0)
				RegPostMoveOfUncapping(2, curPos.x - cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos);
			else
				RegPostMoveOfUncapping(1, cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos - curPos.x);
			postCapping_flash = True;
			
			OSFlagPost(mix_FLAG_GRP, EPSON_DO_CAPPING, OS_FLAG_SET,&err); 
			move_dir = 4;
			move_distance = 0;//move_distance means target position
			move_type = 3;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		}
		cleanPara.flash = True;
#endif		
#else //MANUFACTURER_MICOLOR_EPSON
		move_dir = 2;
		move_distance = curPos.x - cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		cleanPara.flash = True;
#endif
		return True;
	}
	else
		return False;
}
#endif
INT8U UIAction_AllAxis_GoHome(void * pData, INT8U dir)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#if defined( EPSON_CLEAN_UPDOWN)
		if(dir == 1)
		{
			if(curPos.x < PRINTER_X_ORIGIN_PERMITTED_ERROR)
			{
				RegPostCapping();
				move_dir = dir;  //move_dir means axis.
				move_distance = 0;
				move_type = 1;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
			}
			else
			{
				RegPostMoveOfUncapping(0x10 | dir, 0);
				RegPostCapping();
				StartUnCapping();
			}
		}
		else if(dir == 2)
		{
			move_dir = dir;  //move_dir means axis.
			move_distance = 0;
			move_type = 1;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
		}
		else if(dir == 4)
		{
			if(curPos.x < PRINTER_X_ORIGIN_PERMITTED_ERROR)
			{
				move_dir = 4;
				move_distance = cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos;//move_distance means target position
				move_type = 3;
				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
			}
			else
			{
				RegPostMoveOfUncapping(0x10 | 1, 0);
				RegPostCapping();
				StartUnCapping();
			}
		}
#else //MANUFACTURER_MICOLOR_EPSON
		move_dir = dir;
		move_distance = 0;
		move_type = 1;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		return True;
	}
	else
		return False;
}

INT8U UIAction_MeasureMedia(void * pData, INT8U method)
{
	U8 err;
	if (LCDMenuConfig.SheetIsTransparent)
		return False;
#ifdef MICOLOR_AUTOFUCTION
	if(!((!LCDMenuConfig.AutoLackMediaCheck || !IsOutOfMedia()) && (!LCDMenuConfig.AutoFixCheck || IsMediaFixed()) && (!LCDMenuConfig.AutoCoverCheck || IsCoverSensorValid())))
		return False;
#endif
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		move_type = 4;
		measure_media_method = method;
#if defined( EPSON_CLEAN_UPDOWN)
		RegPostMoveOfUncapping(3, 0);
		StartUnCapping();
#else	
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		MediaMeasured = True;
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Left_Start(void * pData)
{
	U8 err;
	if((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ||( (OSFlagAccept(status_FLAG_GRP, CMD_PAUSE |STATUS_PRINT|STATUS_PAUSE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&& (OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) ))			
	{
		keystatus|=0x01;
#if defined( EPSON_CLEAN_UPDOWN)
		if(factoryData.m_nBitFlag & 0x1)
			RegPostMoveOfUncapping(1, 0);
		else
			RegPostMoveOfUncapping(2, 0);
		StartUnCapping();
#elif defined(EPSON_CLEAN_INTEGRATE_3_CAP)
		if(factoryData.m_nBitFlag & 0x1)
			RegPostMoveOfUncapping(1, 0);
		else
			RegPostMoveOfUncapping(2, 0);

		if(OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
		{
			StartUnCapping(True);
		}
		else
		{
			StartUnCapping(False);
		}
#else //MANUFACTURER_MICOLOR_EPSON
#ifdef FAST_MOVE_REQ
		{
			INT8U buf[16];
			buf[0] = 7; //Length						
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = printer.org_oppLimit;
			buf[6] = printer.movSpeed;	
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(10);	
		}
#else 
		if(factoryData.m_nBitFlag & 0x1)
			move_dir = 1;
		else
			move_dir = 2;
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		
#endif
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Right_Start(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)||((OSFlagAccept(status_FLAG_GRP, CMD_PAUSE |STATUS_PRINT|STATUS_PAUSE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&& (OSFlagAccept(status_FLAG_GRP,CMD_START_MOVE|STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)) )	 
	{
		keystatus|=0x02;
#if defined( EPSON_CLEAN_UPDOWN)
		if(factoryData.m_nBitFlag & 0x1)
			RegPostMoveOfUncapping(2, 0);
		else
			RegPostMoveOfUncapping(1, 0);
		StartUnCapping();
#elif defined(EPSON_CLEAN_INTEGRATE_3_CAP)
		if(factoryData.m_nBitFlag & 0x1)
			RegPostMoveOfUncapping(2, 0);
		else
			RegPostMoveOfUncapping(1, 0);
		
		if(OSFlagAccept(status_FLAG_GRP, STATUS_CAPPED, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
		{
			StartUnCapping(True);
		}
		else
		{
			StartUnCapping(False);
		}
#else //MANUFACTURER_MICOLOR_EPSON
		
#ifdef FAST_MOVE_REQ
		{
			INT8U buf[16];
			buf[0] = 7; //Length									
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = 0;
			buf[6] = printer.movSpeed;	
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(10);	
		}
#else
		if(factoryData.m_nBitFlag & 0x1)
			move_dir = 2;
		else
			move_dir = 1;
		move_distance = 0;
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
		
#endif
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Up_Start(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#ifdef RIPSTAR_FLAT_EX    
		move_dir = 3;
#else
		move_dir = 4;
#endif
		move_distance = 0;
		move_type = 0;
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
		keystatus|=0x3C;
#else
		keystatus|=0x04;
#endif
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Down_Start(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#ifdef RIPSTAR_FLAT_EX    
		move_dir = 4;
#else
		move_dir = 3;
#endif
		move_distance = 0;
		move_type = 0;
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
		keystatus|=0x3C;
#else		
		keystatus|=0x08;
#endif
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);		
		return True;
	}
	else
		return False;
}
INT8U UIAction_Move_Z_Up_Start(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS|STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#if defined(MANUFACTURER_TATE_EPSON_UV)&&defined(RIPSTAR_FLAT_EX)&&defined(HEAD_RICOH_G4)   
		move_dir = 6;
#else
		move_dir = 5;
#endif
#if defined(RIPSTAR_SIX_AXIS_MODEL)&&defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
		move_dir +=4;
#endif
		move_distance = 0;
		move_type = 0;
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
		keystatus|=0x3C;
#else		
		keystatus|=0x10;
#endif
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Z_Down_Start(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_X_MOVE_BITS|STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#if defined(MANUFACTURER_TATE_EPSON_UV)&&defined(RIPSTAR_FLAT_EX)&&defined(HEAD_RICOH_G4)   
		move_dir = 5;
#else
		move_dir = 6;
#endif
		
#if defined(RIPSTAR_SIX_AXIS_MODEL)&&defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
		move_dir +=4;
#endif
		move_distance = 0;
		move_type = 0;
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
		keystatus|=0x3C;
#else		
		keystatus|=0x20;
#endif
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);		
		return True;
	}
	else
		return False;
}

INT8U slow_down_at_once = False;
INT8U UIAction_Move_Left_End(void * pData)
{
	U8 err;
	if((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x01))
	{
		keystatus&=(~0x01);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		slow_down_at_once = TRUE;
		//stop_move();
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Right_End(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x02))
	{
		keystatus&=(~0x02);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		//stop_move();
		slow_down_at_once = TRUE;
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Up_End(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x04))
	{
		keystatus&=(~0x04);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Down_End(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x08))
	{
		keystatus&=(~0x08);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}
INT8U UIAction_Move_Z_Up_End(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x10))
	{
		keystatus&=(~0x10);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}

INT8U UIAction_Move_Z_Down_End(void * pData)
{
	U8 err;
	if ((OSFlagAccept(status_FLAG_GRP, CMD_START_MOVE, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(keystatus&0x20))
	{
		keystatus&=(~0x20);
		status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
		OSTimeDly(15); //This delay used to stop old move, can not delete
		return True;
	}
	else
		return False;
}
#endif

#ifdef IIC_Key_Board_LCD
#if defined( EPSON_LCD)
extern INT8U Lcd_Cmd_Q[];
extern INT16U Lcd_Cmd_Num;
#define IIC_LCD_MAX_SIZE 32
INT8U Lcd_Cmd_Send_Q[IIC_LCD_MAX_SIZE+3];
extern unsigned char CRC8_Table(unsigned char *p, char counter);
INT8U IIC_Write_LCD(void)
{
	INT8U Page[IIC_LCD_MAX_SIZE] ={0};
	INT8U page_crc = 0;
	INT8U byte_sum = 0;
	INT8U page_sum = 0,i = 0;
	INT8U page_num = 1,ret = 0;
	INT16U j= 0;
	INT8U err;
	
	OSSemPend(I2C_KEY_LCD, 0, &err);
	
	page_sum = Lcd_Cmd_Num/IIC_LCD_MAX_SIZE + 1;
	if(Lcd_Cmd_Num%IIC_LCD_MAX_SIZE == 0)
		page_sum --;
	
	//    if(ICC_fail == True)
	//    {
	//        return False;
	//    }
	while(!(page_num > page_sum))
	{
		i = 0;
		if(page_sum -page_num == 0)
			byte_sum = Lcd_Cmd_Num - (page_num -1)*IIC_LCD_MAX_SIZE;
		else
			byte_sum = IIC_LCD_MAX_SIZE;
		Lcd_Cmd_Send_Q[0] = byte_sum;
		Lcd_Cmd_Send_Q[1] = page_sum;
		Lcd_Cmd_Send_Q[2] = page_num;
		while(i < byte_sum)
		{
			Lcd_Cmd_Send_Q[3+i] = Lcd_Cmd_Q[(page_num -1)*IIC_LCD_MAX_SIZE + i];
			i++;
		}
		page_crc = CRC8_Table(Lcd_Cmd_Send_Q, byte_sum+3);
		do{
			if(False == IIC_Write_LCD_32(page_crc, Lcd_Cmd_Send_Q,  byte_sum+3))
			{
				OSTimeDly(10);
			}
			//            if(ICC_fail == True)
			//            {
			//                ICC_fail = False;
			//                return False;
			//            }
			do{
				ret = 0xFF;
				OSTimeDly(1);
				if(page_num == page_sum)
					OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_CLR, &err);
				
				if(False == IIC_Read_LCD(0,&ret,1))
				{
					OSTimeDly(10);
					continue;
				}
				else if(ret != 0xFF)
					break;
			}
			while( ret == 0xFF);
			OSTimeDly(1);
		}while(ret != page_num);
		page_num++;
	}	
	
	OSSemPost(I2C_KEY_LCD);
	
	return True;
}


INT8U IIC_ReadKey(INT8U *data)
{
	INT8S resetcnt = 2;
	while(resetcnt > 0)
	{
		INT8S retrycnt = 3;
		while (retrycnt--)
		{
			OSTimeDly(3);
			if (IIC_ReadReg(ZLG7290_Key, data,3))
				//if (IIC_ReadReg(ZLG7290_SystemReg, data,10))
				break;		
		}
		if (retrycnt <= 0)
		{
			//            RESET_KB();
			//            resetcnt --;
			return False;
		}else
			return True;
	}
	return False;
}

INT8U IIC_GetKey(INT8U *key, INT8U *repeat, INT8U *funkey)
{
	INT8U mkey[5][3];
	//	INT8U mkey[5][10];
	INT8S trust;
	INT8S i,j;
	
	if (!IIC_ReadKey(mkey[0]))
		return False;
	if (!IIC_ReadKey(mkey[1]))
		return False;
	if (!IIC_ReadKey(mkey[2]))
		return False;
	if (!IIC_ReadKey(mkey[3]))
		return False;
	if (!IIC_ReadKey(mkey[4]))
		return False;
	
	*repeat = 0;
	for (i=0;i<3;i++)
	{
		trust = 0;
		for (j=i+1;j<5;j++)
		{			
			if ((mkey[i][0] == mkey[j][0])&&(mkey[i][2] == mkey[j][2]))
				trust++;
		}
		if (trust >= 2)
			break;
	}
	
	if (trust < 2)
		return False;
	
	//	if ((mkey[i][0] == 0)&&(mkey[i][2] == 0xFF)) //When the function key release, this will occur, so this is not correct
	//		return False;		
	
	*key = mkey[i][0];
	*repeat = mkey[i][1];
	*funkey = mkey[i][2];
	
	return True;		
}

INT8U UI_DisplayLED(enum LEDIDEnum ledID, enum LEDStatusENUM ledStatus)
{
	INT8U i, buf[2];
	INT8U addr, bit;
	INT8U err;
	
	const INT8U addrTable[] = {PANEL_LED_ADDR_POWERON, PANEL_LED_ADDR_BUSY, PANEL_LED_ADDR_HEAT_PRE, 
	PANEL_LED_ADDR_HEAT_MIDDLE, PANEL_LED_ADDR_HEAT_POST,
	PANEL_LED_ADDR_DATATRANSFER, PANEL_LED_ADDR_CLEAN_WEAK,
	PANEL_LED_ADDR_CLEAN_NORMAL, PANEL_LED_ADDR_CLEAN_STRONG,
	PANEL_LED_ADDR_HEADLEVEL_HIGH, PANEL_LED_ADDR_HEADLEVEL_LOW,
	PANEL_LED_ADDR_QUALITY_ON, PANEL_LED_ADDR_QUALITY_OFF};
	
	const INT8U bitTable[] = {PANEL_LED_BIT_POWERON, PANEL_LED_BIT_BUSY, PANEL_LED_BIT_HEAT_PRE, 
	PANEL_LED_BIT_HEAT_MIDDLE, PANEL_LED_BIT_HEAT_POST,
	PANEL_LED_BIT_DATATRANSFER,PANEL_LED_BIT_CLEAN_WEAK,
	PANEL_LED_BIT_CLEAN_NORMAL,PANEL_LED_BIT_CLEAN_STRONG,
	PANEL_LED_BIT_HEADLEVEL_HIGH,PANEL_LED_BIT_HEADLEVEL_LOW,
	PANEL_LED_BIT_QUALITY_ON,  PANEL_LED_BIT_QUALITY_OFF};
	
	static INT8U AllLEDFlashStatus =  0;
	
	//return TRUE; //only for debug.
	
	addr = addrTable[(INT8U)ledID];
	bit = bitTable[(INT8U)ledID];
	
	OSSemPend(IIC_KeyboardSem, 0, &err);
	if(ledStatus == E_LS_FLASH)
	{
		i = (addr -0x10)*8+bit;
		i |= 0x80; 
		OSTimeDly(3);
		buf[0] = 0x1; 
		buf[1] = i;
		IIC_WriteRegs(0x7, buf, 2); 
		OSTimeDly(3);
		
		AllLEDFlashStatus |= (1<<(addr -0x10));
		
		OSTimeDly(3);
		buf[0] = 0x70; 
		buf[1] = AllLEDFlashStatus;
		IIC_WriteRegs(0x7, buf, 2); 
		OSTimeDly(3);
	}
	else
	{
		if(AllLEDFlashStatus & (1<<(addr -0x10)) )
		{
			AllLEDFlashStatus &= ~(1<<(addr -0x10));
			OSTimeDly(3);
			buf[0] = 0x70; 
			buf[1] = AllLEDFlashStatus;
			IIC_WriteRegs(0x7, buf, 2); 
			OSTimeDly(3);
		}
		i = (addr -0x10)*8+bit;
		if(ledStatus == E_LS_ON)
			i |= 0x80; 
		OSTimeDly(3);
		buf[0] = 0x1; 
		buf[1] = i;
		IIC_WriteRegs(0x7, buf, 2); 
		OSTimeDly(3);
	}
	OSSemPost(IIC_KeyboardSem);           /* Release semaphore */
	return True;    
}

INT16U GetUIKeyID(INT8U PanelKeyID)
{
	const INT8U ValidKeyTable[] = {
		PANEL_KEY_ID_LEFT,  PANEL_KEY_ID_RIGHT,
		PANEL_KEY_ID_UP,    PANEL_KEY_ID_DOWN,
		PANEL_KEY_ID_MENU,  PANEL_KEY_ID_ENTER,
		PANEL_KEY_ID_CANCEL,PANEL_KEY_ID_PAUSE,
		PANEL_KEY_ID_CLEAN, PANEL_KEY_ID_BASEPOINT,
		PANEL_KEY_ID_SHEETCUT,  PANEL_KEY_ID_PRINTQUALITY,
		PANEL_KEY_ID_CHECKNOZZLE, PANEL_KEY_ID_HEATER};
	
	const INT16U KeyIDMapTable[] = {
		KEYID_LEFT, KEYID_RIGHT,  KEYID_UP,     KEYID_DOWN, 
		KEYID_MENU, KEYID_ENTER,  KEYID_CANCEL, KEYID_PAUSE,
		KEYID_CLEAN,KEYID_BASEPOINT,KEYID_SHEETCUT,KEYID_PRINTQUALITY,
		KEYID_CHECKNOZZLE, KEYID_HEATER};
	
	INT8U i;
	
	if(PanelKeyID >64 || PanelKeyID == 0)
		return 0xFFFF;
	
	for(i=0; i< sizeof(ValidKeyTable); i++)
	{
		if(ValidKeyTable[i] == PanelKeyID)
		{
			return KeyIDMapTable[i];
		}
	}
	return 0xFFFF;
}

INT16U GetUIKeyIDEx(INT8U PanelFuncKeyID)
{
	if((PanelFuncKeyID & 0x1) == 0)
		return GetUIKeyID(56+1);
	else if((PanelFuncKeyID & 0x2) == 0)
		return GetUIKeyID(56+2);
	else return 0xFFFF;
}

INT8U IsValidKey(INT8U key, INT8U funkey)
{
	if(!(GetUIKeyID(key)!=0xFFFF || key == 0))
		return False;
	if(!(GetUIKeyIDEx(funkey) != 0xFFFF || funkey == 0xFF))
		return False;
	return True;
}

extern INT8U read_reset(void);
void Keyboard_Task (void *data)
{
	INT8U err;
	
	INT8U key = 0, repeat = 0, funckey;
	INT16U UI_keyid;
	INT8U oldkey = 0, oldfunckey = 0xFF; //最多２个功能键。
	INT8U IsKeyPush = 0;
	OS_FLAGS flags;
	int i = 0;
	
	
#if 0    
	SwitchMenuMode_S_A(1);
	LCD_SendKey(KEYID_MENU, KeyDown);
	OSTimeDly(50);
	LCD_SendKey(KEYID_MENU, KeyUp);
	OSTimeDly(50);
	while(1)
	{
		OSSemPend(IIC_KeyboardSem, 0, &err);
		err = IIC_GetKey(&key, &repeat, &funckey);
		OSSemPost(IIC_KeyboardSem);           /* Release semaphore */
		
		LCD_SendKey(KEYID_MENU, KeyDown);
		
		//OSTimeDly(50);
		OSSemPend(IIC_KeyboardSem, 0, &err);
		err = IIC_GetKey(&key, &repeat, &funckey);
		OSSemPost(IIC_KeyboardSem);           /* Release semaphore */
		
		LCD_SendKey(KEYID_MENU, KeyUp);	
		//OSTimeDly(50);
	}
#endif
	while(1)
	{
		if(oldkey == 0 & oldfunckey == 0xFF)
			IsKeyPush = 0;
		else
			IsKeyPush = 1;
		if(IsKeyPush)
			OSFlagPend(mix_FLAG_GRP,IIC_KEY_FLAG,OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,80,&err);
		else
		{
			flags = OSFlagPend(mix_FLAG_GRP,IIC_KEY_FLAG|Reset_Key_STATE,OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME,0,&err);
			if(flags&IIC_KEY_FLAG)
			{
				//OSFlagPost(mix_FLAG_GRP, IIC_KEY_FLAG, OS_FLAG_CLR, &err);
			}
			else
			{
#if !(defined(MANUFACTURER_MICOLOR_EPSON) || defined(MANUFACTURER_COLORFUL))                
				while((i<5)&&( read_reset()))
				{
					i++;
					OSTimeDly(50);
				}
				if(i==5)
					LCD_SendKey(0x201,KeyUp);
				//OSFlagPost(mix_FLAG_GRP, Reset_Key_STATE, OS_FLAG_CLR, &err);
#endif
				continue;
			}
		}
		
		OSSemPend(IIC_KeyboardSem, 0, &err);
		err = IIC_GetKey(&key, &repeat, &funckey);
		OSSemPost(IIC_KeyboardSem);           /* Release semaphore */
		
		if(!err || !IsValidKey(key, funckey)) //sometimes, get an invalid key,
			continue;
		
		if(oldkey != key || funckey != oldfunckey)
		{
			if(key != oldkey)
			{
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
				INT8U key_bak = key;
				if(funckey != 0xFF)
				{
					if(GetUIKeyIDEx(funckey) == KEYID_ENTER)
					{
						if( key == PANEL_KEY_ID_UP)	
							key = PANEL_KEY_ID_CLEAN;
						if( oldkey == PANEL_KEY_ID_UP)	
							oldkey = PANEL_KEY_ID_CLEAN;	
						if( key == PANEL_KEY_ID_DOWN)	
							key = PANEL_KEY_ID_SHEETCUT;
						if( oldkey == PANEL_KEY_ID_DOWN)	
							oldkey = PANEL_KEY_ID_SHEETCUT;							
					}
				}
#endif								
				UI_keyid = GetUIKeyID(oldkey);
				if(UI_keyid != 0xFFFF)
					LCD_SendKey(UI_keyid, KeyUp);
				
				UI_keyid = GetUIKeyID(key);
				if(UI_keyid != 0xFFFF)
					LCD_SendKey(UI_keyid, KeyDown);
#if defined(MANUFACTURER_DYSS)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(LCD_BOARD_B_VERSION)
				if(key == 0 &&(oldkey == PANEL_KEY_ID_SHEETCUT||oldkey == PANEL_KEY_ID_CLEAN)&&GetUIKeyIDEx(funckey) == KEYID_ENTER)
					funckey = oldfunckey = 0xFF;
				oldkey = key = key_bak;
#else
				oldkey = key;
#endif
			}
			if(GetUIKeyIDEx(funckey) != GetUIKeyIDEx(oldfunckey))
			{
				UI_keyid = GetUIKeyIDEx(oldfunckey);
				if(UI_keyid != 0xFFFF)
					LCD_SendKey(UI_keyid, KeyUp);
				
				UI_keyid = GetUIKeyIDEx(funckey);
				if(UI_keyid != 0xFFFF)
					LCD_SendKey(UI_keyid, KeyDown);
				
				oldfunckey = funckey;
			}
		}
	}
}

INT8U UIAction_CancelPrint(void * pData)
{
	INT8U Buff[4];
	U8 err;
	
	if((OSFlagAccept(status_FLAG_GRP, STATUS_PRINT|STATUS_NO_CANCEL, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)&&(OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR))
	{
		if((OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)&&(IS_ONLINE))
		{
			((INT16U*)Buff)[0] = EP6_CMD_T_PRINTACTION;
			((INT16U*)Buff)[1] = EP6_CMD_T_PA_CANCEL;
			return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 4, False);
		}
		else
		{
			//copy from USB CMD handler
			//status_ReportStatus(CMD_CANCEL_REQ, STATUS_CLR); 
			status_ReportStatus(STATUS_NO_CANCEL, STATUS_CLR);
			status_ReportStatus(STATUS_PAUSE, STATUS_CLR); //Clear pause
			status_ReportStatus(CMD_PAUSE, STATUS_CLR); //Clear pause
			status_FlushAllErrStatus();
			status_ReportStatus(STATUS_ERR, STATUS_CLR); //Clear error
			return True;
		}
	}
	else if(OSFlagAccept(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_WAIT_SET_ANY,&err), err == OS_NO_ERR)
	{
		OSFlagPost(mix_FLAG_GRP, CLEAN_FILL_STEP1_STATE, OS_FLAG_CLR, &err);
		Buff[0] = 2;
		Buff[1] = UART_STOPMOVE_CMD;
		while(!UART_SendCMD(UART_MOTION_CHANNEL, Buff)) //Stop move
			OSTimeDly(10);	
		return True;
	}
	else
	{
		return False;
	}
}

INT8U UIAction_PauseResumePrint(void * pData)
{
	INT8U Buff[4];
	U8 err;
#ifdef ALLWIN_EPSON_SAME
	INT32S data = (INT32S)pData;
	if(pause_cmd_from_dsp == 3)		//LCD、PM不可恢复打印
		return False;
#endif		
	if(CMD_Epson_Operate_Type == CMD_EPSON_T_MACHIME_TEST)
		return False;
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE | STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
		{
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			{
				((INT16U*)Buff)[0] = EP6_CMD_T_PRINTACTION;
				((INT16U*)Buff)[1] = EP6_CMD_T_PA_PAUSE;
				return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 4, False);
			}
			else
			{
				//copy from USB CMD handler
				status_ReportStatus(CMD_PAUSE, STATUS_SET);
				return True;
			}
		}
#ifdef DYSS_UV_CLEAN		
		else if(pData == (void *)1)
			return False;
#endif	
		else if(OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
		{
#ifdef EPSON_FLASH_IDLE
			if(flash_idle_on == True)
			{
				
				False_Idle_Off();
				cleanPara.flash = False;
			}
#endif
			
#if defined(HEAD_RICOH_G4)&&defined(RICOH_CLEAN_PRESS)
			False_Idle_Off(); 
#endif        
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			{
				((INT16U*)Buff)[0] = EP6_CMD_T_PRINTACTION;
				((INT16U*)Buff)[1] = EP6_CMD_T_PA_RESUME;
				return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 4, False);
			}
			else
			{
				//copy from USB CMD handler
				status_ReportStatus(CMD_PAUSE, STATUS_CLR);
				status_ReportStatus(STATUS_PAUSE, STATUS_CLR);
				return True;
			}
		}
		return False;
	}
	else
	{
		return False;
	}
}

#ifdef ALLWIN_EPSON_SAME
INT8U Dspcmd_PauseResumePrint(void * pData)
{
	INT8U Buff[4];
	U8 err;
	static INT32S curStatus = 0;
	//	0,1:cmd from LCD
	//	2:多次resume from dsp; 3:多次暂停 from dsp
	//	4:单次resume from dsp; 5:单次暂停 from dsp
	INT32S data = (INT32S)pData;

	if(data == 2)		//LCD/PM可恢复打印
	{
		pause_cmd_from_dsp = 0;
		return True;
	}
	if(data == 3 || data == 5)		//pause cmd from dsp
		pause_cmd_from_dsp = data;
	
	if(data == 4){}//do nothing
						
	if(CMD_Epson_Operate_Type == CMD_EPSON_T_MACHIME_TEST)
		return False;
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(status_FLAG_GRP, CMD_PAUSE | STATUS_PAUSE, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
		{
			if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			{
				((INT16U*)Buff)[0] = EP6_CMD_T_PRINTACTION;
				((INT16U*)Buff)[1] = EP6_CMD_T_PA_PAUSE;
				return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 4, False);
			}
			else		//resume --> pause
			{
				//copy from USB CMD handler
				status_ReportStatus(CMD_PAUSE, STATUS_SET);
				return True;
			}
		}
		return False;
	}
	else
	{
		return False;
	}
}
#endif

INT8U UIAction_SetPrintQuality(void * pData)
{
	U8 err;
	INT8U level;
	INT8U type;
	INT8U Buff[2+sizeof(struct USB_Print_Quality)];
	
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		OSSemPend(LCDMenuConfigureSem, 0, &err);
		level = LCDMenuConfig.PrintQuality & 0x0f;
		type = (LCDMenuConfig.PrintQuality >> 4) & 0x0f;
		
		if(level == 0)		//当前为关，即打印质量类型循环的开始
		{
			type = 0;
			level = 1;
		}
		else if(type == 5 && level == 3)		//当前打印类型循环的结束
		{
			level = 0;
			type = 0;
		}
		else
		{
			if(level >= 3)
			{
				level = 1;
				type += 1;
			}
			else
				level += 1;
		}
		
		LCDMenuConfig.PrintQuality = (type << 4) | level;
        	OSSemPost(LCDMenuConfigureSem);
		SaveLCDMenuConfig();
		
		((INT16U*)Buff)[0] = EP6_CMD_T_PRINTQUALITY;
		printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
		memcpy(&Buff[2], &printQuality_ToPM, sizeof(printQuality_ToPM));
		return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_Print_Quality), True);
	}
	else
		return False;
#if 0
	U8 err;
	INT8U Buff[2+sizeof(struct USB_Print_Quality)];
	
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		OSSemPend(LCDMenuConfigureSem, 0, &err);
		if(LCDMenuConfig.PrintQuality < 3)
		{
			LCDMenuConfig.PrintQuality++;
		}
		else
		{
#ifdef MANUFACTURER_GZ  	
			LCDMenuConfig.PrintQuality = 1;
#else
			LCDMenuConfig.PrintQuality = 0;
#endif
		}
		OSSemPost(LCDMenuConfigureSem);
		SaveLCDMenuConfig();
		
		((INT16U*)Buff)[0] = EP6_CMD_T_PRINTQUALITY;
		printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
		memcpy(&Buff[2], &printQuality_ToPM, sizeof(printQuality_ToPM));
		return PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_Print_Quality), True);
	}
	else
		return False;
#endif
}

INT8U UIAction_QuickCleaning(void * pData)
{
#if defined( EPSON_CLEAN_UPDOWN) ||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
	return DoKeyClean_EPSON_ALLWIN(pData,0);
#else
	return DoKeyClean_EPSON_MICOLOR(pData,0);
#endif
}

#endif
#endif
#define MIN_MEDIA_WIDTH 1.0

INT8U SetBasePoint(void * pData, float point, INT8U bGoHome, INT8U bFlashPM)
{
	U8 err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	
	float width;
	
	if(point >= PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)
		point = PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH;
	if(point < 0.0)
		point = 0.0;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(point >= PAPER_MEDIA_WIDTH_INCH)
		point = PAPER_MEDIA_WIDTH_INCH;
	if(point >= LCDMenuConfig.MediaWidth - MIN_MEDIA_WIDTH)
		point = LCDMenuConfig.MediaWidth- MIN_MEDIA_WIDTH;
	LCDMenuConfig.OriginX = point;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	if(bGoHome)
		UIAction_X_GoHome(pData);
#if defined( IIC_Key_Board_LCD) 				
	LCD_SetMainInfoEx(E_IT_PrintOrigin, &point);
#endif	
	if(bFlashPM)
	{
		((INT16U*)Buff)[0] = EP6_CMD_T_MAINUI;
		mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
		mainUIPara_ToPM.PassNum = mainUIPara.PassNum;
		mainUIPara_ToPM.StepModify = mainUIPara.StepModify;
		memcpy(&Buff[2], &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_MainUI_Param), True))
			ret = False;
		
		((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
		mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
		mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
		mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
		memcpy(&mediaInfo, &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
		memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
			ret = False;
	}
	return ret;
}
#ifdef RIPSTAR_FLAT_EX
INT8U SetBasePointY(void * pData, float point,INT8U bGoHome, INT8U bFlashPM)
{
	U8 err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	
	float width;
	if(point < 0.0)
		point = 0.0;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(point >= LCDMenuConfig_EX.MediaL)
		point = LCDMenuConfig_EX.MediaL;
	
	LCDMenuConfig_EX.OriginY = point;
	
	
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();
	
	// if(bGoHome)
	//     UIAction_X_GoHome(pData);
#if defined( IIC_Key_Board_LCD) 				
	LCD_SetMainInfoEx(E_IT_PrintYOrigin, &point);
#endif
	if(bFlashPM)
	{
		((INT16U*)Buff)[0] = EP6_CMD_T_MAINUI;	
		mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
		mainUIPara_ToPM.PrintYOrigin = LCDMenuConfig_EX.OriginY;
		mainUIPara_ToPM.PrintZOrigin = LCDMenuConfig_EX.OriginZ;
		mainUIPara_ToPM.PassNum = mainUIPara.PassNum;
		mainUIPara_ToPM.StepModify = mainUIPara.StepModify;
		memcpy(&Buff[2], &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_MainUI_Param), True))
			ret = False;    
	}
	return ret;	
}

INT8U SetBasePointZ(void * pData, float point,INT8U bGoHome, INT8U bFlashPM)
{
	U8 err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	
	float width;
#ifdef FULGENCY_FUN	
	if(point < 1.5/25.4)
		point = 1.5/25.4;
#else
	if(point < 0)
		point = 0;
#endif
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	
	
	LCDMenuConfig_EX.OriginZ = point;
	
	
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();
#if defined( IIC_Key_Board_LCD) 					
	LCD_SetMainInfoEx(E_IT_PrintZOrigin, &point);
#endif

#ifdef FULGENCY_FUN
	Z_SAFE_Dirty = True;
#endif

	if(bFlashPM)
	{
		((INT16U*)Buff)[0] = EP6_CMD_T_MAINUI;
		mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
		mainUIPara_ToPM.PrintYOrigin = LCDMenuConfig_EX.OriginY;
		mainUIPara_ToPM.PrintZOrigin = LCDMenuConfig_EX.OriginZ;
		mainUIPara_ToPM.PassNum = mainUIPara.PassNum;
		mainUIPara_ToPM.StepModify = mainUIPara.StepModify;
		memcpy(&Buff[2], &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_MainUI_Param), True))
			ret = False;    
	}
	return ret;	
}
INT8U SetBasePoint_All(void * pData, float pointX, float pointY, float pointZ, INT8U bGoHome, INT8U bFlashPM)
{
	U8 err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	
	float width;
	float length;
	float high;
	if(pointX >= PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)
		pointX = PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH;
	if(pointX < 0.0)
		pointX = 0.0;
	
	if(pointY< 0.0)
		pointY = 0.0;
	
#ifdef FULGENCY_FUN	
	if(pointZ < 1.5/25.4)
		pointZ = 1.5/25.4;
#else
	if(pointZ < 0.0)
		pointZ = 0.0;
#endif
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(pointX >= PAPER_MEDIA_WIDTH_INCH)
		pointX = PAPER_MEDIA_WIDTH_INCH;
	if(pointX >= LCDMenuConfig.MediaWidth - MIN_MEDIA_WIDTH)
		pointX = LCDMenuConfig.MediaWidth- MIN_MEDIA_WIDTH;
	
	if(pointY>= LCDMenuConfig_EX.MediaL)
		pointY= LCDMenuConfig_EX.MediaL;
	
	LCDMenuConfig.OriginX = pointX;
	LCDMenuConfig_EX.OriginY = pointY;
	LCDMenuConfig_EX.OriginZ = pointZ;
	
	OSSemPost(LCDMenuConfigureSem);

#ifdef FULGENCY_FUN
	Z_SAFE_Dirty = True;
#endif
	
	SaveLCDMenuConfig();
	SaveLCDMenuConfig_EX();	
	
	if(bGoHome)
		UIAction_X_GoHome(pData);
#if defined( IIC_Key_Board_LCD) 				
	LCD_SetMainInfoEx(E_IT_PrintOrigin, &pointX);
	LCD_SetMainInfoEx(E_IT_PrintYOrigin, &pointY);
	LCD_SetMainInfoEx(E_IT_PrintZOrigin, &pointZ);
#endif

	if(bFlashPM)
	{
		((INT16U*)Buff)[0] = EP6_CMD_T_MAINUI;
		mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
		mainUIPara_ToPM.PrintYOrigin = LCDMenuConfig_EX.OriginY;
		mainUIPara_ToPM.PrintZOrigin = LCDMenuConfig_EX.OriginZ;
		mainUIPara_ToPM.PassNum = mainUIPara.PassNum;
		mainUIPara_ToPM.StepModify = mainUIPara.StepModify;
		memcpy(&Buff[2], &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_MainUI_Param), True))
			ret = False;
		
		((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
		mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
		mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
		mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
		mediaInfo_ToPM.Prt_Y_Continue = mediaInfo.Prt_Y_Continue =  LCDMenuConfig_EX.PRINT_Y_CONTINUE;
		mediaInfo_ToPM.MediaLength = mediaInfo.MediaLength =  LCDMenuConfig_EX.MediaL;	
		memcpy(&mediaInfo, &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
		memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
		if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
			ret = False;
	}
	return ret;
}
#endif
#ifdef SET_ORIGIN_OFFSET
extern INT32S Media_sensor_offset;
#endif
INT8U UIAction_SetBasePoint(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#ifdef SET_ORIGIN_OFFSET
		return SetBasePoint(pData, (float)(curPos.x - printer.platSpace + Media_sensor_offset)/X_BASE_RES, True, True);
#else
		return SetBasePoint(pData, (float)(curPos.x - printer.platSpace)/X_BASE_RES, True, True);
#endif
	}
	else
		return False;
}
#ifdef RIPSTAR_FLAT_EX
INT8U UIAction_SetBasePoint_Y(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if( curPos.y > Y_PLAT_LENGTH)
			return SetBasePointY(pData, (float)(curPos.y -Y_PLAT_LENGTH )/DYSS_Y_PWM_RATIO, False, True);
		else
			return SetBasePointY(pData, 0, False, True);
	}
	else
		return False;
}

INT8U UIAction_SetBasePoint_Z(void * pData)
{
	U8 err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		return SetBasePointZ(pData, (float)curPos.z/Z_BASE_RES, False, True);
	}
	else
		return False;
}

#endif
#if defined(MICOLOR_AUTOFUCTION) || defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U UIAction_SheetCut(void * pData)
{
	U8 err;
	
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		CMD_Epson_Operate_Type = CMD_EPSON_T_SHEET_CUT;
		status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
		return True;
	}
	else
		return False;
}
#endif

INT8U UIAction_CheckNozzle(void * pData)
{
	return Cali_UM_CheckNozzle(pData,0);
}

#ifdef FUNC_MAINTAIN
INT8U UIAction_Maintain(void *pData)
{
	U8 err;
	
	if((bStatus_Maintain & 0x01))	//维护状态
		bStatus_Maintain &= ~0x01;	//非维护
	else
		bStatus_Maintain |= 0x01;	//维护

	status_ReportStatus(STATUS_RDY, STATUS_SET);
	return True;
}
#endif
// sheet type
//Index: 0, transparent, 1, OPAQUE.
INT8U Menu_SetMediaOption(void * pData, int Index, int Data)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	Index = Data;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(!Index)
		LCDMenuConfig.SheetIsTransparent = True;
	else
		LCDMenuConfig.SheetIsTransparent = False;
	OSSemPost(LCDMenuConfigureSem);
	
	SaveLCDMenuConfig();
	return True;
}

INT8U Menu_GetMediaOption(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.SheetIsTransparent;
	OSSemPost(LCDMenuConfigureSem);
	return !ret;
}

// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Menu_SetMediaType(void * pData, int Index, int Data)
{
	U8 err;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	Index = Data;
	if(Index >6 || Index < 0)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.SheetType = Index;
	OSSemPost(LCDMenuConfigureSem);
	
	OSSemPend(CalibrationParamSem, 0, &err);
	bCaliSettingDirty = True;
	OSSemPost(CalibrationParamSem);
	
	SaveLCDMenuConfig();
	return True;
}

INT8U Menu_GetMediaType(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.SheetType;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

//Origin & Width
INT8U Menu_SetBasePoint(void * pData, int Index)
{
	INT8U err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_MOVING|STATUS_PRINT|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		return SetBasePoint(pData, (float)(curPos.x - printer.platSpace)/720, True, True);
	else
		return False;
}

INT8U Menu_InputBasePoint(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return True;
	
	return SetBasePoint(pData, Data, False, True);
}

float Menu_GetBasePoint(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.OriginX;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
#ifdef RIPSTAR_FLAT_EX

INT8U Menu_Move_Zpoint(void * pData, int Index)
{
	INT8U err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		move_dir = 4; 		
		move_distance = LCDMenuConfig_EX.OriginZ*Z_BASE_RES + Z_PLAT_LENGTH;
		move_type = 3;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);					
		return True;
	}
}
INT8U GetCalcParam_RICOH_X_SPEED(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(CalibrationParamSem, 0, &err);
	ret = LCDMenuConfig_EX.Calc_X_Speed;
	OSSemPost(CalibrationParamSem);
	return ret;
}

INT8U SetCalcParam_RICOH_X_SPEED(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//Index = Data;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(Index>=0 && Index< MAX_SPEED_LEVEL_EACH_VSD)
		LCDMenuConfig_EX.Calc_X_Speed = Index;
	else
		ret = False;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();
#if defined( IIC_Key_Board_LCD) 					
	LCD_SetMainInfoEx(E_IT_HeadHeightLevel, 0);	
#endif
	return ret;
}

//return value: 0, off, 1, on.
INT8U GetPRTParam_Y_PRT_CONTINUE_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig_EX.PRINT_Y_CONTINUE;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U SetPRTParam_Y_PRT_CONTINUE_OnOff(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//  Index = Data;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(Index>=0 && Index<2)
	{
		LCDMenuConfig_EX.PRINT_Y_CONTINUE = Index;
	}
	else
		ret = False;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();
	
	((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth =  LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	mediaInfo_ToPM.Prt_Y_Continue = mediaInfo.Prt_Y_Continue = LCDMenuConfig_EX.PRINT_Y_CONTINUE;
	mediaInfo_ToPM.MediaLength = mediaInfo.MediaLength =  LCDMenuConfig_EX.MediaL;	
	memcpy(&mediaInfo, &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
		ret = False;
	
	return ret;
}


INT8U Menu_InputBaseYPoint(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return True;
	
	return SetBasePointY(pData, Data, False, True);
}

float Menu_GetBaseYPoint(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig_EX.OriginY;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
#ifdef MANUFACTURER_DYSS	
INT8U Menu_InputBaseY_CAL_EX(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return True;
	
	LCDMenuConfig_EX.add[Index] = Data;
	return SaveLCDMenuConfig_EX();
}

float Menu_GetBaseY_CAL_EX(void * pData, int Index)
{
	U8 err;
	
	float ret;
	
	ret = LCDMenuConfig_EX.add[Index];
	
	return ret;
}
#endif
INT8U Menu_InputBaseZPoint(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return True;
	
	return SetBasePointZ(pData, Data, False, True);
}

float Menu_GetBaseZPoint(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig_EX.OriginZ;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
INT8U Menu_SetBasePoint_all(void * pData, int Index)
{
	INT8U err;
	if (OSFlagAccept(status_FLAG_GRP, STATUS_MOVING|STATUS_PRINT|STATUS_CLEANING, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		return SetBasePoint_All(pData, (float)(curPos.x - printer.platSpace)/X_BASE_RES,(float)curPos.y/DYSS_Y_PWM_RATIO,(float)curPos.z/Z_BASE_RES, True, True);
	else
		return False;
}
#endif
INT8U GetPRTParam_Head_Encry_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(LCDMenuConfig_EX.Head_Encry_Mask &(1<<Index))
		ret = True;
	else
		ret = False;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U SetPRTParam_Head_Encry_OnOff(void * pData, int Index, INT8U mask)
{
	INT8U err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_CLEANING | CMD_CLEAN_AUTO, OS_FLAG_WAIT_CLR_ALL, &err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//  Index = Data;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(Index == 0)
		LCDMenuConfig_EX.Head_Encry_Mask &= ~(1<<mask);
	else 
		LCDMenuConfig_EX.Head_Encry_Mask |= (1<<mask);
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();
		
	return ret;
}

INT8U GetPRTParam_Head1_Encry_OnOff(void * pData, int Index)
{
	return GetPRTParam_Head_Encry_OnOff(pData, 0);
}
INT8U GetPRTParam_Head2_Encry_OnOff(void * pData, int Index)
{
	return GetPRTParam_Head_Encry_OnOff(pData, 1);
}
INT8U SetPRTParam_Head1_Encry_OnOff(void * pData, int Index, int Data)
{
	return SetPRTParam_Head_Encry_OnOff(pData,Index,0);
}
INT8U SetPRTParam_Head2_Encry_OnOff(void * pData, int Index, int Data)
{
	return SetPRTParam_Head_Encry_OnOff(pData,Index,1);
}
#ifdef EPSON_4H
INT8U GetPRTParam_Head3_Encry_OnOff(void * pData, int Index)
{
	return GetPRTParam_Head_Encry_OnOff(pData, 2);
}
INT8U GetPRTParam_Head4_Encry_OnOff(void * pData, int Index)
{
	return GetPRTParam_Head_Encry_OnOff(pData, 3);
}
INT8U SetPRTParam_Head3_Encry_OnOff(void * pData, int Index, int Data)
{
	return SetPRTParam_Head_Encry_OnOff(pData,Index,2);
}
INT8U SetPRTParam_Head4_Encry_OnOff(void * pData, int Index, int Data)
{
	return SetPRTParam_Head_Encry_OnOff(pData,Index,3);
}
#endif
INT8U Menu_InputMediaWidth(void * pData, int Index, float Data)
{
	U8 err, ret = True;
	INT8U Buff[1 + sizeof(struct USB_RPT_Media_Info)];
	struct USB_RPT_Media_Info *pMediaInfo = (struct USB_RPT_Media_Info *)&Buff[1];
	float width = Data;
	
	if(pData == (void *)-1)
		return True;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(width >= PAPER_MEDIA_WIDTH_INCH )
	{
		width = PAPER_MEDIA_WIDTH_INCH;
	}
	if(width < MIN_MEDIA_WIDTH)
		width = MIN_MEDIA_WIDTH;
	if(width <= LCDMenuConfig.OriginX)
		width = LCDMenuConfig.OriginX;
	
	LCDMenuConfig.MediaWidth = width;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
#if defined( IIC_Key_Board_LCD) 					
	LCD_SetMainInfoEx(E_IT_MediaWidth, &width);
#endif	
	((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	
#ifdef RIPSTAR_FLAT_EX
	mediaInfo_ToPM.Prt_Y_Continue = mediaInfo.Prt_Y_Continue = LCDMenuConfig_EX.PRINT_Y_CONTINUE;
	mediaInfo_ToPM.MediaLength = mediaInfo.MediaLength =  LCDMenuConfig_EX.MediaL;	
#endif	
	memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
		ret = False;
	
	return ret;
}

float Menu_GetMediaWidth(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.MediaWidth;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

#ifdef RIPSTAR_FLAT_EX
INT8U Menu_InputMediaLength(void * pData, int Index, float Data)
{
	U8 err, ret = True;
	float width = Data;
	INT8U Buff[1 + sizeof(struct USB_RPT_Media_Info)];
	if(pData == (void *)-1)
		return True;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	
	if(width < MIN_MEDIA_WIDTH)
		width = MIN_MEDIA_WIDTH;
	if(width <= LCDMenuConfig_EX.OriginY)
		width = LCDMenuConfig_EX.OriginY;
	
	LCDMenuConfig_EX.MediaL= width;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();	
#if defined( IIC_Key_Board_LCD) 					
	LCD_SetMainInfoEx(E_IT_MediaLength, &width);
#endif	
	((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth =  LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	mediaInfo_ToPM.Prt_Y_Continue = mediaInfo.Prt_Y_Continue = LCDMenuConfig_EX.PRINT_Y_CONTINUE;
	mediaInfo_ToPM.MediaLength = mediaInfo.MediaLength =  LCDMenuConfig_EX.MediaL;	
	memcpy(&mediaInfo, &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
		ret = False;        
	return ret;
}

float Menu_GetMediaLength(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig_EX.MediaL;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
INT8U Menu_InputMediaHigh(void * pData, int Index, float Data)
{
	U8 err, ret = True;
	float width = Data;
	
	if(pData == (void *)-1)
		return True;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(width <= LCDMenuConfig_EX.OriginZ)
		width = LCDMenuConfig_EX.OriginZ;
	
	LCDMenuConfig_EX.MediaH = width;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig_EX();	
#if defined( IIC_Key_Board_LCD) 				
	LCD_SetMainInfoEx(E_IT_MediaHigh, &width);
#endif
	
	return ret;
}

float Menu_GetMediaHigh(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig_EX.MediaH;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
#endif
#if defined(MICOLOR_AUTOFUCTION)
INT8U Menu_LoadMedia(void * pData, int Index)
{
	U8 err;
	
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(!IsLoadMediaSensorValid())
		{
			CMD_Epson_Operate_Type = CMD_EPSON_T_LOAD_MEDIA;
			status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
		}
		return True;
	}
	else
		return False;
}
#endif

INT8U SetMediaInfo(float origin, float width, float margin)
{
	U8 err;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(origin >= PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)
		origin = PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH;
	else if (origin<0.0)
		origin = 0.0;
	
	if(origin + width >= PAPER_MEDIA_WIDTH_INCH)
		width = PAPER_MEDIA_WIDTH_INCH - origin;
	else if(width < MIN_MEDIA_WIDTH)
		width = MIN_MEDIA_WIDTH;
	
	if(margin >= (PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)/2)
		margin = (PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)/2;
	
	LCDMenuConfig.OriginX = origin;
	LCDMenuConfig.MediaWidth = width+origin;
	LCDMenuConfig.Margin = margin;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	mediaInfo.MediaOrigin = origin;
	mediaInfo.MediaWidth = width;
	mediaInfo.Margin = margin;
#if defined( IIC_Key_Board_LCD) 			
	LCD_SetMainInfoEx(E_IT_PrintOrigin, &origin);
	LCD_SetMainInfoEx(E_IT_MediaWidth, (void *)&(LCDMenuConfig.MediaWidth));
#endif	
	return True;
}

INT8U SetMeasureResult(	INT32S paper_X_NearEdgePos, INT32S paper_X_FarEdgePos)
{
	U8 err, ret = True;
	INT8U Buff[2+(sizeof(struct USB_RPT_MainUI_Param)>sizeof(struct USB_RPT_Media_Info) ? 
				  sizeof(struct USB_RPT_MainUI_Param):sizeof(struct USB_RPT_Media_Info))];
	struct USB_RPT_MainUI_Param *pMainUI = (struct USB_RPT_MainUI_Param *)&Buff[2];
	struct USB_RPT_Media_Info *pMediaInfo = (struct USB_RPT_Media_Info *)&Buff[2];
	
	float origin, width;
#if defined(SUPPORT_MOTOR_CONTROL) 
	const INT32S sensoroffset = 0; //motion system had removed sensor offset.
#elif defined(MANUFACTURER_ALLWIN_EPSON) 
	const INT32S sensoroffset = 2020 + LCDMenuConfig.MeasureSensorOffset* X_BASE_RES/2.54;
#elif defined(DGEN_AUTOFUCTION) 
	const INT32S sensoroffset = 1650 + LCDMenuConfig.MeasureSensorOffset* X_BASE_RES/2.54;
#else    
	const INT32S sensoroffset = 1130 + LCDMenuConfig.MeasureSensorOffset* X_BASE_RES/2.54;
#endif
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	
	width = ((float)paper_X_FarEdgePos + sensoroffset - printer.platSpace)/720 - LCDMenuConfig.Margin;
	if(width > PAPER_MEDIA_WIDTH_INCH )
		width = PAPER_MEDIA_WIDTH_INCH;
	else if(width < MIN_MEDIA_WIDTH)
		width = MIN_MEDIA_WIDTH;
	
	origin = ((float)paper_X_NearEdgePos + sensoroffset - printer.platSpace)/720 + LCDMenuConfig.Margin;
	if(origin >= width )
		origin = width;
	else if(origin < 0)
		origin = 0.0;
	
	LCDMenuConfig.OriginX = origin;
	LCDMenuConfig.MediaWidth = width;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
#if defined( IIC_Key_Board_LCD) 		
	LCD_SetMainInfoEx(E_IT_PrintOrigin, &origin);
	LCD_SetMainInfoEx(E_IT_MediaWidth, &width);
#endif

	((INT16U*)Buff)[0] = EP6_CMD_T_MAINUI;
	mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
	mainUIPara_ToPM.PassNum = 4;
	mainUIPara_ToPM.StepModify = 0;
	memcpy(&Buff[2], &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_MainUI_Param), True))
		ret = False;
	
	((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
		ret = False;
	
	return ret;
}

INT8U Menu_MeasureMedia(void * pData, int Index)
{
	return UIAction_MeasureMedia(pData, 0);
}

INT8U Menu_MachineTest(void * pData, int Index)
{
	INT8U err;
	INT8U buf[32];
	
	//#ifdef MICOLOR_AUTOFUCTION
	//    if(!((!LCDMenuConfig.AutoLackMediaCheck || !IsOutOfMedia()) && (!LCDMenuConfig.AutoFixCheck || IsMediaFixed()) && (!LCDMenuConfig.AutoCoverCheck || IsCoverSensorValid())))
	//        return False;
	//#endif
	
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
#if defined (EPSON_CLEAN_UPDOWN)
		UnCapping(); //waiting action completed
#endif
		
		CMD_Epson_Operate_Type = CMD_EPSON_T_MACHIME_TEST;
		status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
		return True;
	}
	else
		return False;
}

#if defined(MICOLOR_AUTOFUCTION) || defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U Menu_SheetCut(void * pData, int Index)
{
	return UIAction_SheetCut(pData);
}
#endif

#if defined(DGEN_AUTOFUCTION)
INT8U Menu_DGEN_Move_Paper(void * pData, int Index)
{
    U8 err;
    if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_Y_MOVE_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
    {
        CMD_Epson_Operate_Type = CMD_EPSON_T_DGEN_MOVE_PAPER;
        status_ReportStatus(CMD_EPSON_OPERATION, STATUS_SET);					
        return True;
    }
    else
        return False;
}
INT8U SetMove_Paper_Speed(void * pData, int Index, int Data)
{
    INT8U i, tmp[32];
    INT8U err;
    if(pData == (void *)-1)
    {
    	if(CMD_Epson_Operate_Type != CMD_EPSON_T_DGEN_MOVE_PAPER)
            return True;
	else
            return False;
    }
    if(Data > 100)
        Data = 100;
    else if(Data < 1)
        Data =1;
    LCDMenuConfig_EX.Move_Paper_Speed_Percent = Data;
	SaveLCDMenuConfig_EX();
    return True;
}
INT32S GetMove_Paper_Speed(void * pData, int Index)
{

    return LCDMenuConfig_EX.Move_Paper_Speed_Percent;
}
#endif
INT8U Menu_SetMargin(void * pData, int Index, float Data)
{
	U8 err, ret = True;
	INT8U Buff[1 + sizeof(struct USB_RPT_Media_Info)];
	struct USB_RPT_Media_Info *pMediaInfo = (struct USB_RPT_Media_Info *)&Buff[1];
	
	if(pData == (void *)-1)
		return True;
	
	if(Data >= (PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)/2)
		Data = (PAPER_MEDIA_WIDTH_INCH - MIN_MEDIA_WIDTH)/2;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.Margin = Data;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	((INT16U*)Buff)[0] = EP6_CMD_T_MEDIA;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	memcpy(&Buff[2], &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
	
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_RPT_Media_Info), True))
		ret = False;
	
	return ret;
}

float Menu_GetMargin(void * pData, int Index)
{
	U8 err;
	
	float ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.Margin;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

//Index: 0, OFF, 1, ON.
#if defined(MICOLOR_AUTOFUCTION)


INT8U Menu_SetAutoCut(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoCut = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoCut(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoCut;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoMeasureWidth(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoMeasure = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoMeasureWidth(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoMeasure;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoLoadMedia(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoLoad = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoLoadMedia(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoLoad;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoOutofMedia(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoLackMediaCheck = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoOutofMedia(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoLackMediaCheck;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoCoverCheck(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoCoverCheck = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoCoverCheck(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoCoverCheck;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoPlateFan(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoPlateFan = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoPlateFan(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoPlateFan;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoFixCheck(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoFixCheck = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoFixCheck(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoFixCheck;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoPowerOnClean(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoPowerOnClean = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoPowerOnClean(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoPowerOnClean;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoGoHome(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoGoHome = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoGoHome(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoGoHome;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

//Index: 0, Strong; 1, week; 2, off.
INT8U Menu_SetPlateFanMode(void * pData, int Index, int Data)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index < 0 && Index > 2)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.PlateFanMode = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetPlateFanMode(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.PlateFanMode;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

#endif

#ifdef MANUFACTURER_DYSS
//Index: 0, Strong; 1, week; 2, off.
INT8U Menu_SetPlateFanMode(void * pData, int Index, int Data)
{
	U8 err;
	
	if(pData == (void *)-1)
	{
		return True;
	}
	
	if(Index < 0 && Index > 2)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.PlateFanMode = Index;
	OSSemPost(LCDMenuConfigureSem);
	
	if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_OPEN)
		OpenPlateFan();
	else if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_AUTO)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			OpenPlateFan();
		else
			ClosePlateFan();
	}
	else
		ClosePlateFan();
	
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetPlateFanMode(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.PlateFanMode;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}	
#endif

INT8U Menu_SetAutoStep(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.AutoStep = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoStep(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.AutoStep;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}

INT8U Menu_SetAutoFlashBeforePrint(void * pData, int Index)
{
	U8 err;
	
	if(pData == (void *)-1)
		return True;
	
	if(Index != 0 && Index != 1)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(Index != 0)
		LCDMenuConfig.cfg_bit |= AUTO_FLASH_BEFORE_PRINT;
	else
		LCDMenuConfig.cfg_bit &= ~AUTO_FLASH_BEFORE_PRINT;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
	
	return True;
}

INT8U Menu_GetAutoFlashBeforePrint(void * pData)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	if(AUTO_FLASH_BEFORE_PRINT&LCDMenuConfig.cfg_bit)
		ret = 1;
	else
		ret = 0;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
INT8U SetPrintQuality(int Index)
{
	U8 err;
	
	if(Index <0 || Index > 255)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	LCDMenuConfig.PrintQuality = Index;
	OSSemPost(LCDMenuConfigureSem);
	SaveLCDMenuConfig();
#if defined( IIC_Key_Board_LCD) 	
	LCD_SetMainInfoEx(E_IT_QualitySetting, &Index);
#endif
	return True;
}

#if 0
//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
INT8U Menu_SetPrintQuality(void * pData, int Index, int Data)
{
	U8 ret = True;
	INT8U Buff[2+sizeof(struct USB_Print_Quality)];
	
	if(pData == (void *)-1)
		return True;
	
	Index = Data;
	
	if( Index < 0 || Index > 3)
		return False;
	
	ret = SetPrintQuality(Index);
	
	((INT16U*)Buff)[0] = EP6_CMD_T_PRINTQUALITY;
	printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
	memcpy(&Buff[2], &printQuality_ToPM, sizeof(printQuality_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_Print_Quality), True))
		ret = False;
	
	return ret;
}

INT8U Menu_GetPrintQuality(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.PrintQuality;
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
#endif

INT8U Menu_SetPrintQualityLevel(void * pData, int Index, int Data)
{
	U8 ret = True;
	INT8U Buff[2+sizeof(struct USB_Print_Quality)];
	int curPrintQualityType = 0;
	int tmpIndex = 0;
	
	if(pData == (void *)-1)
		return True;
	
	Index = Data;
	
	if( Index < 0 || Index > 3)
		return False;
	
	curPrintQualityType = Menu_GetPrintQualityType(0, 0);
	tmpIndex = (curPrintQualityType <<4) | Index;
	
	ret = SetPrintQuality(tmpIndex);
	
	((INT16U*)Buff)[0] = EP6_CMD_T_PRINTQUALITY;
	printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
	memcpy(&Buff[2], &printQuality_ToPM, sizeof(printQuality_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_Print_Quality), True))
		ret = False;
	
	return ret;
}

INT8U Menu_GetPrintQualityLevel(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.PrintQuality;
	OSSemPost(LCDMenuConfigureSem);

	return ret & 0x0f;
}

INT8U Menu_SetPrintQualityType(void * pData, int Index, int Data)
{
	U8 ret = True;
	INT8U Buff[2+sizeof(struct USB_Print_Quality)];
	int curPrintQualityLevel = 0;
	int tmpIndex = 0;
	
	if(pData == (void *)-1)
		return True;
	
	Index = Data;
	
	if( Index < 0 || Index > 5)
		return False;
	
	curPrintQualityLevel = Menu_GetPrintQualityLevel(0, 0);
	tmpIndex = curPrintQualityLevel | (Index << 4);
	
	ret = SetPrintQuality(tmpIndex);
	
	((INT16U*)Buff)[0] = EP6_CMD_T_PRINTQUALITY;
	printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;
	memcpy(&Buff[2], &printQuality_ToPM, sizeof(printQuality_ToPM));
	if(!PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, Buff, 2+sizeof(struct USB_Print_Quality), True))
		ret = False;
	
	return ret;
}

INT8U Menu_GetPrintQualityType(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	ret = LCDMenuConfig.PrintQuality;
	OSSemPost(LCDMenuConfigureSem);

	return (ret & 0xf0) >> 4;
}

INT8U SetMenu_EPSON_ALLWIN_1HeadPrint_OnOff(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//  Index = Data;
	
	
	if(Index>=0 && Index<2)
	{
		factoryDataEx.Only_Used_1head = Index;
	}
	else
		ret = False;
	
	if(ret)
		SaveFactoryDataEx_EPSON(&factoryDataEx);
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetMenu_EPSON_ALLWIN_1HeadPrint_OnOff(void * pData, int Index)
{
	INT8U err, ret;
	ret = factoryDataEx.Only_Used_1head;
	return ret;
}
INT8U SetMenu_EPSON_ALLWIN_SELECTHeadPrint(void * pData, int Index, int Data)
{
	INT8U err, ret = True;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	//Index = Data;
	
	if(Index>=0 && Index<2)
	{
		factoryDataEx.Mask_head_used = Index + 1;
	}
	else
		ret = False;
	if(ret)
		SaveFactoryDataEx_EPSON(&factoryDataEx);
	return ret;
}
//return value: 0, off, 1, on.
INT8U GetMenu_EPSON_ALLWIN_SELECTHeadPrint(void * pData, int Index)
{
	INT8U err, ret;
	
	ret = factoryDataEx.Mask_head_used - 1;
	
	return ret;
}

INT8U IIC_WriteEEPRom_Head(INT16U addr, EPM_Head *buf)
{
	INT8U err;
	INT8U curlen = sizeof(EPM_Head);
	OSSemPend(IICSem, 0, &err);
	if (IIC_WriteEEPRom(addr, (INT8U*)buf, &curlen) == False)
	{
		OSSemPost(IICSem);
		return False;
	}
	OSSemPost(IICSem);
	return curlen == sizeof(EPM_Head);
}

INT8U IIC_ReadEEPRom_Head(INT16U addr, EPM_Head *buf)
{
	INT8U err;
	INT8U curlen = sizeof(EPM_Head);
	OSSemPend(IICSem, 0, &err);
	if (IIC_ReadEEPRom(addr, (INT8U*)buf, &curlen) == False)
	{
		OSSemPost(IICSem);
		return False;
	}
	OSSemPost(IICSem);
	return curlen == sizeof(EPM_Head);
}

INT8U IIC_ReadEEPRom_Struct(INT16U addr, INT8U * buf, INT16U bufsize, EPM_Head *phead)
{
	INT8U ret;
	INT16U len = phead->structsize - sizeof(EPM_Head), len_backup;
	INT16U checksum = 0;
	
	if(len > bufsize)
		len = bufsize;
	len_backup = len;
	ret = IIC_ReadEEPRom_Ex(addr, buf,&len);
	
	if(ret == False || len != len_backup)
		return False;
	for(len = 0; len < sizeof(EPM_Head)/sizeof(INT16U); len ++)
	{
		checksum += ((INT16U*)phead)[len];
	}
	for(len = 0; len < len_backup/sizeof(INT16U); len ++)
	{
		checksum += ((INT16U*)buf)[len];
	}
	
	if( phead->structsize - sizeof(EPM_Head) > bufsize)
	{
		INT16U tempbuf[16];
		
		bufsize = bufsize/sizeof(INT16U)*sizeof(INT16U);
		addr += bufsize;
		len = phead->structsize - sizeof(EPM_Head) - bufsize;
		
		while(len > 0)
		{
			INT16U curlen = len;
			INT16U i;
			if( curlen > sizeof(tempbuf))
				curlen = sizeof(tempbuf);
			
			len_backup = curlen;
			ret = IIC_ReadEEPRom_Ex(addr, (INT8U*)tempbuf, &curlen);
			if( ret == False || curlen != len_backup)
				return False;
			
			len -= curlen;
			
			for(i = 0; i < curlen/sizeof(INT16U); i ++)
			{
				checksum += ((INT16U*)tempbuf)[i];
			}
			if( curlen % sizeof(INT16U) != 0)
			{
				tempbuf[i] &= 0xFF;
				checksum += tempbuf[i];
			}
		}
	}
	else
	{
		if(len * sizeof(INT16U) < len_backup)
		{
			checksum += (INT16U) buf[len* sizeof(INT16U)];
		}
	}
	return checksum == 0;
}

INT16U CalcCheckSum(INT8U * buf, EPM_Head * phead)
{
	INT16U len_backup = phead->structsize - sizeof(EPM_Head), len;
	INT16U checksum = 0;
	
	phead->Checksum = 0;
	
	for(len = 0; len < sizeof(EPM_Head)/sizeof(INT16U); len ++)
	{
		checksum += ((INT16U*)phead)[len];
	}
	for(len = 0; len < len_backup/sizeof(INT16U); len ++)
	{
		checksum += ((INT16U*)buf)[len];
	}
	if(len * sizeof(INT16U) < len_backup)
	{
		checksum += (INT16U) buf[len* sizeof(INT16U)];
	}
	phead->Checksum = - checksum;
	return phead->Checksum;
}
#ifdef MANUFACTURER_DYSS
#define LEDD14DEFINE 0x36
void changeLeddisplay(INT8U state)
{
	INT8U buf[2] = {0};
	
	buf[0] = 0x1; 
	buf[1] = LEDD14DEFINE;
	if(state == 1)
	{
		buf[1] |= (1<<7);
	}
	IIC_WriteRegs(0x7, buf, 2);
}
#endif

#if defined(QICAIHONG)

void Control_OpenCose_Led(INT8U Index, INT8U state)
{
	#define LED_NUM 3
	INT8U buf[2] = 0;
	INT8U led_addr[LED_NUM] = {9, 18, 27};

	
	buf[0] = 0x1; 
	buf[1] = led_addr[Index];
	if(state == True)
	{
		buf[1] |= (1<<7);
	}
	IIC_WriteRegs(0x7, buf, 2);
}
#endif


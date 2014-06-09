#ifndef _UV_C_
#define _UV_C_
#include "global.h"
#include "uv.h"
#include "status.h"
#include "FPGA.h"

#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
#include "motion.h"
#endif

#ifdef COORD_NEW_UV
UV_POSI_TYPE UV_POSI = {0};
#endif

#ifdef UV_PRINTER

const Pin UVCtrlPins[] = {MB_CUST_PINS_UV_CTRL}; 
#if !(defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1))
const Pin UVStatusPins[] = {MB_CUST_PINS_UV_STAT}; 
#ifndef DYSS_UV_CLEAN
const Pin UVStatPinLP1ShCl = PIN_UV_STAT_LP1_SH_CL_I; 
const Pin UVStatPinLP1ShOp = PIN_UV_STAT_LP1_SH_OP_I; 
const Pin UVStatPinLP2ShCl = PIN_UV_STAT_LP2_SH_CL_I;
const Pin UVStatPinLP2ShOp = PIN_UV_STAT_LP2_SH_OP_I; 
#endif
const Pin UVStatPinUVRdy = PIN_UV_STAT_UV_RDY;
const Pin UVStatPinLP1Fault = PIN_UV_STAT_LP1_FAULT;
const Pin UVStatPinLP2Fault = PIN_UV_STAT_LP2_FAULT;
#endif

#if !defined(MANUFACTURER_DYSS)&&!defined(OLD_UV)
const Pin UVCtrlPinLP1On = PIN_UV_CTRL_LP1_ON;
const Pin UVCtrlPinLP2On = PIN_UV_CTRL_LP2_ON;
const Pin UVCtrlPinLP1HighPower = PIN_UV_CTRL_LP1_HIGHPOWER;
const Pin UVCtrlPinLP2HighPower = PIN_UV_CTRL_LP2_HIGHPOWER;

#endif

#if defined(OLD_UV)
#if defined(MANUFACTURER_DYSS)
Pin UVCtrlPinSH1Open = PIN_UV_CTRL_SHUTTER_1_OPEN;
Pin UVCtrlPinSH2Open = PIN_UV_CTRL_SHUTTER_2_OPEN;
Pin UVCtrlPinLP2On = PIN_UV_CTRL_LP2_ON;
Pin UVCtrlPinLP2HighPower = PIN_UV_CTRL_LP2_HIGHPOWER;
Pin UVCtrlPinLP1On = PIN_UV_CTRL_LP1_ON;
Pin UVCtrlPinLP1HighPower = PIN_UV_CTRL_LP1_HIGHPOWER;
#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)
Pin UVCtrlPinLP2On = PIN_UV_CTRL_LP2_ON;
Pin UVCtrlPinLP2HighPower = PIN_UV_CTRL_LP2_HIGHPOWER;
Pin UVCtrlPinLP1On = PIN_UV_CTRL_LP1_ON;
Pin UVCtrlPinLP1HighPower = PIN_UV_CTRL_LP1_HIGHPOWER;
Pin STATUS_PRT = PIN_UV_CTRL_SHUTTER_1_OPEN;
Pin Y_HOME_READY = PIN_UV_CTRL_SHUTTER_2_OPEN;
const Pin UVCtrlPinSH1Open = FULGENCY_UV_OUTPUT_J19;
const Pin UVCtrlPinSH2Open = FULGENCY_UV_OUTPUT_J22;
const Pin FULGENCY_WASTE_INK = PIN_FULGENCY_WASTE_INK;
#else
Pin UVCtrlPinSH1Open = PIN_UV_CTRL_SHUTTER_1_OPEN;
Pin UVCtrlPinSH2Open = PIN_UV_CTRL_SHUTTER_2_OPEN;
Pin UVCtrlPinLP2On = PIN_UV_CTRL_LP2_ON;
Pin UVCtrlPinLP2HighPower = PIN_UV_CTRL_LP2_HIGHPOWER;
Pin UVCtrlPinLP1On = PIN_UV_CTRL_LP1_ON;
Pin UVCtrlPinLP1HighPower = PIN_UV_CTRL_LP1_HIGHPOWER;
#endif
#elif defined(MANUFACTURER_TATE_EPSON_UV)&&defined(HEAD_RICOH_G4)
const Pin UVCtrlPinSH1Open = PIN_CLEAN_SUCK_1;//InkSupplyCtrl10Pin 
const Pin UVCtrlPinSH2Open = PIN_CLEAN_RELEASE_1;//InkSupplyCtrl9Pin 
#elif (defined(MANUFACTURER_TATE_EPSON_UV) ||defined(MANUFACTURER_HUMAN_FLAT_EPSON)||defined(MANUFACTURER_RIPSTAR_TEST_UV)||defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H))&&defined(HEAD_EPSON_GEN5)
const Pin UVCtrlPinSH1Open = PIN_INK_SUPPLY_CTRL1;
const Pin UVCtrlPinSH2Open = PIN_INK_SUPPLY_CTRL2;
#else
const Pin UVCtrlPinSH1Open = PIN_UV_CTRL_SHUTTER_1_OPEN;
const Pin UVCtrlPinSH2Open = PIN_UV_CTRL_SHUTTER_2_OPEN;
#endif

#ifdef MANUFACTURER_DOCAN_UV
const Pin UVCtrl_MOTION_OUTPUT1 = DOCAN_UV_MOTION_OUTPUT1;
const Pin UVCtrl_MOTION_OUTPUT2 = DOCAN_UV_MOTION_OUTPUT2;
#endif
const Pin DSPUvControl1Pin = PIN_DSP_UV_CONTROL_1;
const Pin DSPUvControl2Pin = PIN_DSP_UV_CONTROL_2;

#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
INT8U	UV_STATUS_LP1_FAULT()
{
	OS_CPU_SR cpu_sr;
	INT16U io;

	OS_ENTER_CRITICAL();
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if( io & FPGA_EXPAND_IOMASK_UV_L1_FAULT)
		return True;
	else
		return False;
}
INT8U	UV_STATUS_LP2_FAULT()
{
	OS_CPU_SR cpu_sr;
	INT16U io;

	OS_ENTER_CRITICAL();
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if( io & FPGA_EXPAND_IOMASK_UV_L2_FAULT)
		return True;
	else
		return False;
}
INT8U	UV_STATUS_READY()
{
	OS_CPU_SR cpu_sr;
	INT16U io;

	OS_ENTER_CRITICAL();
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if( io & FPGA_EXPAND_IOMASK_UV_READY)
		return True;
	else
		return False;
}
#endif
//#define PRINTSCAN_END_LOW_POWER
#ifdef COORD_NEW_UV
void Configure_Uv_Power(INT8U mask)
{
#if defined(MANUFACTURER_DYSS)||defined(ORIC_FUNCTION)
	if(mask&0x1)
		UV_CTR_SW1_OPEN()
	else
		UV_CTR_SW1_CLOSE()

			if(mask&0x2)
				UV_CTR_SW2_OPEN()
			else
				UV_CTR_SW2_CLOSE()

					if(mask&0x4)
						UV_CTR_SW3_OPEN()
					else
						UV_CTR_SW3_CLOSE()

							if(mask&0x8)
								UV_CTR_SW4_OPEN()
							else
								UV_CTR_SW4_CLOSE()
#elif (defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_SAIBO_ROLL_UV))&&defined(OLD_UV)
#if 0
									if((Ripstar_UV_Setting.UV_SETTING >> 24) == 100)
									{
										UV_CTR_SW1_CLOSE();
										UV_CTR_SW2_CLOSE();
										UV_CTR_SW3_OPEN();		
									}
									else if((Ripstar_UV_Setting.UV_SETTING >> 24) == 75)
									{
										UV_CTR_SW1_OPEN();
										UV_CTR_SW2_OPEN();
										UV_CTR_SW3_OPEN();
									}
									else
									{
										UV_CTR_SW1_CLOSE();
										UV_CTR_SW2_OPEN();
										UV_CTR_SW3_CLOSE();
									}
#endif
#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)								
	if((Ripstar_UV_Setting.UV_SETTING >> 24) == 100)
	{
		UV_CTR_SW1_OPEN();
		UV_CTR_SW2_OPEN();	
	}
	else if((Ripstar_UV_Setting.UV_SETTING >> 24) == 75)
	{
		UV_CTR_SW1_CLOSE();
		UV_CTR_SW2_OPEN();
	}
	else if((Ripstar_UV_Setting.UV_SETTING >> 24) == 50)
	{
		UV_CTR_SW1_OPEN();
		UV_CTR_SW2_CLOSE();
	}
	else//25
	{
		UV_CTR_SW1_CLOSE();
		UV_CTR_SW2_CLOSE();
	}
#endif
}
void Front_Light_on(void)
{
	UV_CTR_SHUTTER2_OPEN();
}
void Back_Light_on(void)
{
	UV_CTR_SHUTTER1_OPEN();
}
void Front_Light_off(void)
{
	UV_CTR_SHUTTER2_CLOSE();
}
void Back_Light_off(void)
{
	UV_CTR_SHUTTER1_CLOSE();
}
#else
//One byte to control: 
//bit0: 1/0 LAMP1 ON/OFF bit1: 1/0 LAMP1 HIGH/LOW bit2: 1/0 SHUTTER1 ON/OFF bit3 dummy
//bit4: 1/0 LAMP2 ON/OFF bit5: 1/0 LAMP2 HIGH/LOW bit6: 1/0 shutter2 ON/OFF bit7 dummy
void uv_CtrUsrAction(UBYTE Usr_Ctr)
{
	if ((Usr_Ctr & 0x01) && (Usr_Ctr & 0x02))
	{
		UV_CTR_LAMP1_ON();
		UV_CTR_LAMP1_HIGH();
	}
	else 
	{
		if (!(Usr_Ctr & 0x02))
			UV_CTR_LAMP1_LOW();
		else
		{}  //Wrong
		if (Usr_Ctr & 0x01)
			UV_CTR_LAMP1_ON();
		else
			UV_CTR_LAMP1_OFF();
	}

	if ((Usr_Ctr & 0x10) && (Usr_Ctr & 0x20))
	{
		UV_CTR_LAMP2_ON();
		UV_CTR_LAMP2_HIGH();
	}
	else 
	{
		if (!(Usr_Ctr & 0x20))
			UV_CTR_LAMP2_LOW();
		else
		{}  //Wrong
		if (Usr_Ctr & 0x10)
			UV_CTR_LAMP2_ON();
		else
			UV_CTR_LAMP2_OFF();
	}

	if (Usr_Ctr & 0x04)
	{
		UV_CTR_SHUTTER1_OPEN();
	}
	else
		UV_CTR_SHUTTER1_CLOSE();

	if (Usr_Ctr & 0x40)
	{
		UV_CTR_SHUTTER2_OPEN();
	}
	else
		UV_CTR_SHUTTER2_CLOSE();
}

//First byte is UV control byte, 2nd byte is UV status
//UV_STATUS_READY bit0 1/0

void uv_GetStatus(UBYTE *status)
{
	//UBYTE ctrByte = 0, stsByte = 0;
	status[0] = 0;
	status[1] = 0;
	if (UV_STATUS_LAMP1_HIGH && (!UV_STATUS_LAMP1_ON))
		status_ReportStatus(STATUS_ERR_UV_CTR_WRONGSTATUS, STATUS_SET);
	if (UV_STATUS_LAMP2_HIGH && (!UV_STATUS_LAMP2_ON))
		status_ReportStatus(STATUS_ERR_UV_CTR_WRONGSTATUS, STATUS_SET);

	if (UV_STATUS_LAMP1_ON)
		status[0] |= 0x1;

	if (UV_STATUS_LAMP1_HIGH)
		status[0] |= 0x2;

	if (UV_CTR_STATUS_SH1_OPEN)
		status[0] |= 0x4;

	if (UV_STATUS_LAMP2_ON)
		status[0] |= 0x10;

	if (UV_STATUS_LAMP2_HIGH)
		status[0] |= 0x20;

	if (UV_CTR_STATUS_SH2_OPEN)
		status[0] |= 0x40;

	if (UV_STATUS_READY)
		status[1] |= 0x2;
	if (UV_STATUS_LP1_FAULT)
		status[1] |= 0x1;
	if (UV_STATUS_LP2_FAULT)
		status[1] |= 0x10;	
	//	if (UV_STATUS_SHUTTER1_OPEN ^ UV_STATUS_SHUTTER1_CLOSE)
	//	{
	//		if (UV_STATUS_SHUTTER1_OPEN)
	//		{} //wrong
	//	}
#ifndef MANUFACTURER_DYSS    
	if (UV_STATUS_SHUTTER1_OPEN)
		status[1] |= 0x4;
	if (UV_STATUS_SHUTTER2_OPEN)
		status[1] |= 0x40;
#endif    
	status[2] = uv_UVMode;		
}

//	if (uv_isShutterOpen&& UV_REQUEST_PRINTSCAN_END)
//		uv_PrintScanEnd();
//	if ((!uv_isShutterOpen)&& UV_REQUEST_PRINTSCAN_START)
//		uv_PrintScanStart();
#endif
#if !defined(COORD_NEW_UV)||defined(MANUFACTURER_DOCAN_UV)
static void ISR_DSPUvControl1(const Pin *pPin)
{
	INT8U err;
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
#ifdef MANUFACTURER_DOCAN_UV
		if (PIO_Get(pPin)) 
			PIO_Clear(&DSPUvControl1Pin);
		else
			PIO_Set(&DSPUvControl1Pin);
#else	
		if (PIO_Get(pPin)) 
		{
			if(!uv_isShutter1Open)
			{
				uv_PrintScanStart(True);
			}
		}
		else
		{
			if(uv_isShutter1Open)
			{
				uv_PrintScanEnd(True);
			}
		}
#endif		
	}
}

static void ISR_DSPUvControl2(const Pin *pPin)
{
	INT8U err;
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{	
#ifdef MANUFACTURER_DOCAN_UV
		if (PIO_Get(pPin)) 
			PIO_Clear(&DSPUvControl2Pin);
		else
			PIO_Set(&DSPUvControl2Pin);
#else		
		if (PIO_Get(pPin)) 
		{
			if(!uv_isShutter2Open)
			{
				uv_PrintScanStart(False);
			}
		}
		else
		{
			if(uv_isShutter2Open)
			{
				uv_PrintScanEnd(False);
			}
		}
#endif		
	}
}
#endif
#if defined(FULGENCY_FUN)||defined(MANUFACTURER_DYSS)
void Waste_Ink_CHECK(void)
{
#ifdef MANUFACTURER_DYSS
	if (!PIO_Get(&UVStatPinUVRdy)) 
#elif defined(FULGENCY_FUN)
		if (PIO_Get(&FULGENCY_WASTE_INK)) 
#endif
			status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_SET);
		else
			status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_CLR);
}
#endif

#ifdef FULGENCY_FUN
INT8U FULGENCY_PLC_MASK = 0;
static void ISR_FULGENCY_START_PRT(const Pin *pPin)
{ 
	INT8U err;
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR) 
	{
		if (PIO_Get(pPin)) 
			FULGENCY_PLC_MASK |=START_PRT; 
	}
}

static void ISR_FULGENCY_Y_UNLOAD_FINISH(const Pin *pPin)
{ 
	if (PIO_Get(pPin)) 
		FULGENCY_PLC_MASK |=Y_UNLOAD_FINISH; 
}
#endif

INT8U uv_Init()
{
	OS_CPU_SR cpu_sr;

#ifdef EPSON_BOTTOM_BOARD_V3
#error "the following code of UV is un-completed"
	// need to config FPGA position irq 
#elif defined(COORD_NEW_UV)    
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
#ifdef MANUFACTURER_DOCAN_UV
	PIO_Set(&DSPUvControl1Pin);
	PIO_Set(&DSPUvControl2Pin);
	OS_ENTER_CRITICAL();	
	PIO_Configure(&DSPUvControl1Pin, 1);
	PIO_ConfigureIt(&DSPUvControl1Pin, ISR_DSPUvControl1);
	PIO_EnableIt(&DSPUvControl1Pin);
	PIO_Configure(&DSPUvControl2Pin, 1);
	PIO_ConfigureIt(&DSPUvControl2Pin, ISR_DSPUvControl2);
	PIO_EnableIt(&DSPUvControl2Pin);
	OS_EXIT_CRITICAL();
#endif
#else
	// Configure PIO
#ifndef COORD_NEW_UV	
	uv_UVMode = 3;
#endif	
	UV_CTR_SHUTTER1_CLOSE();
	uv_isShutter1Open = False;
	UV_CTR_SHUTTER2_CLOSE();
	uv_isShutter2Open = False;
	uv_isShutterOpen = False;
	//	UV_CTR_LAMP1_ON();
	//	UV_CTR_LAMP2_ON();

	OS_ENTER_CRITICAL();	
	PIO_Configure(&DSPUvControl1Pin, 1);
	PIO_ConfigureIt(&DSPUvControl1Pin, ISR_DSPUvControl1);
	PIO_EnableIt(&DSPUvControl1Pin);
	PIO_Configure(&DSPUvControl2Pin, 1);
	PIO_ConfigureIt(&DSPUvControl2Pin, ISR_DSPUvControl2);
	PIO_EnableIt(&DSPUvControl2Pin);
	OS_EXIT_CRITICAL();
#endif
#ifdef FULGENCY_FUN
	OS_ENTER_CRITICAL();	
	PIO_Configure(&UVStatPinLP1Fault, 1);
	PIO_ConfigureIt(&UVStatPinLP1Fault, ISR_FULGENCY_START_PRT);
	PIO_EnableIt(&UVStatPinLP1Fault);
	PIO_Configure(&UVStatPinLP2Fault, 1);
	PIO_ConfigureIt(&UVStatPinLP2Fault, ISR_FULGENCY_Y_UNLOAD_FINISH);
	PIO_EnableIt(&UVStatPinLP2Fault);
	OS_EXIT_CRITICAL();
#endif 
	return True;
}

INT8U uv_PrintScanStart(INT8U bLeft)
{
	switch(uv_UVMode & 0x3)
	{
	case 2: //双灯高功率
		UV_CTR_SHUTTER1_OPEN();
		uv_isShutter1Open = True;
		UV_CTR_SHUTTER2_OPEN();
		uv_isShutter2Open = True;
		break;
	case 3:	//单灯高功率
		if(bLeft)
			UV_CTR_LAMP1_HIGH();
		else
			UV_CTR_LAMP2_HIGH();
	case 0:	//双灯低功率
	case 1: //单灯低功率
		if(bLeft)
		{
			UV_CTR_SHUTTER1_OPEN();
			uv_isShutter1Open = True;
		}
		else
		{
			UV_CTR_SHUTTER2_OPEN();
			uv_isShutter2Open = True;
		}
		uv_isShutterOpen = True;
		break;
	default:
		return False;			
	}
	return True;
}

INT8U uv_PrintEndJob()
{
#ifdef COORD_NEW_UV
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
#else
	UV_CTR_LAMP1_LOW();
	UV_CTR_LAMP2_LOW();
#endif	
	return True;
}

INT8U uv_PrintScanEnd(INT8U bLeft)
{
#ifdef PRINTSCAN_END_LOW_POWER
	if(bLeft)
		UV_CTR_LAMP1_LOW();
	else
		UV_CTR_LAMP2_LOW();
#endif	
	if(bLeft)
	{
		UV_CTR_SHUTTER1_CLOSE();
		uv_isShutter1Open = False;
	}
	else
	{
		UV_CTR_SHUTTER2_CLOSE();
		uv_isShutter2Open = False;
	}
	uv_isShutterOpen = False;
	return True;
}

INT8U uv_PrintCancelJob()
{
#ifdef COORD_NEW_UV
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
#else
	UV_CTR_LAMP1_LOW();
	UV_CTR_LAMP2_LOW();
	UV_CTR_SHUTTER1_CLOSE();
	uv_isShutter1Open = False;
	UV_CTR_SHUTTER2_CLOSE();
	uv_isShutter2Open = False;
	uv_isShutterOpen = False;
	return True;
#endif	
}

INT8U uv_PrintPause()
{
#ifdef COORD_NEW_UV
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
#else
	UV_CTR_LAMP1_LOW();
	UV_CTR_LAMP2_LOW();
	UV_CTR_SHUTTER1_CLOSE();
	uv_isShutter1Open = False;
	UV_CTR_SHUTTER2_CLOSE();
	uv_isShutter2Open = False;
	uv_isShutterOpen = False;
	return True;	
#endif	
}

INT8U uv_PrintScanErr()
{
#ifdef COORD_NEW_UV
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
#else
	UV_CTR_LAMP1_LOW();
	UV_CTR_LAMP2_LOW();
	UV_CTR_SHUTTER1_CLOSE();
	uv_isShutter1Open = False;
	UV_CTR_SHUTTER2_CLOSE();
	uv_isShutter2Open = False;
	uv_isShutterOpen = False;
	return True;	
#endif	
}
#endif

#undef _UV_C_
#endif

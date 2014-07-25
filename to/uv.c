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

#if !(defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1))
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

void Configure_Uv_Power(INT8U mask)
{
	if(mask&0x1)
		UV_CTR_SW1_OPEN();
	else
		UV_CTR_SW1_CLOSE();

	if(mask&0x2)
		UV_CTR_SW2_OPEN();
	else
		UV_CTR_SW2_CLOSE();

	if(mask&0x4)
		UV_CTR_SW3_OPEN();
	else
		UV_CTR_SW3_CLOSE();

	if(mask&0x8)
		UV_CTR_SW4_OPEN();
	else
		UV_CTR_SW4_CLOSE();
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

#if defined(FULGENCY_FUN)||defined(MANUFACTURER_DYSS)
void Waste_Ink_CHECK(void)
{
	if (!PIO_Get(&UVStatPinUVRdy)) 
		status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_CLR);
}
#endif

INT8U uv_Init()
{
	OS_CPU_SR cpu_sr;

	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
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
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
	return True;
}

INT8U uv_PrintScanEnd(INT8U bLeft)
{
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
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
	return True;
}

INT8U uv_PrintPause()
{
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
	return True;	
}

INT8U uv_PrintScanErr()
{
	UV_POSI.STOP_UV = True;
	Front_Light_off();
	Back_Light_off();
	return True;	
}
#endif

#undef _UV_C_
#endif

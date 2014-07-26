#ifndef _UV_C_
#define _UV_C_

#include "global.h"
#include "uv.h"
#include "status.h"
#include "FPGA.h"

UV_POSI_TYPE UV_POSI = {0};

const Pin UVStatPinLP1ShCl = PIN_UV_STAT_LP1_SH_CL_I; 
const Pin UVStatPinLP1ShOp = PIN_UV_STAT_LP1_SH_OP_I; 
const Pin UVStatPinLP2ShCl = PIN_UV_STAT_LP2_SH_CL_I;
const Pin UVStatPinLP2ShOp = PIN_UV_STAT_LP2_SH_OP_I; 
const Pin UVStatPinUVRdy = PIN_UV_STAT_UV_RDY;
const Pin UVStatPinLP1Fault = PIN_UV_STAT_LP1_FAULT;
const Pin UVStatPinLP2Fault = PIN_UV_STAT_LP2_FAULT;

Pin UVCtrlPinSH1Open = PIN_UV_CTRL_SHUTTER_1_OPEN;
Pin UVCtrlPinSH2Open = PIN_UV_CTRL_SHUTTER_2_OPEN;
Pin UVCtrlPinLP2On = PIN_UV_CTRL_LP2_ON;
Pin UVCtrlPinLP2HighPower = PIN_UV_CTRL_LP2_HIGHPOWER;
Pin UVCtrlPinLP1On = PIN_UV_CTRL_LP1_ON;
Pin UVCtrlPinLP1HighPower = PIN_UV_CTRL_LP1_HIGHPOWER;

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

void Waste_Ink_CHECK(void)
{
	if (!PIO_Get(&UVStatPinUVRdy)) 
		status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_Waste_Ink_FULL, STATUS_CLR);
}

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

#undef _UV_C_
#endif

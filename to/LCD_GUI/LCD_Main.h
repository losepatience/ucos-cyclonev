#ifndef __LCD_MAIN_H__
#define __LCD_MAIN_H__

#include "os_cpu.h"
#include "lcd.h"
//#include "Menu_GUI.h"
#include "display.h"
#ifdef FUNC_MAINTAIN
enum KeyIDEnum
{
	Btn_Move_left =1,
	Btn_Move_right,
	Btn_Move_backward,
	Btn_Move_forward,
	Btn_BASEPOINT,	//5
	Btn_Enter,
	Btn_Abort,
	Btn_Pause_resume,
	Btn_PRINTQUALITY,
	Btn_Menu = 0x101,
	Btn_Width = 0x102,
	Btn_Maintain = 0x103,
	Btn_Checknozzle,
	Btn_SHEETCUT,
	Btn_Cleaning,
};
#else
enum KeyIDEnum
{
	Btn_Move_left =1,
	Btn_Move_right,
	Btn_Move_backward,
	Btn_Move_forward,
	Btn_Menu,
	Btn_Enter,
	Btn_Abort,
	Btn_Pause_resume,
	Btn_Cleaning,
	Btn_Width,
	Btn_Checknozzle = 0x201,	//Allwin Special:
	Btn_BASEPOINT = 0x101,
	Btn_SHEETCUT = 0x102,
	Btn_PRINTQUALITY = 0x103,
	Btn_HEATER = 0x203
};
#endif
double xPow( double d , int n );
void SetUILanguage(void * pData, int Index, int Data);
INT8U GetUILanguage(void* Data,int index);


#if defined(LCD_MODULE_128_64_ST5668A)
extern INT8U radiocheckbox_offset;
#endif

#ifdef FUNC_WAVE_MAP
INT8U SetUIWaveMap(void* pData,int index, int Data);
INT8U GetUIWaveMap(void* Data,int index);
#endif
void ChangMenuDisMode(INT8U MenuMode);
#endif

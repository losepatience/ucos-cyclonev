
#include "os_includes.h"
#include "board.h"
#include "global.h"
#include "keylcd.h"
#include "display.h"
#include "miscellance.h"

/*******************************************************************************************************
*      							LCD Diaplay														   *	
*	Pin No.	Symbol	Level		Description											GPIO			   *	
* 	1		CS1		L			KS0108B(1)选通信号									GPD8/VD16		   *	
* ----------------------------------------------------------------------------------------------------*
*	2		CS2		L			KS0108B(2) 选通信号									GPD9/VD17		   *	
* ----------------------------------------------------------------------------------------------------*
*	6		RS		H/L			指令和数据选择信号	H : 数据信号, L : 指令信号		GPD10/VD18		   *	
* ----------------------------------------------------------------------------------------------------*
*	7		R/W		H/L			指令和数据读写信号	H : 读信号, L : 写信号			GPD11/VD19		   *	
* ----------------------------------------------------------------------------------------------------*
*	8		E		H, H → L	控制器使能信号									    GPD12/VD20		   *
* ----------------------------------------------------------------------------------------------------*
*	9~16	D0~D7	H/L			8位数据线											GPD0~GPD7/VD8~VD15 *
* ---------4-------------------------------------------------------------------------------------------*
*  		nLED	H/L			背光开关											GPD13/VD21		   *			
*******************************************************************************************************/

//20101020:cheney:
/*******************************************************************************************************
*      							LCD Diaplay														   *	
*	Pin No.	Symbol	Level		Description											GPIO			   *	
* 	1		CS1		L			KS0108B(1)选通信号									GPD0/VD8		   *	
* ----------------------------------------------------------------------------------------------------*
*	2		CS2		L			KS0108B(2) 选通信号									GPD1/VD9		   *	
* ----------------------------------------------------------------------------------------------------*
*	6		RS		H/L			指令和数据选择信号	H : 数据信号, L : 指令信号		GPD2/VD10		   *	
* ----------------------------------------------------------------------------------------------------*
*	7		R/W		H/L			指令和数据读写信号	H : 读信号, L : 写信号			GPD3/VD11		   *	
* ----------------------------------------------------------------------------------------------------*
*	8		E		H, H → L	控制器使能信号									    GPD4/VD12		   *
* ----------------------------------------------------------------------------------------------------*
*	9~16	D0~D7	H/L			8位数据线											GPC8~GPC15/VD0~VD7 *
* ----------------------------------------------------------------------------------------------------*
*  		nLED	H/L			背光开关											GPD5/VD13		   *			
*******************************************************************************************************/

#ifdef IIC_Key_Board_LCD

#ifdef LCD_MODULE_ZX19264M1A
static const Pin LcdBusyPin = PIN_KB_LCD_BUSY;
static const Pin ReSetPin = PIN_KB_LCD_nLED;
#else
static const Pin LcdLEDPin = PIN_KB_LCD_nLED;
static const Pin LcdEnPin = PIN_KB_LCD_EN;
static Pin LcdDataPins = PIN_KB_LCD_DATA_BUS;

static const Pin LcdRWPin = PIN_KB_LCD_RW;
static const Pin LcdRSPin = PIN_KB_LCD_RS;
static const Pin LcdCS2Pin = PIN_KB_LCD_CS2;
static const Pin LcdCS1Pin = PIN_KB_LCD_CS1;
#endif
#if (defined (EPSON_LCD))

extern const INT8U hztab[][32];
extern INT8U ASC_Bmp_Tab[];

OS_EVENT *LCDSem; //Sem to lock LCD display

static INT8U DisplayBuff[LCD_SUBMODULE_COUNT*LCD_PAGENUMBER*LCD_MODULE_WIDTH];
static INT8U pageNo[LCD_SUBMODULE_COUNT];
static INT8U yAddr[LCD_SUBMODULE_COUNT];

static INT8U LCD_ReadStatusA(); /* 读状态子程序（左）*/
static INT8U LCD_ReadStatusB(); /* 读状态子程序（右）*/
#ifdef LCD_MODULE_ZX19264M1A
void LCD_Rectangle_Handle(unsigned short X, unsigned short Y, unsigned short Width, unsigned short High, unsigned char handle);
void LCD_DrawPicture_New(unsigned short X, unsigned short Y, unsigned short pictureWidth, unsigned short pictureHigh, unsigned char * pictureBuffer);
void  LCD_New_Light_On(void);
extern INT8U imgTop[8];
extern INT8U imgBot[8];
extern INT8U imgLeft[8];
extern INT8U imgRight[8];
extern INT8U imgZero[16];
INT8U Lcd_Cmd_Q[LCD_MAX_CMD_SIZE] = {0};
INT16U Lcd_Cmd_Num = 0;
#else
static void LCD_WriteCMD(INT8U data); /* 写指令代码子程序（左右同时）*/
#endif
#ifdef EPSON_BOTTOM_BOARD_V2
#define SHIFT_BUS_DATA(data)    (((INT32U)data) << 7)
#define GET_BUS_DATA(data)    (((INT32U)data) >> 7)
#else
#define SHIFT_BUS_DATA(data)    (((((INT32U)data) & 0x1F) << 10) | ((((INT32U)data) & 0xE0) << 12))
#define GET_BUS_DATA(data)    (INT8U)(((((INT32U)data) & 0x7C00) >> 10) | ((((INT32U)data) & 0xE0000) >> 12))
#endif
//#define LCD_TEST
void LCD_Irq_Interrupt(const Pin * pin)
{
	INT8U err;
	OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_SET, &err);
	
}
void Reset_key_Irq_Interrupt(const Pin * pin)
{
	INT8U err;
	if( PIO_Get(&ReSetPin)==0)
		OSFlagPost(mix_FLAG_GRP, Reset_Key_STATE, OS_FLAG_SET, &err);
}
INT8U read_reset()
{
	return ( PIO_Get(&ReSetPin)==0);
}
void LCD_Init()
{
	//  INT8U status[2];
#ifdef LCD_TEST
	INT8U i, buf[64*32/8];
	//unsigned short strText[10] = {1,2,3,4,5,6,7,8,0,0};
	//unsigned short strMixedText[10] = {0x103,0x104,'A','b','C','*',0,0,0,0};
#endif
	INT8U err;
	PIO_ConfigureIt(&ReSetPin, Reset_key_Irq_Interrupt);
	PIO_EnableIt(&ReSetPin);
	PIO_ConfigureIt(&LcdBusyPin, LCD_Irq_Interrupt);
	PIO_EnableIt(&LcdBusyPin);
	OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_CLR, &err);
	
	LCDSem = OSSemCreate(1);             /* Random number semaphore */
}

void LCD_ShortDly(INT16U time)
{
	INT16U i;
	for (i=0;i<time;i++);
}

void LCD_WriteCMDA(INT8U data)/* 写指令代码子程序（左）*/
{ 
#ifndef LCD_MODULE_ZX19264M1A
	INT32U mask;
	
	//csa=0; csb=1; rs=0; rw=0; data
	//	rGPDDAT = (rGPDDAT & 0xF000)|(0x0200|data);
	PIO_Set(&LcdCS1Pin);
	PIO_Clear(&LcdCS2Pin);
	PIO_Clear(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = SHIFT_BUS_DATA(data);;
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);	
#else
	return;
#endif
}

void LCD_WriteCMDB(INT8U data)/* 写指令代码子程序（右）*/
{
#ifndef LCD_MODULE_ZX19264M1A
	INT32U mask = data;
	
	//csa=1; csb=0; rs=0; rw=0; data
	//rGPDDAT = (rGPDDAT & 0xF000)|(0x0100|data);
	PIO_Clear(&LcdCS1Pin);
	PIO_Set(&LcdCS2Pin);
	PIO_Clear(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);
#else
	return;
#endif
}
#ifndef LCD_MODULE_ZX19264M1A
INT8U LCD_ReadStatusA()/* 读状态子程序（左）*/
{
	INT32U io;
	INT8U data;
	
	LcdDataPins.type = PIO_INPUT;
	LcdDataPins.pio->PIO_ODR = LcdDataPins.mask;
	
	PIO_Set(&LcdCS1Pin);
	PIO_Clear(&LcdCS2Pin);
	
	PIO_Clear(&LcdRSPin);
	PIO_Set(&LcdRWPin);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	io = LcdDataPins.pio->PIO_PDSR;
	data = GET_BUS_DATA(io);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_DLY);	
	
	LcdDataPins.type = PIO_OUTPUT_1;
	LcdDataPins.pio->PIO_OER = LcdDataPins.mask;
	
	return data;
}

INT8U LCD_ReadStatusB()/* 读状态子程序（右）*/
{
	INT32U io;
	INT8U data;
	
	LcdDataPins.type = PIO_INPUT;
	LcdDataPins.pio->PIO_ODR = LcdDataPins.mask;
	
	PIO_Clear(&LcdCS1Pin);
	PIO_Set(&LcdCS2Pin);
	
	PIO_Clear(&LcdRSPin);
	PIO_Set(&LcdRWPin);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	io = LcdDataPins.pio->PIO_PDSR;
	data = GET_BUS_DATA(io);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_DLY);	
	
	LcdDataPins.type = PIO_OUTPUT_1;
	LcdDataPins.pio->PIO_OER = LcdDataPins.mask;
	
	return data;
}
#endif
void LCD_WriteCMD(INT8U data)/* 写指令代码子程序（左右同时）*/
{ 
#ifndef LCD_MODULE_ZX19264M1A
	LCD_WriteCMDA(data);
	LCD_WriteCMDB(data);
#else
	return;
#endif
	/*
	INT32U mask = data;
	
	//csa=0; csb=1; rs=0; rw=0; data
	//	rGPDDAT = (rGPDDAT & 0xF000)|(0x0200|data);
	PIO_Clear(&LcdCS1Pin);
	PIO_Clear(&LcdCS2Pin);
	PIO_Clear(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	mask <<=  7;
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = data;
	mask <<= 7;
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);	
	*/
}


void LCD_WriteDataA(INT8U data)/* 写显示数据子程序（左）*/
{
#ifndef LCD_MODULE_ZX19264M1A
	INT32U mask;
	
	//csa=0; csb=1; rs=1; rw=0; data
	//rGPDDAT = (rGPDDAT & 0xF000)|(0x0600|data);
	PIO_Set(&LcdCS1Pin);
	PIO_Clear(&LcdCS2Pin);
	PIO_Set(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);
#else
	return;
#endif
}

void LCD_WriteDataB(INT8U data)/* 写显示数据子程序（右）*/
{
#ifndef LCD_MODULE_ZX19264M1A
	INT32U mask;
	
	//csa=1; csb=0; rs=1; rw=0; data
	//rGPDDAT = (rGPDDAT & 0xF000)|(0x0500|data);
	PIO_Clear(&LcdCS1Pin);
	PIO_Set(&LcdCS2Pin);
	PIO_Set(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = SHIFT_BUS_DATA(data);
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);
#else
	return;
#endif
}

void LCD_WriteData(INT8U data)/* 写显示数据子程序（左右同时）*/
{
#ifndef LCD_MODULE_ZX19264M1A
	LCD_WriteDataA(data);
	LCD_WriteDataB(data);
#else
	return;
#endif 
	/*
	INT32U mask = data;
	
	//csa=1; csb=0; rs=1; rw=0; data
	//rGPDDAT = (rGPDDAT & 0xF000)|(0x0500|data);
	PIO_Clear(&LcdCS1Pin);
	PIO_Clear(&LcdCS2Pin);
	PIO_Set(&LcdRSPin);
	PIO_Clear(&LcdRWPin);
	mask <<=  7;
	PIO_SET_HIGH(&LcdDataPins, mask);
	data = (INT8U)~data;
	mask = data;
	mask <<= 7;
	PIO_SET_LOW(&LcdDataPins, mask);
	
	LCD_ShortDly(LCD_EL_DLY);	
	LCD_E_HIGH();
	LCD_ShortDly(LCD_EH_DLY);
	LCD_E_LOW();
	LCD_ShortDly(LCD_EL_AND_BUSY_DLY);
	*/
}
#ifndef LCD_MODULE_ZX19264M1A
void LCD_Clr(void)
{
	INT8U i,j;
	for(i=0; i<LCD_PAGENUMBER; i++)
	{
		LCD_WriteCMD(LCD_CMD_SetPageNo | i);
		LCD_WriteCMD(LCD_CMD_SetYAddr | 0);
		for(j=0; j<LCD_MODULE_WIDTH; j++)
		{
#ifdef LCD_TEST
			LCD_WriteData(0x55);
			LCD_WriteData(0x55);
			LCD_WriteData(0x55);
			LCD_WriteData(0xAA);
			j+=3;
#else
			LCD_WriteData(0);
#endif            
		}
	}
} 

void LCD_DisplayON()
{
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
}

void LCD_DisplayOFF()
{
	LCD_WriteCMD(LCD_CMD_DISPLAY_OFF);
}
#endif
void LCD_SendCmdOrData(INT8U ch,INT8U index);
void LCD_ClearScreen() //清屏
{
#ifdef LCD_MODULE_ZX19264M1A
	//LCD_Rectangle_Handle(0,0,192,64,0);
	INT8U cmd_id = 0xF4;
	INT8U err;
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	OSSemPost(LCDSem);
#else
	INT8U err;
	
	OSSemPend(LCDSem, 0, &err);
	
	LCD_Clr();
	
	LCD_WriteCMD(LCD_CMD_SetYAddr | 0);
	LCD_WriteCMD(LCD_CMD_SetPageNo | 0);
	LCD_WriteCMD(LCD_CMD_SetStartLine | 0);
	
	pageNo[0] = pageNo[1] = 0;
	yAddr[0] = yAddr[1] = 0;
	memset(DisplayBuff, 0, sizeof(DisplayBuff));
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
#endif
}
#ifndef LCD_MODULE_ZX19264M1A
INT8U LCD_MapCoorToLCD(INT16U x, INT16U y, INT8U * moduleNo, INT8U * pageno, INT8U * yAddres, INT8U * xBit)
{
	*moduleNo = x / LCD_MODULE_WIDTH;
	*yAddres = x % LCD_MODULE_WIDTH;
	* pageno = y / 8;
	* xBit = y % 8;
	
	if(*moduleNo >= LCD_SUBMODULE_COUNT || * pageno >= LCD_PAGENUMBER)
	{
		if(*moduleNo >= LCD_SUBMODULE_COUNT)
		{
			*moduleNo = LCD_SUBMODULE_COUNT -1;
			*yAddres = LCD_MODULE_WIDTH -1;
		}
		if(* pageno >= LCD_PAGENUMBER)
		{
			* pageno = LCD_PAGENUMBER -1;
			* xBit = 7;
		}
		return 0;
	}
	else
		return 1;
}

INT8U LCD_DrawPixel(unsigned short X, unsigned short Y, unsigned char isBlack)
{
	INT8U moduleNo, pageno, yAddres, xBit, bitmask;
	INT8U err;
	
	if(!LCD_MapCoorToLCD(X, Y, &moduleNo, &pageno, &yAddres, &xBit))
		return 0;
	
	OSSemPend(LCDSem, 0, &err);
	
	bitmask = DisplayBuff[(moduleNo * LCD_PAGENUMBER  + pageno) *LCD_MODULE_WIDTH + yAddres];
	
	if(isBlack)
		bitmask |= 1<<xBit;
	else 
		bitmask &= ~(1<<xBit);
	
	if(pageNo[moduleNo] != pageno)
	{
		if(moduleNo == 0)
			LCD_WriteCMDA(LCD_CMD_SetPageNo | pageno);
		else
			LCD_WriteCMDB(LCD_CMD_SetPageNo | pageno);
		pageNo[moduleNo] = pageno;
	}
	if(yAddr[moduleNo] != yAddres)
	{
		if(moduleNo == 0)
			LCD_WriteCMDA(LCD_CMD_SetYAddr | yAddres);
		else
			LCD_WriteCMDB(LCD_CMD_SetYAddr | yAddres);
		yAddr[moduleNo] = yAddres;
	}
	
	if(moduleNo == 0)
		LCD_WriteDataA(bitmask);
	else
		LCD_WriteDataB(bitmask);
	yAddr[moduleNo] ++;
	
	DisplayBuff[(moduleNo * LCD_PAGENUMBER  + pageno) *LCD_MODULE_WIDTH + yAddres] = bitmask;
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	return 1;
}
#endif
INT8U LCD_FillRectangle(unsigned short X, unsigned short Y, unsigned short Width, unsigned short High, unsigned char isBlack)
{
#ifdef LCD_MODULE_ZX19264M1A
	if(isBlack==0&&(Width == LCD_PIXEL_WIDTH&& High ==LCD_PIXEL_HIGH && Y ==0 && Y==0))
	{
		INT8U cmd_id = 0xF4;
		INT8U err;
		OSSemPend(LCDSem, 0, &err);
		LCD_SendCmdOrData(cmd_id,1);
		OSSemPost(LCDSem);
	}
	else
		LCD_Rectangle_Handle(X,Y,Width,High,isBlack);
	return 1;
#else
	INT8U moduleNo[2], pageno[2], yAddres[2], xBit[2], ret;
	INT8U cur_moduleNo, cur_pageno, cur_yAddres, yAddres_start, yAddres_end, xBitMask_start,xBitMask_end, xBitMask, dst;
	INT8U err;
	
	if(!LCD_MapCoorToLCD(X, Y, &moduleNo[0], &pageno[0], &yAddres[0], &xBit[0]))
		return 0;
	ret = LCD_MapCoorToLCD(X+Width-1, Y+High-1, &moduleNo[1], &pageno[1], &yAddres[1], &xBit[1]);
	
	cur_moduleNo = moduleNo[0];
	
	OSSemPend(LCDSem, 0, &err);
	
	while(cur_moduleNo <= moduleNo[1])
	{
		if(cur_moduleNo == moduleNo[0])
			yAddres_start = yAddres[0];
		else
			yAddres_start = 0;
		if(cur_moduleNo == moduleNo[1])
			yAddres_end = yAddres[1];
		else
			yAddres_end = LCD_MODULE_WIDTH -1;
		
		cur_pageno =  pageno[0];
		while(cur_pageno <= pageno[1])
		{
			if(cur_pageno == pageno[0])
				xBitMask_start = (0xFF << xBit[0]);
			else
				xBitMask_start = 0xFF;
			if(cur_pageno == pageno[1])
				xBitMask_end = (0xFF >>(7- xBit[1]));
			else
				xBitMask_end = 0xFF;
			xBitMask = xBitMask_start & xBitMask_end;
			
			if(pageNo[cur_moduleNo] != cur_pageno)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetPageNo | cur_pageno);
				else
					LCD_WriteCMDB(LCD_CMD_SetPageNo | cur_pageno);
				pageNo[cur_moduleNo] = cur_pageno;
			}
			if(yAddr[cur_moduleNo] != yAddres_start)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetYAddr | yAddres_start);
				else        
					LCD_WriteCMDB(LCD_CMD_SetYAddr | yAddres_start);
				yAddr[cur_moduleNo] = yAddres_start;
			}
			
			cur_yAddres = yAddres_start;
			while(cur_yAddres <= yAddres_end)
			{
				dst = DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres];
				if(isBlack)
					dst |= xBitMask;
				else
					dst &= ~xBitMask;
				
				if(cur_moduleNo == 0)
					LCD_WriteDataA(dst);
				else
					LCD_WriteDataB(dst);
				
				DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres] = dst;
				yAddr[cur_moduleNo]++;
				cur_yAddres++;
				
			}
			cur_pageno ++;
		}
		cur_moduleNo ++;
	}
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	return ret + 1; //1 is clipped,  2 is ok.
#endif
}

INT8U LCD_GetSrcByte(INT8U cur_moduleNo, INT8U cur_pageno, INT8U cur_yAddres, 
					 unsigned short X, unsigned short Y, unsigned short pictureWidth, 
					 unsigned short pictureHigh, const unsigned char * pictureBuffer)
{
	INT8U x = cur_moduleNo*LCD_MODULE_WIDTH + cur_yAddres - X;
	INT8S y = cur_pageno *8 - Y;
	INT8U src, dst, i, bitmask, mask;
	
	bitmask = 1<<(7-x%8);
	dst = 0;
	mask= 1;
	
	for(i =0; i< 8; i++)
	{
		if(y+i >= 0 && y+i < pictureHigh)
		{
			src = pictureBuffer[(y+i)*((pictureWidth+8-1)/8)+x/8];
			if(src & bitmask)
				dst |= mask;
		}
		mask = mask<<1;
	}
	
	return dst;
}

INT8U LCD_GetSrcByte_internal(INT8U cur_moduleNo, INT8U cur_pageno, INT8U cur_yAddres, 
							  INT16U X, INT16U Y, INT16U pictureWidth, INT16U pictureHigh, const INT8U * pictureBuffer)
{ //assert pictureWidth == 8, pictureHigh == 16.
	INT8U x = cur_moduleNo*LCD_MODULE_WIDTH + cur_yAddres - X;
	INT8S y = cur_pageno *8 - Y;
	INT8U src, dst, i, mask;
	
	dst = 0;
	mask= 1;
	
	for(i =0; i< 8; i++)
	{
		if(y+i >= 0 && y+i < pictureHigh)
		{
			src = pictureBuffer[((y+i)/8)*pictureWidth+x];
			if(src & ( 1<<((y+i)%8) ))
				dst |= mask;
		}
		mask = mask<<1;
	}
	
	return dst;
}
#ifndef LCD_MODULE_ZX19264M1A
INT8U LCD_DrawPicture_Internal(unsigned short X, unsigned short Y, unsigned short pictureWidth, 
							   unsigned short pictureHigh, const unsigned char * pictureBuffer, INT8U picType)
{
	INT8U moduleNo[2], pageno[2], yAddres[2], xBit[2], ret;
	INT8U cur_moduleNo, cur_pageno, cur_yAddres, yAddres_start, yAddres_end, xBitMask_start,xBitMask_end, xBitMask, src,dst;
	
	if(!LCD_MapCoorToLCD(X, Y, &moduleNo[0], &pageno[0], &yAddres[0], &xBit[0]))
		return 0;
	ret = LCD_MapCoorToLCD(X+pictureWidth-1, Y+pictureHigh-1, &moduleNo[1], &pageno[1], &yAddres[1], &xBit[1]);
	
	cur_moduleNo = moduleNo[0];
	
	while(cur_moduleNo <= moduleNo[1])
	{
		if(cur_moduleNo == moduleNo[0])
			yAddres_start = yAddres[0];
		else
			yAddres_start = 0;
		if(cur_moduleNo == moduleNo[1])
			yAddres_end = yAddres[1];
		else
			yAddres_end = LCD_MODULE_WIDTH -1;
		
		cur_pageno =  pageno[0];
		while(cur_pageno <= pageno[1])
		{
			if(cur_pageno == pageno[0])
				xBitMask_start = (0xFF << xBit[0]);
			else
				xBitMask_start = 0xFF;
			if(cur_pageno == pageno[1])
				xBitMask_end = (0xFF >>(7- xBit[1]));
			else
				xBitMask_end = 0xFF;
			xBitMask = xBitMask_start & xBitMask_end;
			
			if(pageNo[cur_moduleNo] != cur_pageno)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetPageNo | cur_pageno);
				else
					LCD_WriteCMDB(LCD_CMD_SetPageNo | cur_pageno);
				pageNo[cur_moduleNo] = cur_pageno;
			}
			if(yAddr[cur_moduleNo] != yAddres_start)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetYAddr | yAddres_start);
				else        
					LCD_WriteCMDB(LCD_CMD_SetYAddr | yAddres_start);
				yAddr[cur_moduleNo] = yAddres_start;
			}
			
			cur_yAddres = yAddres_start;
			while(cur_yAddres <= yAddres_end)
			{
				if(picType == 0)
					src = LCD_GetSrcByte(cur_moduleNo, cur_pageno, cur_yAddres, X, Y, pictureWidth, pictureHigh, pictureBuffer);
				else
					src = LCD_GetSrcByte_internal(cur_moduleNo, cur_pageno, cur_yAddres, X, Y, pictureWidth, pictureHigh, pictureBuffer);
				
				dst = DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres];
				dst &= ~xBitMask;
				dst |= xBitMask & src;
				
				if(cur_moduleNo == 0)
					LCD_WriteDataA(dst);
				else
					LCD_WriteDataB(dst);
				
				DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres] = dst;
				
				yAddr[cur_moduleNo]++;
				cur_yAddres++;
				
			}
			cur_pageno++;
		}
		cur_moduleNo++;
	}
	
	return ret + 1; //1 is clipped,  2 is ok.
}
#endif
INT8U LCD_DrawPicture(unsigned short X, unsigned short Y, unsigned short pictureWidth, 
					  unsigned short pictureHigh, unsigned char * pictureBuffer)
{
#ifdef LCD_MODULE_ZX19264M1A
	LCD_DrawPicture_New(X,Y,pictureWidth,pictureHigh,pictureBuffer);
	return 1;
#else
	INT8U ret;
	INT8U err;
	
	OSSemPend(LCDSem, 0, &err);
	
	ret = LCD_DrawPicture_Internal(X, Y, pictureWidth, pictureHigh, pictureBuffer, 0);
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	return ret;
#endif
}

INT8U LCD_InverseRegion(unsigned short X, unsigned short Y, unsigned short regionWidth, unsigned short regionHigh)
{
#ifdef LCD_MODULE_ZX19264M1A
	LCD_Rectangle_Handle(X,Y,regionWidth,regionHigh,2);
	return 1;
#else
	INT8U moduleNo[2], pageno[2], yAddres[2], xBit[2], ret;
	INT8U cur_moduleNo, cur_pageno, cur_yAddres, yAddres_start, yAddres_end, xBitMask_start,xBitMask_end, xBitMask, src,dst;
	INT8U err;
	
	if(!LCD_MapCoorToLCD(X, Y, &moduleNo[0], &pageno[0], &yAddres[0], &xBit[0]))
		return 0;
	ret = LCD_MapCoorToLCD(X+regionWidth-1, Y+regionHigh-1, &moduleNo[1], &pageno[1], &yAddres[1], &xBit[1]);
	
	cur_moduleNo = moduleNo[0];
	
	OSSemPend(LCDSem, 0, &err);
	
	while(cur_moduleNo <= moduleNo[1])
	{
		if(cur_moduleNo == moduleNo[0])
			yAddres_start = yAddres[0];
		else
			yAddres_start = 0;
		if(cur_moduleNo == moduleNo[1])
			yAddres_end = yAddres[1];
		else
			yAddres_end = LCD_MODULE_WIDTH -1;
		
		cur_pageno =  pageno[0];
		while(cur_pageno <= pageno[1])
		{
			if(cur_pageno == pageno[0])
				xBitMask_start = (0xFF << xBit[0]);
			else
				xBitMask_start = 0xFF;
			if(cur_pageno == pageno[1])
				xBitMask_end = (0xFF >>(7- xBit[1]));
			else
				xBitMask_end = 0xFF;
			xBitMask = xBitMask_start & xBitMask_end;
			
			if(pageNo[cur_moduleNo] != cur_pageno)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetPageNo | cur_pageno);
				else
					LCD_WriteCMDB(LCD_CMD_SetPageNo | cur_pageno);
				pageNo[cur_moduleNo] = cur_pageno;
			}
			if(yAddr[cur_moduleNo] != yAddres_start)
			{
				if(cur_moduleNo == 0)
					LCD_WriteCMDA(LCD_CMD_SetYAddr | yAddres_start);
				else        
					LCD_WriteCMDB(LCD_CMD_SetYAddr | yAddres_start);
				yAddr[cur_moduleNo] = yAddres_start;
			}
			
			cur_yAddres = yAddres_start;
			while(cur_yAddres <= yAddres_end)
			{
				src = DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres];
				dst = src ^ xBitMask;
				
				if(cur_moduleNo == 0)
					LCD_WriteDataA(dst);
				else
					LCD_WriteDataB(dst);
				
				DisplayBuff[(cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres] = dst;
				
				yAddr[cur_moduleNo]++;
				cur_yAddres++;
			}
			cur_pageno ++;
		}
		cur_moduleNo ++;
	}
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	return ret + 1; //1 is clipped,  2 is ok.
#endif
}

INT8U LCD_GetRegion(unsigned short X, unsigned short Y, unsigned short regionWidth, unsigned short regionHigh, unsigned char * regionBuffer)
{
	INT16U curX, curY;
	INT16U index;
	INT8U mask, clipped = 0, filled = 0;
	INT8U cur_moduleNo, cur_pageno, cur_yAddres;
	INT8U err;
	
	memset(regionBuffer, 0, (regionWidth+8-1)/8 * regionHigh);
	
	OSSemPend(LCDSem, 0, &err);
	
	for(curY = Y; curY < Y + regionHigh; curY ++)
	{
		for(curX = X; curX < X + regionWidth; curX ++)
		{
			cur_moduleNo = curX / LCD_MODULE_WIDTH;
			cur_yAddres = curX % LCD_MODULE_WIDTH;
			cur_pageno = curY / 8;
			mask = 1<<(curY % 8);
			if(cur_moduleNo >= LCD_SUBMODULE_COUNT || cur_pageno >= LCD_PAGENUMBER)
			{
				clipped = 1;
				continue;
			}
			
			filled = 1;
			index = (cur_moduleNo * LCD_PAGENUMBER  + cur_pageno) *LCD_MODULE_WIDTH + cur_yAddres;
			if(DisplayBuff[index] & mask)
			{
				regionBuffer[(curY-Y)*((regionWidth+8-1)/8) + (curX-X)/8] |= 0x80>>((curX-X)%8);
			}
		}
	}
	
	OSSemPost(LCDSem);
	
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
}
#ifdef LCD_MODULE_ZX19264M1A
void LCD_SendCmdOrData(INT8U ch,INT8U index)
{
	INT32U mask = 0,i = 0,tmp;
	INT8U data = ch;
	INT8U err;
#if 1
	Lcd_Cmd_Q[Lcd_Cmd_Num++] = ch;
#else
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();
	
	if( PIO_Get(&LcdBusyPin)==1)
	{
		OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_CLR, &err);
		OS_EXIT_CRITICAL();
		if(index == 1)
			OSFlagPend(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 400, &err);
		else
			OSFlagPend(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 10, &err);
	}
	else
	{
		OS_EXIT_CRITICAL();
	}
	
	PIO_Set(&LcdLEPin);
	LCD_ShortDly(LCD_EH_DLY);
	PIO_Clear(&LcdRWPin);
	
	LCD_ShortDly(LCD_EH_DLY);
	
	if(ch & 0x01)
		PIO_Set(&LcdDataD0);
	else
		PIO_Clear(&LcdDataD0);
	
	mask = SHIFT_BUS_DATA(data);
	
	PIO_SET_HIGH(&LcdDataPins, mask);
	
	data = (INT8U)~data;
	
	mask = SHIFT_BUS_DATA(data);
	
	
	PIO_SET_LOW(&LcdDataPins, mask);
	
	
	LCD_ShortDly(LCD_EH_DLY);
	
	PIO_Clear(&LcdLEPin);
	LCD_ShortDly(LCD_EH_DLY);
	PIO_Set(&LcdRWPin);       	
	LCD_ShortDly(LCD_EH_DLY);
	PIO_Clear(&LcdReqPin);
	
	
	OS_ENTER_CRITICAL();
	
	if( PIO_Get(&LcdBusyPin)==0)
	{
		OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_CLR, &err);
		OS_EXIT_CRITICAL();
		if(index == 2)
			OSFlagPend(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 400, &err);
		else
			OSFlagPend(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 10, &err);
	}
	else
	{
		OS_EXIT_CRITICAL();
	}
	
	//PIO_Set(&LcdLEPin);
	PIO_Set(&LcdReqPin);
	//PIO_Clear(&LcdDataPins);
#endif	
}
void LCD_Rectangle_Handle(unsigned short X, unsigned short Y, unsigned short Width, unsigned short High, unsigned char handle)
{
	INT8U cmd_id = 0xc7;
	INT8U err;
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	LCD_SendCmdOrData((INT8U)handle,0);
	LCD_SendCmdOrData((INT8U)Y,0);
	LCD_SendCmdOrData((INT8U)X,0);
	LCD_SendCmdOrData((INT8U)(Width/8),0);
	LCD_SendCmdOrData((INT8U)(Width%8),0);
	LCD_SendCmdOrData((INT8U)High,2);
	OSSemPost(LCDSem);
	
}
void LCD_DrawPicture_New(unsigned short X, unsigned short Y, unsigned short pictureWidth, 
						 unsigned short pictureHigh, unsigned char * pictureBuffer)
{
	INT8U cmd_id = 0xfd;
	INT8U err,i;
	i = 0;
	if(pictureBuffer == 0)
	{
		return;
	}
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	LCD_SendCmdOrData((INT8U)Y,0);
	LCD_SendCmdOrData((INT8U)X,0);
	LCD_SendCmdOrData((INT8U)(pictureWidth/8),0);
	LCD_SendCmdOrData((INT8U)pictureHigh,0);
	while(i < (pictureWidth/8)*pictureHigh)
	{	
		if(i == ((pictureWidth/8)*pictureHigh - 1))
		{
			LCD_SendCmdOrData(*pictureBuffer,2);
			break;
		}
		LCD_SendCmdOrData(*pictureBuffer,0);
		pictureBuffer++;
		i++;
	}
	OSSemPost(LCDSem);
}
void LCD_New_Light_On(void)
{
	INT8U cmd_id = 0xb5;
	INT8U err;
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	LCD_SendCmdOrData(1,2);
	OSSemPost(LCDSem);
}
#endif
INT8U LCD_DrawText(unsigned short X, unsigned short Y, char * strText)
{
#ifdef LCD_MODULE_ZX19264M1A
	
#if 0
	INT8U cmd_id = 0xeb;
	INT8U err;
	if((*strText == 0) || (strText == 0))
	{
		return;
	}
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	LCD_SendCmdOrData((INT8U)Y,0);
	LCD_SendCmdOrData((INT8U)X,0);
	while(*strText != 0)
	{	
		LCD_SendCmdOrData((INT8U)(*strText),0);
		strText++;
	}
	LCD_SendCmdOrData(0,2);
	OSSemPost(LCDSem);
#else
	INT8U cmd_id = 0xe9;
	INT8U err;
	if((*strText == 0) || (strText == 0))
	{
		return False;
	}
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	LCD_SendCmdOrData((INT8U)Y/16,0);
	LCD_SendCmdOrData((INT8U)X/8,0);
	while(*strText != 0)
	{	
		LCD_SendCmdOrData((INT8U)(*strText),0);
		strText++;
	}
	LCD_SendCmdOrData(0,2);
	OSSemPost(LCDSem);
#endif
	return 1;
#else
	INT8U ch, ret, filled = 0, clipped = 0;
	INT8U err;
	
	OSSemPend(LCDSem, 0, &err);
	
	while(* strText != 0)
	{
		ch = (INT8U)*strText;	
		if (ch < 30)
			ch = 30;
		else if (ch > 129)
			ch = 129;
		
		ch -= 30;
		
		strText ++;
		
		ret = LCD_DrawPicture_Internal(X, Y, 8, 16, &ASC_Bmp_Tab[ch*16], 1);
		
		if(ret > 0) filled = 1;
		if(ret < 2) clipped = 1;
		
		X += 8;
	}
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
#endif
}

INT8U LCD_DrawChineseText(unsigned short X, unsigned short Y, unsigned short * strText)
{
#ifdef LCD_MODULE_ZX19264M1A
	LCD_DrawText(X,Y,(char*)strText);
	return 1;
#else
	INT8U ret, filled = 0, clipped = 0, err;
	INT16U ch;
	
	OSSemPend(LCDSem, 0, &err);
	
	while(* strText != 0)
	{
		ch = (INT16U)*strText;	
		strText ++;
		
		ret = LCD_DrawPicture_Internal(X, Y, 16, 16, hztab[ch], 0);
		
		if(ret > 0) filled = 1;
		if(ret < 2) clipped = 1;
		X += 16;
	}
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
#endif
}

INT8U LCD_DrawMixedText(unsigned short X, unsigned short Y, unsigned short * strText)
{
#ifdef LCD_MODULE_ZX19264M1A
	LCD_DrawText(X,Y,(char*)strText);
	return 1;
#else
	INT8U ret, filled = 0, clipped = 0, err;
	INT16U ch;
	
	OSSemPend(LCDSem, 0, &err);
	
	while(* strText != 0)
	{
		ch = (INT16U)*strText;	
		strText ++;
		
		if(ch >= 0x100)
		{
			ret = LCD_DrawPicture_Internal(X, Y, 16, 16, hztab[ch-0x100], 0);
			X += 16;
		}
		else
		{
			if (ch < 30)
				ch = 30;
			else if (ch > 129)
				ch = 129;
			
			ch -= 30;
			
			ret = LCD_DrawPicture_Internal(X, Y, 8, 16, &ASC_Bmp_Tab[ch*16], 1);
			X += 8;
		}  
		if(ret > 0) filled = 1;
		if(ret < 2) clipped = 1;
	}
	
	LCD_WriteCMD(LCD_CMD_DISPLAY_ON);
	
	OSSemPost(LCDSem);
	
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
#endif
}


//For language setting store.
INT8U GetLanguageIndex()
{
	return LCDMenuConfig.LanguageIndex;
}
INT8U UI_Get_Language(void * pData)
{
	switch(LCDMenuConfig.LanguageIndex)
	{
	case 3:
		return 0;
	case 0:
	default:
		return 1;
	}
}

INT8U UI_Set_Language(void * pData, int Index)
{
	if(Index < 0 && Index >= 4)
		return False;
	switch(Index)
	{
	case 0:
		Index = 3;
		break;
	case 1:
		Index = 0;
		break;
	default:
		Index = 0;
	}
	if(password.m_bLanguage == Index)
	{
		if(LCDMenuConfig.LanguageIndex != Index)
		{
			LCDMenuConfig.LanguageIndex = Index;
			SaveLCDMenuConfig();
		}
		return True;
	}
	return False;
}

INT8U LCD_SetDisStatusEx(enum StatusTypeEnum status, INT32U errorcode, INT8U percent, void * extraInfo)
{
	if(kb_installed)
		return LCD_SetDisStatus(status, errorcode, percent, extraInfo);
	else
		return False;
}

INT8U LCD_SetMainInfoEx(enum InfoTypeEnum infoType, void * value)
{
	if(kb_installed)
		return LCD_SetMainInfo(infoType, value);
	else
		return False;
}


#ifdef EPSON_BOTTOM_BOARD_V2

#ifdef LCD_TEST

INT8U LCD_SetDisStatus(enum StatusTypeEnum status, INT32U errorcode, INT8U percent, void * extraInfo)
{ //temp testing.
	return 0;
}
INT8U LCD_SendKey(unsigned int nKeyID,  enum KeyEventEnum keyEvent) // 传递键盘事件
{ //temp testing.
	
	LCD_ClearScreen();
	
	switch(nKeyID)
	{
	case KEYID_LEFT:
		LCD_DrawText(0, 0, "Left");
		break;
	case KEYID_RIGHT:
		LCD_DrawText(0, 0, "Right");
		break;
	case KEYID_UP:
		LCD_DrawText(0, 0, "Up");
		break;
	case KEYID_DOWN:
		LCD_DrawText(0, 0, "Down");
		break;
	case KEYID_MENU:
		LCD_DrawText(0, 0, "Menu");
		break;
	case KEYID_ENTER:
		LCD_DrawText(0, 0, "Enter");
		break;
	case KEYID_CANCEL:
		LCD_DrawText(0, 0, "Cancel");
		break;
	case KEYID_PAUSE:
		LCD_DrawText(0, 0, "Pause");
		break;
	case KEYID_CLEAN:
		LCD_DrawText(0, 0, "Cleaning");
		break;
	case KEYID_BASEPOINT:
		LCD_DrawText(0, 0, "Basepoint");
		break;
	case KEYID_SHEETCUT:
#if defined(QICAIHONG)
		LCD_DrawText(0, 0, "ADVANCE MODE");
#else
		LCD_DrawText(0, 0, "SheetCut");
#endif
		break;
	case KEYID_PRINTQUALITY:
#if defined(QICAIHONG)
		LCD_DrawText(0, 0, "MEDIA TYPE");
#else
		LCD_DrawText(0, 0, "PrintQuality");
#endif
		break;
	case KEYID_CHECKNOZZLE:
		LCD_DrawText(0, 0, "CheckNozzle");
		break;
	case KEYID_CAPPING_UNCAPPING:
		LCD_DrawText(0, 0, "Cap/Uncap");
		break;
#if defined(QICAIHONG)
	case KEYID_HEATER:
		LCD_DrawText(0, 0, "Heater");
		break;
#endif
	default:
		LCD_DrawText(0, 0, "Undefined");
		break;
	}
	if(keyEvent == KeyDown)
	{
		LCD_DrawText(0, 16, "KeyDown");
	}
	else
	{
		LCD_DrawText(0, 16, "KeyUp");
	}
	switch (nKeyID)
	{
	case KEYID_LEFT:
		if(keyEvent == KeyDown)
			return UIAction_Move_Left_Start(0);
		else
			return UIAction_Move_Left_End(0);
	case KEYID_RIGHT:
		if(keyEvent == KeyDown)
			return UIAction_Move_Right_Start(0);
		else
			return UIAction_Move_Right_End(0);
	case KEYID_UP:
		if(keyEvent == KeyDown)
			return UIAction_Move_Up_Start(0);
		else
			return UIAction_Move_Up_End(0);
	case KEYID_DOWN:
		if(keyEvent == KeyDown)
			return UIAction_Move_Down_Start(0);
		else
			return UIAction_Move_Down_End(0);
	case KEYID_CANCEL:
		if(keyEvent == KeyUp)
			return UIAction_CancelPrint(0);
	case KEYID_PAUSE:
		if(keyEvent == KeyUp)
			return UIAction_PauseResumePrint(0);
	case KEYID_CLEAN:
		if(keyEvent == KeyUp)
			return UIAction_QuickCleaning(0);
	case KEYID_BASEPOINT:
		if(keyEvent == KeyUp)
			return UIAction_SetBasePoint(0);
	case KEYID_SHEETCUT:
#if defined(MANUFACTURER_MICOLOR_EPSON)
		if(keyEvent == KeyUp)
			return UIAction_SheetCut(0);
#endif
	case KEYID_CHECKNOZZLE:
		if(keyEvent == KeyUp)
			return UIAction_CheckNozzle(0);
	case KEYID_MENU:
#if defined(MANUFACTURER_MICOLOR_EPSON)
		if(keyEvent == KeyUp)
		{
			LCD_DrawText(0, 32, "LoadMedia");
			return Menu_LoadMedia(0,0);
		}
#endif
	case KEYID_ENTER:
		if(keyEvent == KeyUp)
		{
			LCD_DrawText(0, 32, "MeasureMedia");
			return Menu_MeasureMedia(0,0);
		}
#if defined(QICAIHONG)
	case KEYID_HEATER:
		//LCD_DrawText(0, 0, "Heater"); // open menu
		break;
	case KEYID_PRINTQUALITY:
		// open media type menu
		break;
#endif
	default:
		return 0;
	}
	return True;    
}
INT8U LCD_SetMainInfo(enum InfoTypeEnum infoType, void * value)
{ //temp testing.
	return 0;
}

void Initial_Dis()
{
}
#endif

#endif

#endif
#endif


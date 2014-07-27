
#include "os_includes.h"
#include "board.h"
#include "global.h"
#include "keylcd.h"
#include "display.h"
#include "miscellance.h"


static const Pin LcdBusyPin = PIN_KB_LCD_BUSY;
static const Pin ReSetPin = PIN_KB_LCD_nLED;

OS_EVENT *LCDSem;

void LCD_Rectangle_Handle(unsigned short X, unsigned short Y,
		unsigned short Width, unsigned short High, unsigned char handle);
void LCD_DrawPicture_New(unsigned short X,
		unsigned short Y, unsigned short pictureWidth,
		unsigned short pictureHigh, unsigned char * pictureBuffer);

INT8U Lcd_Cmd_Q[LCD_MAX_CMD_SIZE] = {0};
INT16U Lcd_Cmd_Num = 0;

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

void LCD_Init(void)
{
	INT8U err;
	PIO_ConfigureIt(&ReSetPin, Reset_key_Irq_Interrupt);
	PIO_EnableIt(&ReSetPin);
	PIO_ConfigureIt(&LcdBusyPin, LCD_Irq_Interrupt);
	PIO_EnableIt(&LcdBusyPin);
	OSFlagPost(mix_FLAG_GRP, LCD_BUSY_STATE, OS_FLAG_CLR, &err);

	LCDSem = OSSemCreate(1);             /* Random number semaphore */
}

void LCD_SendCmdOrData(INT8U ch,INT8U index);
void LCD_ClearScreen(void) //«Â∆¡
{
	INT8U cmd_id = 0xF4;
	INT8U err;
	OSSemPend(LCDSem, 0, &err);
	LCD_SendCmdOrData(cmd_id,1);
	OSSemPost(LCDSem);
}
INT8U LCD_FillRectangle(unsigned short X, unsigned short Y,
		unsigned short Width,unsigned short High, unsigned char isBlack)
{
	if(isBlack==0&&(Width == LCD_PIXEL_WIDTH
				&& High ==LCD_PIXEL_HIGH && Y ==0 && Y==0))
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
}

INT8U LCD_DrawPicture(unsigned short X, unsigned short Y,
		unsigned short pictureWidth, unsigned short pictureHigh,
		unsigned char * pictureBuffer)
{
	LCD_DrawPicture_New(X,Y,pictureWidth,pictureHigh,pictureBuffer);
	return 1;
}

INT8U LCD_InverseRegion(unsigned short X, unsigned short Y,
		unsigned short regionWidth, unsigned short regionHigh)
{
	LCD_Rectangle_Handle(X,Y,regionWidth,regionHigh,2);
	return 1;
}

#ifdef LCD_MODULE_ZX19264M1A
void LCD_SendCmdOrData(INT8U ch,INT8U index)
{
	Lcd_Cmd_Q[Lcd_Cmd_Num++] = ch;
}
void LCD_Rectangle_Handle(unsigned short X, unsigned short Y,
		unsigned short Width, unsigned short High, unsigned char handle)
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
void LCD_DrawPicture_New(unsigned short X, unsigned short Y,
		unsigned short pictureWidth, unsigned short pictureHigh,
		unsigned char * pictureBuffer)
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
#endif
INT8U LCD_DrawText(unsigned short X, unsigned short Y, char * strText)
{
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
	return 1;
}

INT8U LCD_DrawMixedText(unsigned short X, unsigned short Y,
		unsigned short * strText)
{
	LCD_DrawText(X,Y,(char*)strText);
	return 1;
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

INT8U LCD_SetDisStatusEx(enum StatusTypeEnum status, INT32U errorcode,
		INT8U percent, void * extraInfo)
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


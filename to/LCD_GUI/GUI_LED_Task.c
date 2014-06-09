
#include "os_cpu.h"
#include "ucos_ii.h"
#include "global.h"
#include "GUI_LED_Task.h"
#include "Menu_GUI.h"
//#include "miscellance.h"
//#include "tasks.h"

#ifdef IIC_Key_Board_LCD 
OS_STK  TaskFLICKERCaliStk[GUI_FLICKER_STK_SIZE];
OS_FLAG_GRP *LCD_FLAG_GRP;

INT8U Flicker_Init()
{
	return 0;
}

void Flicker_Task (void *data)
{
	INT8U err;
	INT8U IsOpen = 0;
	
	while(1)
	{
		OS_FLAGS flags = OSFlagPend(LCD_FLAG_GRP,
									GUI_FLICKER_BUSY|GUI_FLICKER_K|GUI_FLICKER_C|GUI_FLICKER_M|GUI_FLICKER_Y|GUI_FLICKER_LM|GUI_FLICKER_LC|GUI_FLICKER_reserve,
									OS_FLAG_WAIT_CLR_ANY|OS_FLAG_WAIT_SET_ANY,
									0,&err);
		
		//OSSemPend(IIC_KeyboardSem, 0, &err);
		INT8U x = 0,y = 0;
		if((flags & GUI_FLICKER_BUSY) == GUI_FLICKER_BUSY)
		{
			LCD_FillRectangle(x, y, 8, 8, IsOpen);
			x+=8;
		}
		if((flags & GUI_FLICKER_K) == GUI_FLICKER_K)
		{
			LCD_FillRectangle(x, y, 8, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"K");
			if(IsOpen)
				LCD_InverseRegion(x, y, 8, LCD_MENUITEM_HEIGHT);
			x+=8;
		}
		if((flags & GUI_FLICKER_C) == GUI_FLICKER_C)
		{
			LCD_FillRectangle(x, y, 8, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"C");
			if(IsOpen)
				LCD_InverseRegion(x, y, 8, LCD_MENUITEM_HEIGHT);
			x+=8;
		}
		if((flags & GUI_FLICKER_M) == GUI_FLICKER_M)
		{
			LCD_FillRectangle(x, y, 8, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"M");
			if(IsOpen)
				LCD_InverseRegion(x, y, 8, LCD_MENUITEM_HEIGHT);
			x+=8;
		}
		if((flags & GUI_FLICKER_Y) == GUI_FLICKER_Y)
		{
			LCD_FillRectangle(x, y, 8, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"Y");
			if(IsOpen)
				LCD_InverseRegion(x, y, 8, LCD_MENUITEM_HEIGHT);
			x+=8;
		}
		if((flags & GUI_FLICKER_LM) == GUI_FLICKER_LM)
		{
			LCD_FillRectangle(x, y, 16, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"LM");
			if(IsOpen)
				LCD_InverseRegion(x, y, 16, LCD_MENUITEM_HEIGHT);
			x+=16;
		}
		if((flags & GUI_FLICKER_LC) == GUI_FLICKER_LC)
		{
			LCD_FillRectangle(x, y, 16, LCD_MENUITEM_HEIGHT, 0);
			LCD_DrawText(x, y,"LC");
			if(IsOpen)
				LCD_InverseRegion(x, y, 16, LCD_MENUITEM_HEIGHT);
			x+=16;
		}
		if(IsOpen == 1)
			IsOpen = 0;
		else
			IsOpen = 1;
		//OSSemPost(IIC_KeyboardSem);           /* Release semaphore */
	}
}

void StartFlickerTask()
{	
	INT8U err;
	
	LCD_FLAG_GRP = OSFlagCreate(0x0000, &err); //USB Communication Flags      	
	OSTaskCreate(Flicker_Task, (void *)0, (void *)&TaskFLICKERCaliStk[GUI_FLICKER_STK_SIZE-1], GUI_FLICKER_TASK_PRIO);
}

#endif

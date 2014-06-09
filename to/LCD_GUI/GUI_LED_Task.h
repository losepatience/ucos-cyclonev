#ifndef	__GUI_LED_Task_h__
#define	__GUI_LED_Task_h__

#define  GUI_FLICKER_STK_SIZE   128

#define GUI_FLICKER_TASK_PRIO 13

#define GUI_FLICKER_BUSY		0x10000000
#define GUI_FLICKER_K			0x20000000
#define GUI_FLICKER_C			0x40000000
#define GUI_FLICKER_M			0x80000000
#define GUI_FLICKER_Y			0x01000000
#define GUI_FLICKER_LM			0x02000000
#define GUI_FLICKER_LC			0x04000000
#define GUI_FLICKER_reserve		0x08000000

extern OS_FLAG_GRP *LCD_FLAG_GRP;
INT8U Flicker_Init(void);
void Flicker_Task(void* data);
void StartFlickerTask();

#endif


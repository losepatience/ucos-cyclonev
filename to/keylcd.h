#ifndef _KEYLCD_H_
#define _KEYLCD_H_

#include "os_includes.h"

#include "pio/pio.h"

#include "lcd.h"

/*
功能键寄存器FunctionKey 的初始值是FFH，每一个位对应一个功能键，
第0 位（LSB）对应F0，第1 位对应F1，依次类推，第7位（MSB）对应F7。
某一功能键被按下时，相应的FunctionKey 位就清零。功能键还有一
个特性就是二次中断，按下时产生一次中断信号，抬起时又会产生一次中断信号；而普
通键只会在被按下时产生一次中断。*/

//定义ZLG7290内部寄存器地址（子地址）
#define ZLG7290_SystemReg		0x00		//系统寄存器, Same as INT
#define ZLG7290_Key				0x01		//键值寄存器
#define ZLG7290_RepeatCnt		0x02		//连击次数寄存器
#define ZLG7290_FunctionKey		0x03		//功能键寄存器
#define ZLG7290_CmdBuf			0x07		//命令缓冲区起始地址
#define ZLG7290_CmdBuf0			0x07		//命令缓冲区0
#define ZLG7290_CmdBuf1			0x08		//命令缓冲区1
#define ZLG7290_FlashOnOff		0x0C		//闪烁控制寄存器
#define ZLG7290_ScanNum			0x0D		//扫描位数寄存器
#define ZLG7290_DpRam			0x10		//显示缓存起始地址
#define ZLG7290_DpRam0			0x10		//显示缓存0
#define ZLG7290_DpRam1			0x11		//显示缓存1
#define ZLG7290_DpRam2			0x12		//显示缓存2
#define ZLG7290_DpRam3			0x13		//显示缓存3
#define ZLG7290_DpRam4			0x14		//显示缓存4
#define ZLG7290_DpRam5			0x15		//显示缓存5
#define ZLG7290_DpRam6			0x16		//显示缓存6
#define ZLG7290_DpRam7			0x17		//显示缓存7


//#define IIC_2410_ADDR	0x26 //Address = 0x13 last bit is not address
//#define IIC_KB_ADDR	0x70 //0x38: pure, 0x71: read, 0x70:write
#define IIC_2410_ADDR	0x13
#define IIC_KB_ADDR	0x38
#define IIC_MAX11614_ADDR	0x33

//#define IIC_INT_PENDING	(rIICCON&0x10)  //Pending status

#define IIC_ClearPending()	TWI_GetStatus(BOARD_BASE_TWI_EEPROM)
//#define IIC_EEPROM_ADDR 0xA2 //10100010(0xA2) write, 0xA3 Read
#ifdef ek_board_sim
#define E2PROM_PAGESIZE   64
#define IIC_EEPROM_ADDR 0x50
#else
#define E2PROM_PAGESIZE   64
#define IIC_EEPROM_ADDR 0x50 //10100010(0xA2) write, 0xA3 Read
#endif

#define LCD_GROUND_LIGHT_ON()	(PIO_Set(&LcdLEDPin))
#define LCD_GROUND_LIGHT_OFF()	(PIO_Clear(&LcdLEDPin))

#define LCD_E_HIGH()	(PIO_Set(&LcdEnPin))
#define LCD_E_LOW()	(PIO_Clear(&LcdEnPin))

#define LCD_SET_DATAOUT()   (PIO_SET_OUTPUT(&LcdDataPins))
#define LCD_SET_DATAIN()    (PIO_SET_INPUT(&LcdDataPins))

#define LCD_EH_DLY	(20*5)	//Enable High Time
//#define LCD_EH_DLY	(100*5)	//Enable High Time
#define LCD_EL_DLY	(10*5)	//Enable Low Time
#define LCD_EL_AND_BUSY_DLY	(200*5)	//Enable Low Time, must be 200, otherwise, release version can not work 

//#define LCD_EH_DLY	10	//Enable High Time
//#define LCD_EL_DLY	180	//Enable Low Time, include waiting time between 
//void LCD_DisplayON(void);
//void LCD_DisplayOFF(void);
//void LCD_Clr(void);
//void LCD_AllON(void);
//void LCD_Test(void);
//void LCD_Text(INT8U colum,INT8U  page, const char *str);
//void LCD_TextChinese(INT8U colum4,INT8U  page4,INT8U *str1);/* 写汉字字符串 */                     

#endif

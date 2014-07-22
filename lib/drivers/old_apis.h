#ifndef __OLD_APIS_H__
#define __OLD_APIS_H__

#include <asm/types.h>
#include <stdbool.h>
#include <stdio.h>

#define BOOL	bool
#define False	false
#define FALSE	false
#define True	true
#define TRUE	true

#define U32	u32
#define U8	u8
#define UBYTE	u8


#define TRACE_INFO(fmt, args...) pr_info(fmt, ##args)

u8 IIC_WriteEEPRom(u16 addr, u8 *buf, u8 *len);
inline u8 IIC_WriteEEPRom_Ex(u16 addr, u8 *buf, u16 *len);
u8 IIC_ReadEEPRom(u16 addr, u8 *buf, u8 *len);
inline u8 IIC_ReadEEPRom_Ex(u16 addr, u8 *buf, u16 *len);

u8 IIC_Write_Max11614(u8 reg, u8 data, u8 pending);
float IIC_Read_Max11614(u8 Flag);

u8 IIC_WriteRegs(u8 reg, u8 *buf, u8 cnt);
inline u8 IIC_WriteReg(u8 reg, u8 data);
u8 IIC_ReadReg(u8 reg, u8 *buf, u8 cnt);

u8 IIC_Write_LCD_32(u8 reg, u8 *buf, u8 cnt);
u8 IIC_Read_LCD(u8 reg, u8 *buf, u8 cnt);

void IIC_Init(void);


void UART_Init(u8 flag);
u8 UART_SendCMD(u8 num, u8 *data);

#endif


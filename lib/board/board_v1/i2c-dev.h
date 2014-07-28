/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jul 24 14:22:55 CST 2014
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __IIC_DEV__
#define __IIC_DEV__

#include <asm/types.h>

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

#endif

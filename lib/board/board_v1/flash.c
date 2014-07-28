/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jul  8 15:36:51 CST 2014
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

#include <stdio.h>
#include <stdbool.h>
#include <asm/types.h>
#include <config.h>

#include <job.h>
#include <hugebuf.h>
#include <status.h>
#include <flash.h>
#include <lcd.h>

#define VERSION_TAG_OFFSET	(0x200 + 0x30)
#define GZ_KEY_TAG_OFFSET	(0x200 + 0x3C)
#define INK_PWD_TAG_OFFSET	(0x200 + 0x40)
#define HW_CFG_TAG_OFFSET	(0x200 + 0x44)

s8 Flash_UpdateCode(struct Struct_Updater *info, u8 idx)
{
	int rval;
	char *start;
	ulong offset;

	u32 checks[] = {
		0x334E5342, 0x49313235, 0x10001, 0x554B4559, 0x504B4E49
	};
	
	start = (char *)HugeBuf_GetCodeAddr(info->data_bytescnt);
	start += info->SubAddressTable[idx];
	offset = FLASH_OS0_OFFSET;
	
	if (info->FileVersion == 2 && info->SubSizeTable[idx] != 0)
		DeSecurity((u8 *)start, (u8 *)start, 0x400);

	if (memcmp(start + VERSION_TAG_OFFSET, checks, 3 * sizeof(u32))) {	
		pr_err("Unsupported files!\r\n");
		status_ReportStatus(STATUS_UPDATEERR_ILIGALFILE,STATUS_SET);
		return false;
	}
	
#ifdef GZ_KEY_USED	
	if ((*(u32 *)(start + GZ_KEY_TAG_OFFSET) != 0x554B4559)) {	
		status_ReportStatus(STATUS_UPDATEERR_GZ_ILIGALFILE,STATUS_SET);
		return false;
	}
#endif	
	
#ifdef INK_PASSWORD	
	if ((*(u32 *)(start + INK_PWD_TAG_OFFSET) != 0x504B4E49)) {	
		status_ReportStatus(STATUS_UPDATEERR_INKP_ILIGALFILE,STATUS_SET);
		return false;
	}
#endif	
	
	if (info->FileVersion != 2 && info->SubSizeTable[idx] != 0)
		EnSecurity((u8 *)start, (u8 *)start, info->SubSizeTable[idx]);
	else
		EnSecurity((u8 *)start, (u8 *)start, 0x400);
	
	LCD_Text(0,0, "Updating Main FW... ");

	rval = flash_write(start, offset, info->SubSizeTable[idx]);
	if (rval < info->SubSizeTable[idx]) {
		status_ReportStatus(STATUS_UPDATEERR_FLASHOP, STATUS_SET);	
		LCD_Text(0,0, "Update Main FW Failed!  ");		
		return false;
	}

	LCD_Text(0,0, "Update Main FW Success! ");
	return true;
}

s8 Flash_UpdateFPGA(struct Struct_Updater *info, u8 idx)
{
	int rval;
	char *start;
	ulong offset;
	
	start = (char *)HugeBuf_GetCodeAddr(info->data_bytescnt);
	start += info->SubAddressTable[idx];
	offset = FLASH_FPGA_OFFSET;
	
	LCD_Text(0,0, "Updating FPGA...");
	
	rval = flash_write(start, offset, info->SubSizeTable[idx]);
	if (rval < info->SubSizeTable[idx]) {
		pr_err("failed to update firmware(%d)\n", rval);
		status_ReportStatus(STATUS_UPDATEERR_FLASHOP, STATUS_SET);
		LCD_Text(0,0, "Update FPGA Failed! ");
		return false;
	}

	LCD_Text(0,0, "Update FPGA Success! ");
	return true;
}


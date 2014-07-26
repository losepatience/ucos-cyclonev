/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue May  6 18:59:40 CST 2014
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

#ifndef __FLASH_H__
#define __FLASH_H__

#include <spi_flash.h>
/* #include <nand_flash.h> */

int flash_write(char *src, ulong addr, ulong cnt);
int flash_read(char *des, ulong addr, ulong cnt);

int flash_init(unsigned int bus, unsigned int cs,
	unsigned int max_hz, unsigned int mode);

#endif

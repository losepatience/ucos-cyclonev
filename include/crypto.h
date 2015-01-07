/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Aug 19 10:58:26 CST 2014
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

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <asm/types.h>

extern u8 const crc8_CCITT_table[256];
extern u8 const w1_crc8_table[256];

extern u8 crc8(const u8 table[256], u8 *buf, size_t len, u8 crc);
extern u8 w1_crc8(unsigned char *data, int len);

extern u16 const crc16_table[256];
extern u16 crc16(u16 crc, const u8 *buffer, size_t len);

extern void __sha1(u32 sha1[5], const u8 data[64]);
extern void w1_sha1(u8 *mac, const u8 data[64]);

#endif

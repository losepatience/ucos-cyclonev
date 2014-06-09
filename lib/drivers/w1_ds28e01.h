/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
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

#ifndef __W1_DS28E01_H__
#define __W1_DS28E01_H__

#define W1_F2F_PAGE_NUM		0x05
#define W1_F2F_PAGE_SIZE	0x20

#define W1_F2F_READ_ROMID	0x33
#define W1_F2F_READ_EEPROM	0xf0
#define W1_F2F_WRITE_SCRATCH	0x0f
#define W1_F2F_READ_SCRATCH	0xaa
#define W1_F2F_COPY_SCRATCH	0x55
#define W1_F2F_LOAD_SECRET	0x5a
#define W1_F2F_WRITE_CHALLENGE	0x0f
#define W1_F2F_READ_AUTH_PAGE	0xa5

extern u32 owBoardID;
extern u16 owManufacturerID;

struct w1_master {
	void	*data;
	void	*slave;

	int	(*read_bit)(void *);
	void	(*write_bit)(void *, u8);

	u8	(*read_byte)(void *);
	void	(*write_byte)(void *, u8);

	ssize_t	(*read_block)(void *, u8 *, int);
	ssize_t	(*write_block)(void *, const u8 *, int);

	u8	(*reset_bus)(void *);
	u8	(*set_pullup)(void *, int);
};

struct w1_ds28e01 {

	u8 rd_buf[64];
	u8 wr_buf[64];
	u8 sha1_src[64];

	unsigned page_size;
	unsigned sub_page_size;

	spinlock_t w1_lock;
	struct w1_master *master;
};

#endif

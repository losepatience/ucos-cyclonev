/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jul 30 11:24:49 CST 2014
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

#ifndef __W1_H__
#define __W1_H__

#include <platform.h>

#define W1_SEARCH		0xF0
#define W1_ALARM_SEARCH		0xEC
#define W1_CONVERT_TEMP		0x44
#define W1_SKIP_ROM		0xCC
#define W1_READ_SCRATCHPAD	0xBE
#define W1_READ_ROM		0x33
#define W1_READ_PSUPPLY		0xB4
#define W1_MATCH_ROM		0x55
#define W1_RESUME_CMD		0xA5

struct w1_master {
	unsigned long	irqflags;

	unsigned char	(*read_bit)(void);
	void		(*write_bit)(unsigned char);

	unsigned char	(*read_byte)(void);
	void		(*write_byte)(unsigned char);

	unsigned char	(*read_block)(unsigned char *, int);
	void		(*write_block)(const unsigned char *, int);

	unsigned char	(*reset)(void);

	void		*data;
};

struct w1_slave
{
	unsigned char		rom[9];
	unsigned char		cmd[3];

	struct w1_master	*master;
	void			*priv;
};

int w1_reset(struct w1_slave *sl);
unsigned char w1_read_8(struct w1_slave *);
void w1_write_8(struct w1_slave *, unsigned char);
int w1_read_block(struct w1_slave *, unsigned char *, int);
void w1_write_block(struct w1_slave *, const unsigned char *, int);
int w1_reset_slave(struct w1_slave *);
int w1_read_rom(struct w1_slave *, unsigned char *, int);

#endif

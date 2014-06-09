/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Mar 25 17:32:54 CST 2014
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

#ifndef __FIFO_H__
#define __FIFO_H__

#include <asm/types.h>
#include <platform.h>

struct fifo {
	void		*data;
	u32		esize;

	u32		count;	/* element count */
	u32		unused;

	u32		in;
	u32		out;

	spinlock_t	lock;
};

inline u32 fifo_unused(struct fifo *fifo);
inline u32 fifo_cached(struct fifo *fifo);

u32 fifo_in(struct fifo *fifo, const void *src, u32 cnt);
u32 fifo_out(struct fifo *fifo, void *des, u32 cnt);

void *fifo_iaddr(struct fifo *fifo);
void *fifo_oaddr(struct fifo *fifo);

u32 fifo_cnt2size(struct fifo *fifo, int cnt);
int fifo_size2cnt(struct fifo *fifo, u32 size);

void fifo_reset(struct fifo *fifo);
struct fifo *fifo_init(void *buffer, u32 esize, u32 cnt);

#endif

/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jul 23 10:41:48 CST 2014
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

#ifndef __CIRC_H
#define __CIRC_H

#include <fifo.h>
#include <stdbool.h>

typedef struct __rx_info {
	bool		first;
	int		remainning;
	int		cnt;
} __rx_info_t;

typedef struct circ {
	struct fifo	*fifo;

	int		sizeof_header;	/* const */
	int		sizeof_band;
	void		*nextheader;	/* FlushCurBand */
	int		band_left;

	__rx_info_t	xinfo;
} circ_t;

circ_t *__to_circ(void);

#endif


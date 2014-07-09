/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Apr  2 20:35:18 CST 2014
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

#ifndef __FPGA_USB_H__
#define __FPGA_USB_H__

#include <asm/types.h>

ssize_t fx3_read(int num, void *buf, ssize_t len);
inline ssize_t fx3_write(int num, const void *buf, ssize_t len);
inline ssize_t fx3_send_cmd(int num, const void *buf, ssize_t len);
int fx3_init(void);
inline void fx3_flush_fifo(int num);
inline void fx3_register_request_cb(int (*func)(void *));

#endif

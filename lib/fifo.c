/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Mar  6 16:33:42 CST 2014
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the teresize of the GNU General Public License as
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
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <fifo.h>

inline unsigned long fifo_unused(struct fifo *fifo)
{
	return fifo->unused;
}

inline unsigned long fifo_unused_nowrap(struct fifo *fifo)
{
	return (fifo->count - fifo->in) * fifo->esize;
}

inline unsigned long fifo_cached(struct fifo *fifo)
{
	return fifo->count - fifo->unused;
}

inline unsigned long fifo_cached_nowrap(struct fifo *fifo)
{
	return (fifo->count - fifo->out) * fifo->esize;
}

static void __fifo_in(struct fifo *fifo, const void *src,
			unsigned long len, unsigned long off)
{
	unsigned long size;
	unsigned long l;

	if (src == NULL)
		return;

	off *= fifo->esize;
	len *= fifo->esize;
	size = fifo->count * fifo->esize;

	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);
}

unsigned long fifo_in(struct fifo *fifo, const void *src, unsigned long cnt)
{
	unsigned long unused;

	unused = fifo_unused(fifo);
	if (cnt > unused)
		cnt = unused;

	__fifo_in(fifo, src, cnt, fifo->in);
	fifo->in = (fifo->in + cnt) % fifo->count;
	fifo->unused -= cnt;

	return cnt;
}

static void __fifo_out(struct fifo *fifo, void *des,
			unsigned long len, unsigned long off)
{
	unsigned long size;
	unsigned long l;

	if (des == NULL)
		return;

	off *= fifo->esize;
	len *= fifo->esize;
	size = fifo->count * fifo->esize;

	l = min(len, size - off);

	memcpy(des, fifo->data + off, l);
	memcpy(des + l, fifo->data, len - l);
}

/*
 * a spinlock could be a waste of the system,
 * but it makes this routine could be called in irq.
 */
unsigned long fifo_out(struct fifo *fifo, void *des, unsigned long cnt)
{
	unsigned long cached;

	cached = fifo_cached(fifo);
	if (cnt > cached)
		cnt = cached;

	__fifo_out(fifo, des, cnt, fifo->out);

	/* take a warning, (fifo->out + cnt) could overflow */
	fifo->out = (fifo->out + cnt) % fifo->count;
	fifo->unused += cnt;

	return cnt;
}

inline void *fifo_iaddr(struct fifo *fifo)
{
	return fifo->data + fifo->in * fifo->esize;
}

inline void *fifo_oaddr(struct fifo *fifo)
{
	return fifo->data + fifo->out * fifo->esize;
}

void *fifo_oaddr_plus(struct fifo *fifo, unsigned long size)
{
	unsigned long idx;
	int cnt;

	cnt = fifo_size2cnt(fifo, size);
	idx = (fifo->out + cnt) % fifo->count;

	return fifo->data + idx * fifo->esize;
}

inline unsigned long fifo_cnt2size(struct fifo *fifo, int cnt)
{
	return cnt * fifo->esize;
}

inline int fifo_size2cnt(struct fifo *fifo, unsigned long size)
{
	return size / fifo->esize;
}

void fifo_reset(struct fifo *fifo)
{
	fifo->unused	= fifo->count;
	fifo->in	= 0;
	fifo->out	= 0;
}

struct fifo *fifo_init(void *buffer, unsigned long esize, unsigned long cnt)
{
	struct fifo *fifo = malloc(sizeof(struct fifo));
	if (fifo == NULL) {
		pr_err("%s(line%d): no memory\n", __func__, __LINE__);
		return NULL;
	}

	fifo->data	= buffer;
	fifo->esize	= esize;
	fifo->count	= cnt;
	fifo->unused	= cnt;
	fifo->in	= 0;
	fifo->out	= 0;

	return fifo;
}


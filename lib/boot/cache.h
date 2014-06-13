/*
 * arch/arm/include/asm/outercache.h
 *
 * Copyright (C) 2010 ARM Ltd.
 * Written by Catalin Marinas <catalin.marinas@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __CACHE_H
#define __CACHE_H

#include <asm/types.h>

struct maint_cache_fns {
	void (*inv_range)(unsigned long, unsigned long);
	void (*clean_range)(unsigned long, unsigned long);
	void (*flush_range)(unsigned long, unsigned long);
};

struct outer_cache_fns {
	void (*inv_range)(unsigned long, unsigned long);
	void (*clean_range)(unsigned long, unsigned long);
	void (*flush_range)(unsigned long, unsigned long);
	void (*flush_all)(void);
	void (*inv_all)(void);
	void (*disable)(void);
	void (*sync)(void);
};

extern struct maint_cache_fns maint_cache;
extern struct outer_cache_fns outer_cache;

/* ------------------------------------------------------------------------------
 * maint cache
 * ------------------------------------------------------------------------------
 */
static inline void maint_inv_range(unsigned long start, unsigned long end)
{
	if (maint_cache.inv_range)
		maint_cache.inv_range(start, end);
}
static inline void maint_clean_range(unsigned long start, unsigned long end)
{
	if (maint_cache.clean_range)
		maint_cache.clean_range(start, end);
}
static inline void maint_flush_range(unsigned long start, unsigned long end)
{
	if (maint_cache.flush_range)
		maint_cache.flush_range(start, end);
}

/* ------------------------------------------------------------------------------
 * outer cache
 * ------------------------------------------------------------------------------
 */
static inline void outer_inv_range(unsigned long start, unsigned long end)
{
	if (outer_cache.inv_range)
		outer_cache.inv_range(start, end);
}
static inline void outer_clean_range(unsigned long start, unsigned long end)
{
	if (outer_cache.clean_range)
		outer_cache.clean_range(start, end);
}
static inline void outer_flush_range(unsigned long start, unsigned long end)
{
	if (outer_cache.flush_range)
		outer_cache.flush_range(start, end);
}

static inline void outer_flush_all(void)
{
	if (outer_cache.flush_all)
		outer_cache.flush_all();
}

static inline void outer_inv_all(void)
{
	if (outer_cache.inv_all)
		outer_cache.inv_all();
}

static inline void outer_disable(void)
{
	if (outer_cache.disable)
		outer_cache.disable();
}

static inline void outer_sync(void)
{
	if (outer_cache.sync)
		outer_cache.sync();
}

/* ------------------------------------------------------------------------------
 * other
 * ------------------------------------------------------------------------------
 */
void __enable_cache(void);
int pl310_init(u32, u32);
#endif	/* __CACHE_H */

/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Jun 10 11:36:11 CST 2014
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
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include <platform.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <asm/types.h>
#include "cache.h"

#define L2X0_CACHE_ID			0x000
#define L2X0_CACHE_TYPE			0x004
#define L2X0_CTRL			0x100
#define L2X0_AUX_CTRL			0x104
#define L2X0_TAG_LATENCY_CTRL		0x108
#define L2X0_DATA_LATENCY_CTRL		0x10C
#define L2X0_EVENT_CNT_CTRL		0x200
#define L2X0_EVENT_CNT1_CFG		0x204
#define L2X0_EVENT_CNT0_CFG		0x208
#define L2X0_EVENT_CNT1_VAL		0x20C
#define L2X0_EVENT_CNT0_VAL		0x210
#define L2X0_INTR_MASK			0x214
#define L2X0_MASKED_INTR_STAT		0x218
#define L2X0_RAW_INTR_STAT		0x21C
#define L2X0_INTR_CLEAR			0x220
#define L2X0_CACHE_SYNC			0x730
#define L2X0_DUMMY_REG			0x740
#define L2X0_INV_LINE_PA		0x770
#define L2X0_INV_WAY			0x77C
#define L2X0_CLEAN_LINE_PA		0x7B0
#define L2X0_CLEAN_LINE_IDX		0x7B8
#define L2X0_CLEAN_WAY			0x7BC
#define L2X0_CLEAN_INV_LINE_PA		0x7F0
#define L2X0_CLEAN_INV_LINE_IDX		0x7F8
#define L2X0_CLEAN_INV_WAY		0x7FC
/*
 * The lockdown registers repeat 8 times for L310, the L210 has only one
 * D and one I lockdown register at 0x0900 and 0x0904.
 */
#define L2X0_LOCKDOWN_WAY_D_BASE	0x900
#define L2X0_LOCKDOWN_WAY_I_BASE	0x904
#define L2X0_LOCKDOWN_STRIDE		0x08
#define L2X0_ADDR_FILTER_START		0xC00
#define L2X0_ADDR_FILTER_END		0xC04
#define L2X0_TEST_OPERATION		0xF00
#define L2X0_LINE_DATA			0xF10
#define L2X0_LINE_TAG			0xF30
#define L2X0_DEBUG_CTRL			0xF40
#define L2X0_PREFETCH_CTRL		0xF60
#define L2X0_POWER_CTRL			0xF80
#define L2X0_DYNAMIC_CLK_GATING_EN	(1 << 1)
#define L2X0_STNDBY_MODE_EN		(1 << 0)

#define L2X0_AUX_CTRL_WAY_SIZE_MASK	(0x7 << 17)
#define L2X0_CTRL_EN			1
#define L2X0_WAY_SIZE_SHIFT		3


#define CONFIG_SYS_PL310_BASE		SOCFPGA_MPUL2_ADDRESS
#define CACHE_LINE_SIZE			32

struct pl310 {
	void		*base;
	u32		size;
	u32		way_mask;
};

static struct pl310 *__pl310;

static inline struct pl310 *__get_pl310(void)
{
	return __pl310;
}

static inline void cache_wait_way(void *reg)
{
	/* wait for cache operation by line or way to complete */
	while (readl_relaxed(reg) & __get_pl310()->way_mask)
		__asm__ __volatile__ ("" : : : "memory");
}

static inline void cache_sync(void)
{
	writel_relaxed(0, __get_pl310()->base + L2X0_CACHE_SYNC);
}

static void pl310_cache_sync(void)
{
	unsigned long flags;

	local_irq_save(flags);
	cache_sync();
	local_irq_restore(flags);
}

/* ------------------------------------------------------------------------------
 * line operations
 * ------------------------------------------------------------------------------
 */
static inline void pl310_op_line(unsigned long addr, u32 opreg)
{
	writel_relaxed(addr, __get_pl310()->base + opreg);
}

/* ------------------------------------------------------------------------------
 * operate the total cache
 * ------------------------------------------------------------------------------
 */
static void pl310_op_all(u32 opreg)
{
	struct pl310 *pl310 = __get_pl310();
	unsigned long flags;

	local_irq_save(flags);
	writel_relaxed(pl310->way_mask, pl310->base + opreg);
	cache_wait_way(pl310->base + opreg);
	cache_sync();
	local_irq_restore(flags);

}
static void pl310_inv_all(void)
{
	/* Invalidating when L2 is enabled is a nono */
	if (readl(__get_pl310()->base + L2X0_CTRL) & L2X0_CTRL_EN)
		return;

	pl310_op_all(L2X0_INV_WAY);
}

static inline void pl310_clean_all(void)
{
	pl310_op_all(L2X0_CLEAN_WAY);
}

static inline void pl310_flush_all(void)
{
	pl310_op_all(L2X0_CLEAN_INV_WAY);
}

/* ------------------------------------------------------------------------------
 * range operations
 * ------------------------------------------------------------------------------
 */
static void pl310_op_range(ulong start, ulong end, u32 opreg)
{
	unsigned long flags;

	local_irq_save(flags);
	while (start < end) {
		u32 blk_end = start + min(end - start, 4096UL);

		while (start < blk_end) {
			pl310_op_line(start, opreg);
			start += CACHE_LINE_SIZE;
		}

		if (blk_end < end) {
			local_irq_restore(flags);
			local_irq_save(flags);
		}
	}
	cache_sync();
	local_irq_restore(flags);

}

static void pl310_inv_range(unsigned long start, unsigned long end)
{
	unsigned long flags;

	local_irq_save(flags);
	if (start & (CACHE_LINE_SIZE - 1)) {
		start &= ~(CACHE_LINE_SIZE - 1);
		pl310_op_line(start, L2X0_INV_LINE_PA);
		start += CACHE_LINE_SIZE;
	}

	if (end & (CACHE_LINE_SIZE - 1)) {
		end &= ~(CACHE_LINE_SIZE - 1);
		pl310_op_line(end, L2X0_INV_LINE_PA);
	}
	local_irq_restore(flags);

	pl310_op_range(start, end, L2X0_INV_LINE_PA);
}

static void pl310_clean_range(unsigned long start, unsigned long end)
{
	if ((end - start) >= __get_pl310()->size) {
		pl310_clean_all();
		return;
	}

	start &= ~(CACHE_LINE_SIZE - 1);
	pl310_op_range(start, end, L2X0_CLEAN_LINE_PA);
}

static void pl310_flush_range(unsigned long start, unsigned long end)
{
	if ((end - start) >= __get_pl310()->size) {
		pl310_flush_all();
		return;
	}

	start &= ~(CACHE_LINE_SIZE - 1);
	pl310_op_range(start, end, L2X0_CLEAN_INV_LINE_PA);
}


static void pl310_unlock(struct pl310 *pl310)
{
	int lockregs = 2;
	u32 val = 0;
	int i;

	/*
	 * CPU0 I/D: WAY0 | WAY1
	 * CPU1 I/D: WAY2 | WAY3
	 */
	for (i = 0; i < lockregs; i++) {
		val = 0xf << (i * 4);
		writel_relaxed(val, pl310->base + L2X0_LOCKDOWN_WAY_D_BASE +
			       i * L2X0_LOCKDOWN_STRIDE);
		writel_relaxed(val, pl310->base + L2X0_LOCKDOWN_WAY_I_BASE +
			       i * L2X0_LOCKDOWN_STRIDE);
	}
}

int pl310_init(u32 aux_val, u32 aux_mask)
{
	u32 aux;
	u32 way_size;
	int ways;

	__pl310 = (struct pl310 *)calloc(1, sizeof(struct pl310));
	if (__pl310) {
		pr_err("%s: no mem!\n", __func__);
		return -ENOMEM;
	}

	__pl310->base = (void *)CONFIG_SYS_PL310_BASE;

	aux = readl_relaxed(__pl310->base + L2X0_AUX_CTRL);

	/* l2x0_of_init(0, ~0UL); */
	aux &= aux_mask;
	aux |= aux_val;

	ways = aux & (1 << 16) ? 16 : 8;
	__pl310->way_mask = (1 << ways) - 1;

	/* l2 cache size =  way size * number of ways */
	way_size = (aux & L2X0_AUX_CTRL_WAY_SIZE_MASK) >> 17;
	way_size = 1 << (way_size + L2X0_WAY_SIZE_SHIFT);

#define SZ_1K	0x400
	__pl310->size = ways * way_size * SZ_1K;


	if (!(readl_relaxed(__pl310->base + L2X0_CTRL) & L2X0_CTRL_EN)) {

		pl310_unlock(__pl310);

		/* Enable prefetching and early BRESP */
		writel_relaxed(aux | 0x7 << 28, __pl310->base + L2X0_AUX_CTRL);

		/* Setup tag and data ram latencies */
		writel_relaxed(0, __pl310->base + L2X0_TAG_LATENCY_CTRL);
		writel_relaxed(0x10, __pl310->base + L2X0_DATA_LATENCY_CTRL);

		pl310_inv_all();

		/* enable L2X0 */
		writel_relaxed(L2X0_CTRL_EN, __pl310->base + L2X0_CTRL);
	}

	outer_cache.inv_range = pl310_inv_range;
	outer_cache.clean_range = pl310_clean_range;
	outer_cache.flush_range = pl310_flush_range;
	outer_cache.sync = pl310_cache_sync;
	outer_cache.flush_all = pl310_flush_all;
	outer_cache.inv_all = pl310_inv_all;

	return 0;
}

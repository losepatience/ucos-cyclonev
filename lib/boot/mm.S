/* ~.~ *-S-*
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

#include <asm/linkage.h>
#include <asm/regs.h>
#include <config.h>
#include "mm.h"

	/* get the minimum dcache line size from the CTR on ARMv7 */
	.macro	dcache_line_size, reg, tmp
	mrc	p15, 0, \tmp, c0, c0, 1		@ read ctr
	lsr	\tmp, \tmp, #16
	and	\tmp, \tmp, #0xf		@ cache line size encoding
	mov	\reg, #4			@ bytes per word
	mov	\reg, \reg, lsl \tmp		@ actual cache line size
	.endm

/*
 * Invalidate the whole D-cache.
 * Corrupted registers:
 * 	r0-r5, r7, r9-r11
 */
ENTRY(armv7_inv_dcache)
	push	{r4-r5, r7, r9-r11}
	dmb				@ ensure ordering memory accesses
	mrc	p15, 1, r0, c0, c0, 1	@ read clidr
	ands	r3, r0, #0x7000000	@ extract loc from clidr
	mov	r3, r3, lsr #23		@ left align loc bit field
	beq	finished		@ if loc is 0, then no need to clean
	mov	r10, #0			@ start clean at cache level 0
flush_levels:
	add	r2, r10, r10, lsr #1	@ work out 3x current cache level
	mov	r1, r0, lsr r2		@ extract cache type bits from clidr
	and	r1, r1, #7		@ mask of the bits for current cache
	cmp	r1, #2			@ see what cache we have at this level
	blt	skip			@ skip if no cache, or just i-cache

	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	isb				@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0	@ read the new csidr

	and	r2, r1, #7		@ extract the length of the cache lines
	add	r2, r2, #4		@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3	@ find maximum number on the way size
	clz	r5, r4			@ find position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13	@ extract max number of the index size
loop1:
	mov	r9, r4			@ create working copy of max way size
loop2:
	orr	r11, r10, r9, lsl r5	@ factor way and cache number into r11
	orr	r11, r11, r7, lsl r2	@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2	@ invalidate by set/way

	subs	r9, r9, #1		@ decrement the way
	bge	loop2
	subs	r7, r7, #1		@ decrement the index
	bge	loop1
skip:
	add	r10, r10, #2		@ increment cache number
	cmp	r3, r10
	bgt	flush_levels
finished:
	mov	r10, #0			@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
	dsb
	isb
	pop	{r4-r5, r7, r9-r11}
	mov	pc, lr
ENDPROC(armv7_inv_dcache)


/*
 * __v7_inv_range(start, end)
 * invalidate the data cache within the specified region
 */
__v7_inv_range:
	dcache_line_size r2, r3
	sub	r3, r2, #1
	tst	r0, r3
	bic	r0, r0, r3
	mcrne	p15, 0, r0, c7, c14, 1	@ clean & invalidate D / U line

	tst	r1, r3
	bic	r1, r1, r3
	mcrne	p15, 0, r1, c7, c14, 1	@ clean & invalidate D / U line
1:
	mcr	p15, 0, r0, c7, c6, 1	@ invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(__v7_inv_range)

/*
 * __v7_clean_range(start, end)
 *  @start: virtual start address of region
 *  @end: virtual end address of region
 */
__v7_clean_range:
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c10, 1	@ clean D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(__v7_clean_range)

/*
 * __v7_flush_range(start, end)
 * clean&invalidate the data cache within the specified region
 */
__v7_flush_range:
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c14, 1	@ clean & invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
ENDPROC(__v7_flush_range)

/*
 * Set up page table: use 1M areas, VA = PA
 * The data cache is not active unless the mmu is enabled too
 */
ENTRY(__enable_cache)
	push	{lr}
	bl	__create_page_table	@ get page-table(r0 points to tlb)

	ldr	r0, =__mmu_table
	mcr	p15, 0, r0, c2, c0, 0	@ load page table pointer

	bl	armv7_inv_dcache	@ unessary to a PIPT dache

	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	@ I+BTB cache invalidate
	dsb
	mcr	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs

	ldr	r0, =0xffffffff		@ full access to all-supervisor
	mcr	p15, 0, r0, c3, c0, 0	@ load domain access register

	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #CR_M		@ mmu on
	bic	r0, r0, #CR_A		@ no strict alignment
	orr	r0, r0, #CR_C		@ dcache on
	orr	r0, r0, #CR_I		@ icache on

	b	__turn_mmu_on

	.align 5
__turn_mmu_on:
	mov	r0, r0
	isb
	mcr	p15, 0, r0, c1, c0, 0	@ write control reg
	mrc	p15, 0, r0, c0, c0, 0	@ read id reg
	isb
	mov	r0, r0
	mov	r0, r0

	pop	{lr}
	mov	pc, lr
ENDPROC(__enable_cache)

	.align
	.globl	maint_cache
	.type	maint_cache, %object
maint_cache:
	.long	__v7_inv_range
	.long	__v7_clean_range
	.long	__v7_flush_range

	.size	maint_cache, . - maint_cache

/*
 * __map_memory_region(u32 addr, u32 len, u32 flags)
 * map a memory region with VA = PA, r0 used as VA,
 * r1 as size(M) and r2 as flags.
 */
ENTRY(__map_memory_region)
	ldr	r3, =(1 << SECTION_SHIFT) - 1
	bic	r0, r0, r3
	orr	r0, r0, r2
	ldr	r2, =__mmu_table
	add	r2, r2, r0, lsr #SECTION_SHIFT - 2

1:	str	r0, [r2], #4
	add	r0, r0, #1 << SECTION_SHIFT
	cmp	r1, #0
	subne	r1, r1, #1
	bne	1b

	mov	pc, lr
ENDPROC(__map_memory_region)

/*
 * The data cache is not active unless the mmu is enabled too
 * TEX remap is disabled
 * Register corrupted: r0-r2, r0 to return page table base
 * WARN: __pgtable must be defined as an array in c and i do not know why.
 *
 * 31                    20 19 18 17 16 15     12   10  9  8     5  4  3  2  1 0
 * .-----------------------.--.--.--.--.--.------.----.--.--------.--.--.--.---.
 * | section base address  |NS| 0|nG| S| X| TEX  | AP | I| domain |XN| C| B| 10|
 * ^-----------------------^--^--^--^--^--^------^----^--^--------^--^--^--^---^
 *  x x x x x x x x x x x x  0  0  0  0  0  1 0 1  1 1  x  0 0 0 0  1  1  1  1 0
 *
 * TEX(101): L2 cache write-back, write-allocate
 */
__create_page_table:

	push	{lr}

 	/* Map the full 4GB memory space(VA = PA) */
	mov	r0, #0
	mov	r1, #4096
	ldr	r2, =PAGE1M_NOCACHE
	bl	__map_memory_region

	/* Map memory region for OS(including both OS) */
	ldr	r0, =__OS_BASE
	ldr	r1, =__OS_SIZE >> SECTION_SHIFT
	ldr	r2, =PAGE1M_NOMAL
	bl	__map_memory_region

 	/* Map SDRam used by both OS */
	ldr	r0, =__OS_BASE + __OS_SIZE
	ldr	r1, =PHYS_SDRAM_SIZE >> SECTION_SHIFT
	sub	r1, r0, lsr #SECTION_SHIFT
	ldr	r2, =PAGE1M_NOMAL | PAGE1M_SHARED
	bl	__map_memory_region

	pop	{lr}
	mov	pc, lr
ENDPROC(__create_page_table)


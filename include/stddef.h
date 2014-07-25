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

#ifndef __STD_DEF_H__
#define __STD_DEF_H__

#undef NULL
#if defined(__cplusplus)
#  define NULL 0
#else
#  define NULL ((void *)0)
#endif

typedef void (*callback_t)(void *);

#define offsetof(type, member) ((size_t) &((type *)0)->member)

#define container_of(ptr, type, member) ({				\
		const typeof(((type *)0)->member) * __mptr = (ptr);	\
		(type *)((char *)__mptr - offsetof(type, member)); })

#define roundup(x, y) (					\
{							\
	const typeof(y) __y = y;			\
	(((x) + (__y - 1)) / __y) * __y;		\
}							\
)

#define rounddown(x, y) (				\
{							\
	typeof(x) __x = (x);				\
	__x - (__x % (y));				\
}							\
)

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifdef CONFIG_64BIT
# define BITS_PER_LONG 64
#else
# define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))

#endif

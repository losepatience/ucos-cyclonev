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

#ifndef __STD_IO_H__
#define __STD_IO_H__

#include <vsprintf.h>
#include <stddef.h>

#ifndef EOF
# define EOF	(-1)
#endif

enum {
	stdin,
	stdout,
	stderr
};

struct stdio_dev {
	char name[32];

	void (*putc)(const char c);	/* To put a char */
	void (*puts)(const char *s);	/* To put a string (accelerator) */

	int (*tstc)(void);		/* To test if a char is ready... */
	int (*getc)(void);		/* To get that char */
};

int stdio_register(int fd, struct stdio_dev *dev);
int fputc(int fd, const char c);

inline int putchar(char c);
int puts(const char *s);

int printf(const char *fmt, ...);

/* print level */
#ifndef __PR_LEVEL__
#  define __PR_LEVEL__	__PR_INFO__
#endif

#define __PR_ERR__	3
#define __PR_WARN__	2
#define __PR_INFO__	1
#define __PR_DEBUG__	0

#if __PR_LEVEL__ <= __PR_DEBUG__
#  define pr_debug(fmt, args...) printf(fmt, ##args)
#  define debug(fmt, args...) printf(fmt, ##args)
#else
#  define pr_debug(fmt, args...) do {} while (0)
#  define debug(fmt, args...) do {} while (0)
#endif

#if __PR_LEVEL__ <= __PR_INFO__
#  define pr_info(fmt, args...) printf(fmt, ##args)
#else
#  define pr_info(fmt, args...) do {} while (0)
#endif

#if __PR_LEVEL__ <= __PR_WARN__
#  define pr_warn(fmt, args...) printf(fmt, ##args)
#else
#  define pr_warn(fmt, args...) do {} while (0)
#endif

#if __PR_LEVEL__ <= __PR_ERR__
#  define pr_err(fmt, args...) printf(fmt, ##args)
#else
#  define pr_err(fmt, args...) do {} while (0)
#endif

#endif

/* ~.~ *-c-*
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

/* this is a simple implement of printf, it does not support cache */

#include <stdio.h>
#include <vsprintf.h>
#include <platform.h>

static struct mutex  pr_lock;
static struct mutex  puts_lock;

static struct stdio_dev *stdio_dev[3];
static char printbuffer[1024];

int stdio_register(int fd, struct stdio_dev *dev)
{
	unsigned long flags = 0;

	/* allow to reassign stdio_dev to fd */
	local_irq_save(flags);
	stdio_dev[fd] = dev;
	local_irq_restore(flags);
	return 0;
}

int fputc(int fd, const char c)
{
	struct stdio_dev *dev;
	unsigned long flags = 0;

	local_irq_save(flags);
	dev = stdio_dev[fd];
	local_irq_restore(flags);

	if (!dev || !dev->putc)
		return EOF;

	if (c == '\n')
		dev->putc('\r');
	dev->putc(c);

	return c;
}

inline int putchar(const char c)
{
	return fputc(stdout, c);
}

int puts(const char *s)
{
	int cnt = 0;

	mutex_lock(&puts_lock);
	while (*s) {
		if (putchar(*s++) == EOF) {
			if (cnt == 0)
				cnt = EOF;
			break;
		}
		cnt++;
	}

	mutex_unlock(&puts_lock);
	return cnt;
}

int printf(const char *fmt, ...)
{
	int rval;
	va_list args;

	mutex_lock(&pr_lock);
	va_start(args, fmt);
	rval = vsprintf(printbuffer, fmt, args);
	va_end(args);

	puts(printbuffer);
	mutex_unlock(&pr_lock);
	return rval;
}

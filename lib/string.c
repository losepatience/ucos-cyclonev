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

#include <asm/types.h>

int memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1 = cs, *su2 = ct, *end = su1 + count;
	int res = 0;

	while (su1 < end) {
		res = *su1++ - *su2++;
		if (res)
			break;
	}
	return res;
}

void *memset(void *s, int c, size_t n)
{
	unsigned long *sl = (unsigned long *)s;
	unsigned long cl = 0;
	char *s8;
	int i;

	/* do it one word at a time (32 bits or 64 bits) while possible */
	if (((ulong)s & (sizeof(*sl) - 1)) == 0) {
		for (i = 0; i < sizeof(*sl); i++) {
			cl <<= 8;
			cl |= c & 0xff;
		}
		while (n >= sizeof(*sl)) {
			*sl++ = cl;
			n -= sizeof(*sl);
		}
	}
	/* fill 8 bits at a time */
	s8 = (char *)sl;
	while (n--)
		*s8++ = c;

	return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned long *dl = (unsigned long *)dest, *sl = (unsigned long *)src;
	char *d8, *s8;

	if (src == dest)
		return dest;

	/* while all data is aligned (common case), copy a word at a time */
	if ((((ulong)dest | (ulong)src) & (sizeof(*dl) - 1)) == 0) {
		while (n >= sizeof(*dl)) {
			*dl++ = *sl++;
			n -= sizeof(*dl);
		}
	}
	/* copy the reset one byte at a time */
	d8 = (char *)dl;
	s8 = (char *)sl;
	while (n--)
		*d8++ = *s8++;

	return dest;
}


void *memmove(void *dest, const void *src, size_t n)
{
	char *d, *s;

	if (src == dest)
		return dest;

	if (dest <= src) {
		d = (char *)dest;
		s = (char *)src;
		while (n--)
			*d++ = *s++;
	} else {
		d = (char *)dest + n;
		s = (char *)src + n;
		while (n--)
			*--d = *--s;
	}

	return dest;
}

size_t strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

size_t strnlen(const char *s, size_t maxlen)
{
	const char *sc;

	for (sc = s; maxlen-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

char *strcpy(char *dest, const char *src)
{
	char *p = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;

	return p;
}


char *strncpy(char *dest, const char *src, size_t n)
{
	char *p = dest;

	while (n-- && (*dest++ = *src++) != '\0')
		/* nothing */;

	return p;
}


int strcmp(const char *s1, const char *s2)
{
	register signed char __res;

	while (1) {
		if ((__res = *s1 - *s2++) != 0 || !*s1++)
			break;
	}

	return __res;
}


int strncmp(const char *s1, const char *s2, size_t n)
{
	register signed char __res = 0;

	while (n) {
		if ((__res = *s1 - *s2++) != 0 || !*s1++)
			break;
		n--;
	}

	return __res;
}


char *strcat(char *dest, const char *src)
{
	char *tmp = dest;

	while (*dest)
		dest++;

	while ((*dest++ = *src++) != '\0')
		;

	return tmp;
}


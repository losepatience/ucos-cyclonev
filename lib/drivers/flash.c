/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon May  5 11:04:58 CST 2014
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

/*
 * only envs would be writen to buffer firstly, others would be
 * directly writen to the flash. there are 2 opportunities to
 * flush the buffer:
 *  1. at the arrival of the timer.
 *  2. when sync is manually called.
 */
#include <asm/types.h>
#include <platform.h>
#include <spi_flash.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>

struct flash {
	struct spi_flash	*chip;
	char			*wrbuf;
	u32			sector_size;	/* Erase (sector) size */
	spinlock_t		lock;
};

static struct flash __flash;

static int read_sector(struct flash *flash, char *des, ulong addr)
{
	return flash->chip->read(flash->chip, addr, flash->sector_size, des);
}

static int write_sector(struct flash *flash, char *src, ulong addr)
{
	int rval = flash->chip->erase(flash->chip, addr, flash->sector_size);
	if (rval < 0)
		return rval;

	return flash->chip->write(flash->chip, addr, flash->sector_size, src);
}

static int __write(char *src, ulong addr, ulong cnt)
{
	struct flash *flash = &__flash;
	u32 padding = addr - rounddown(addr, flash->sector_size);
	int rval;

	if (cnt > flash->sector_size - padding)
		cnt = flash->sector_size - padding;

	if (padding || cnt < flash->sector_size) {

		addr -= padding;

		rval = read_sector(flash, flash->wrbuf, addr);
		if (rval < 0)
			return rval;

		memcpy(flash->wrbuf + padding, src, cnt);
		src = flash->wrbuf;
	}

	rval = write_sector(flash, src, addr);

	return rval < 0 ? rval : cnt;
}

int flash_write(char *src, ulong addr, ulong cnt)
{
	struct flash *flash = &__flash;
	unsigned long flags = 0;
	int rval = 0;

	spin_lock_irqsave(&flash->lock, flags);
	while (cnt) {
		int size = __write(src, addr, cnt);
		if (size <= 0) {
			if (rval == 0)
				rval = size;
			break;
		}

		src += size;
		addr += size;
		cnt -= size;
		rval += size;
	}
	spin_unlock_irqrestore(&flash->lock, flags);

	return rval;
}

int flash_read(char *des, ulong addr, ulong cnt)
{
	struct flash *flash = &__flash;
	unsigned long flags = 0;
	int rval;

	spin_lock_irqsave(&flash->lock, flags);
	rval = flash->chip->read(flash->chip, addr, cnt, des);
	if (rval < 0)
		cnt = rval;
	spin_unlock_irqrestore(&flash->lock, flags);
	return cnt;
}

int flash_init(void)
{
	struct flash *flash = &__flash;

	flash->chip = spi_flash_probe(0, 0, 50000000, SPI_CPOL | SPI_CPHA);
	if (flash->chip == NULL)
		return -ENODEV;

	flash->wrbuf = (char *)calloc(1, flash->sector_size);
	if (flash->wrbuf == NULL) {
		spi_flash_free(flash->chip);
		return -ENOMEM;
	}

	flash->sector_size = flash->chip->sector_size;

	return 0;
}

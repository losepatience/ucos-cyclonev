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

#include <asm/types.h>
#include <platform.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <flash.h>

struct flash {
#ifndef CONFIG_NAND_FLASH
	struct spi_flash	*chip;
#else
	struct nand_flash	*chip;
#endif
	u32			sector_size;	/* Erase (sector) size */
	char			*wrbuf;
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
	unsigned long flags = 0;
	int rval = 0;

	local_irq_save(flags);
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
	local_irq_restore(flags);

	return rval;
}

int flash_read(char *des, ulong addr, ulong cnt)
{
	struct flash *flash = &__flash;
	unsigned long flags = 0;
	int rval;

	local_irq_save(flags);
	rval = flash->chip->read(flash->chip, addr, cnt, des);
	if (rval < 0)
		cnt = rval;
	local_irq_restore(flags);
	return cnt;
}

int flash_init(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)

{
	struct flash *flash = &__flash;

#ifndef CONFIG_NAND_FLASH
	flash->chip = spi_flash_probe(bus, cs, max_hz, mode);
#else
	flash->chip = nand_flash_probe(bus, cs, max_hz, mode);
#endif

	if (flash->chip == NULL)
		return -ENODEV;

	flash->wrbuf = (char *)calloc(1, flash->sector_size);
	if (flash->wrbuf == NULL) {
#ifndef CONFIG_NAND_FLASH
		spi_flash_free(flash->chip);
#else
		nand_flash_free(flash->chip);
#endif
		return -ENOMEM;
	}

	flash->sector_size = flash->chip->sector_size;

	return 0;
}

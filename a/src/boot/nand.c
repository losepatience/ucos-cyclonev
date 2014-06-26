/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 25 22:14:49 CST 2014
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

#include <asm/arch/regs.h>
#include <asm/io.h>
#include "nand.h"
#include "gpmc.h"
#include "mem.h"
#include "delay.h"

#define NAND_DEVICE_WIDTH	8	/* FIXME */

/* -----------------------------------------------------------------------------
 * nand common function
 * -----------------------------------------------------------------------------
 */
static int nand_wait_ready(struct nand_chip *chip)
{
	int retries = 100;

	do {
		if (chip->dev_ready(chip))
			return 1;
		udelay(0x500);

	} while (retries--);

	return 0;
}

static void nand_cmd_ctrl(struct nand_chip *chip, int dat, u32 ctrl)
{
	if (NAND_CMD_NONE == dat)
		return;

	switch (ctrl) {
	case NAND_CTRL_CLE:
		writeb(dat, chip->cmd_port);
		break;

	case NAND_CTRL_ALE:
		writeb(dat, chip->adr_port);
		break;

	case NAND_NCE:
		writeb(dat, chip->dat_port);
		break;

	default:
		break;
	}
}

static void nand_command(struct nand_chip *chip, u32 cmd, int col, int page)
{
	int ctrl = NAND_CTRL_CLE;

	/* write out the command to the device */
	if (cmd == NAND_CMD_SEQIN) {
		int readcmd;

		if (col >= chip->writesize) {
			/* OOB area */
			col -= chip->writesize;
			readcmd = NAND_CMD_READOOB;
		} else if (col < 256) {
			/* First 256 bytes --> READ0 */
			readcmd = NAND_CMD_READ0;
		} else {
			col -= 256;
			readcmd = NAND_CMD_READ1;
		}
		nand_cmd_ctrl(chip, readcmd, ctrl);
	}

	nand_cmd_ctrl(chip, cmd, ctrl);

	/* Serially input address */
	ctrl = NAND_CTRL_ALE;

	if (col != -1) {
		/* Adjust columns for 16 bit buswidth */
		if (chip->options & NAND_BUSWIDTH_16)
			col >>= 1;
		nand_cmd_ctrl(chip, col & 0xff, ctrl);
	}

	if (page != -1) {
		nand_cmd_ctrl(chip, page & 0xff , ctrl);
		nand_cmd_ctrl(chip, (page >> 8) & 0xff, ctrl);

		/* One more address cycle for devices > 32MiB */
		if (chip->chipsize > (32 << 20))
			nand_cmd_ctrl(chip, (page >> 16) & 0xff, ctrl);
	}

	if (cmd == NAND_CMD_PAGEPROG
			|| cmd == NAND_CMD_ERASE1
			|| cmd == NAND_CMD_ERASE2
			|| cmd == NAND_CMD_SEQIN
			|| cmd == NAND_CMD_STATUS)
		return;

	udelay(100);
	nand_wait_ready(chip);
}

static void nand_command_lp(struct nand_chip *chip, u32 cmd, int col, int page)
{
	int ctrl = NAND_CTRL_CLE;

	/* emulate NAND_CMD_READOOB */
	if (cmd == NAND_CMD_READOOB) {
		col += chip->writesize;
		cmd = NAND_CMD_READ0;
	}

	nand_cmd_ctrl(chip, cmd, ctrl);

	/* Serially input address */
	ctrl = NAND_CTRL_ALE;

	if (col != -1) {
		/* Adjust columns for 16 bit buswidth */
		if (chip->options & NAND_BUSWIDTH_16)
			col >>= 1;
		nand_cmd_ctrl(chip, col & 0xff, ctrl);
		nand_cmd_ctrl(chip, (col >> 8) & 0xff, ctrl);
	}

	if (page != -1) {
		nand_cmd_ctrl(chip, page & 0xff , ctrl);
		nand_cmd_ctrl(chip, (page >> 8) & 0xff, ctrl);

		/* One more address cycle for devices > 32MiB */
		if (chip->chipsize > (32 << 20))
			nand_cmd_ctrl(chip, (page >> 16) & 0xff, ctrl);
	}

	if (cmd == NAND_CMD_PAGEPROG
		|| cmd == NAND_CMD_ERASE1
		|| cmd == NAND_CMD_ERASE2
		|| cmd == NAND_CMD_SEQIN
		|| cmd == NAND_CMD_STATUS
		|| cmd == NAND_CMD_CACHEDPROG
		|| cmd == NAND_CMD_RNDIN
		|| cmd == NAND_CMD_DEPLETE1)
		return;

	if (cmd >= NAND_CMD_STATUS_ERROR && cmd <= NAND_CMD_STATUS_ERROR3) {
		/* Read error status commands require
		 * only a short delay
		 */
		udelay(chip->chip_delay);
		return;
	}

	if (cmd == NAND_CMD_RNDOUT) {
		/* No ready / busy check necessary */
		nand_cmd_ctrl(chip, NAND_CMD_RNDOUTSTART, ctrl);
		return;
	}

	if (cmd == NAND_CMD_READ0)
		nand_cmd_ctrl(chip, NAND_CMD_READSTART, ctrl);

	udelay(100);
	nand_wait_ready(chip);

}

/* read 8bit buswidth nands */
static void nand_read_buf(struct nand_chip *chip, u8 *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
		buf[i] = readb(chip->dat_port);
}

/* read 16bit buswidth nands */
static void nand_read_buf16(struct nand_chip *chip, u8 *buf, int len)
{
	int i;
	u16 *p = (u16 *)buf;
	len >>= 1;

	for (i = 0; i < len; i++)
		p[i] = readw(chip->dat_port);
}

int nand_block_isbad(struct nand_chip *chip, int ofs)
{
	return chip->block_bad(chip, ofs);
}

static int nand_read_page(struct nand_chip *chip,
			int block, int page, void *dst)
{
	/* FIXME */
	int page_addr = page + block * (chip->erasesize / chip->writesize);
	//int page_addr = page + block * CONFIG_SYS_NAND_PAGE_COUNT;

	chip->cmdfunc(chip, NAND_CMD_READ0, 0, page_addr);
	chip->read_buf(chip, dst, chip->writesize);
	return 0;
}

int nand_load(struct nand_chip *chip, u32 ofs, u32 size, void *dst)
{
	unsigned int block, lastblock;
	unsigned int page;

	/* offs has to be aligned to a page address!  */
	block = ofs / chip->erasesize;
	lastblock = (ofs + size - 1) / chip->erasesize;
	page = (ofs % chip->erasesize) / chip->writesize;

	while (block <= lastblock) {
		if (!nand_block_isbad(chip, ofs)) {
			int cnt = chip->erasesize / chip->writesize;

			/* Skip bad blocks */
			while (page < cnt) {
				nand_read_page(chip, block, page, dst);
				dst += chip->writesize;
				page++;
			}

			page = 0;
		} else {
			lastblock++;
		}

		block++;
		ofs += chip->erasesize;
	}

	return 0;
}


void nand_init(struct nand_chip *chip)
{
	int i;
	u8 devid;
	const struct nand_flash_desc *type = (void *)0;

	/* reset the chip, required by some chips */
	chip->cmdfunc(chip, NAND_CMD_RESET, -1, -1);

	/* read vendor ID and device ID */
	chip->cmdfunc(chip, NAND_CMD_READID, 0x00, -1);

	/* read out vendor ID */
	readb(chip->dat_port);

	devid = readb(chip->dat_port);

	/* lookup the flash ID */
	for (i = 0; nand_flash_ids[i].name != (void *)0; i++) {
		if (devid == nand_flash_ids[i].id) {
			type =  &nand_flash_ids[i];
			break;
		}
	}

	if (!type)
		return;

	chip->chipsize = (u64)type->chipsize << 20;

	if (!type->pagesize) {
		u8 extid;

		/* the 3rd id byte holds MLC / multichip data */
		readb(chip->dat_port);

		/* The 4th id byte is the important one:
		 *   PS-PageSize(0-1): 1K << n
		 *   BS-BlockRize(4-5): 64K << n
		 *   OS-OobSize(2, 8x/16x): (PS / 512) * 8 << n
		 *   OG-x8/x16(6)
		 * .-----.-----.-----.-----.-----.-----.-----.-----.
		 * |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
		 * .-----.-----+-----+-----+-----.-----+-----+-----.
		 * |    PS     | OS  |  X  |    BS     |  OG |  X  |
		 * ^-----------^-----^-----^-----------^-----^-----^
		 */
		extid = readb(chip->dat_port);

		chip->writesize = 1024 << (extid & 0x3);
		extid >>= 4;
		chip->erasesize = (64 * 1024) << (extid & 0x03);
	} else {
		chip->erasesize = type->erasesize;
		chip->writesize = type->pagesize;
	}

	/* Get chip options, preserve non chip based options */
	chip->options |= type->options;

	/* Set the bad block position */
	if (chip->writesize > 512 || (chip->options & NAND_BUSWIDTH_16))
		chip->badblockpos = NAND_LARGE_BADBLOCK_POS;
	else
		chip->badblockpos = NAND_SMALL_BADBLOCK_POS;

	if (type->options & NAND_BUSWIDTH_16)
		chip->read_buf = nand_read_buf16;

	/* Do not replace user supplied command function! */
	if (chip->writesize > 512 && chip->cmdfunc == nand_command)
		chip->cmdfunc = nand_command_lp;
}


/* -----------------------------------------------------------------------------
 * omap nand
 * -----------------------------------------------------------------------------
 */
static inline int omap_dev_ready(struct nand_chip *chip)
{
	return readl((void *)GPMC_BASE + GPMC_STATUS) & (1 << 8);
}

static int omap_nand_block_bad(struct nand_chip *chip, int ofs)
{
	/* FIXME */
	int page = ofs / chip->writesize;

	chip->cmdfunc(chip, NAND_CMD_READOOB,
			chip->badblockpos & 0xFE, page);

	if (chip->options & NAND_BUSWIDTH_16) {
		if (readw(chip->dat_port) != 0xffff)
			return 1;
	} else {
		if (readb(chip->dat_port) != 0xff)
			return 1;
	}

	return 0;
}

int omap_nand_init(struct nand_chip *chip)
{
	int cfg = 0;
	int cs;

	for (cs = 0; cs < GPMC_MAX_CS; cs++)
		if ((readl(gpmc_cs(cs)) & 0xC00) == 0x800)
			break;

	if (cs >= GPMC_MAX_CS)
		return -1;

	cfg = readl(gpmc_reg(GPMC_CONFIG));
	cfg |= 0x10;	/* disable write protect */
	writel(cfg, gpmc_reg(GPMC_CONFIG));

	chip->cmd_port	= gpmc_cs(cs) + GPMC_CS_NAND_CMD,
	chip->adr_port	= gpmc_cs(cs) + GPMC_CS_NAND_ADR,
	chip->dat_port	= gpmc_cs(cs) + GPMC_CS_NAND_DAT,
	chip->cmd_ctrl	= nand_cmd_ctrl;
	chip->options	|= NAND_NO_PADDING | NAND_CACHEPRG;

	chip->chip_delay = 100;

	/* configure driver and controller based on NAND device bus-width */
	cfg = readl(gpmc_cs(cs) + GPMC_CS_CONFIG1);
	if (NAND_DEVICE_WIDTH == 16) {
		chip->options |= NAND_BUSWIDTH_16;
		cfg |= 0x1 << 12;
	} else {
		chip->options &= ~NAND_BUSWIDTH_16;
		cfg &= ~(0x1 << 12);
	}
	writel(cfg, gpmc_cs(cs) + GPMC_CS_CONFIG1);

	if (chip->options & NAND_BUSWIDTH_16)
		chip->read_buf = nand_read_buf16;
	else
		chip->read_buf = nand_read_buf;

	chip->dev_ready = omap_dev_ready;
	chip->block_bad = omap_nand_block_bad;

	nand_init(chip);

	return 0;
}


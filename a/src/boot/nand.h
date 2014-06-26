/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jun 26 10:14:47 CST 2014
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

#ifndef __NAND_H__
#define __NAND_H__

#include <asm/types.h>

/*
 * Standard NAND flash commands */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_LOCK_TIGHT	0x2c
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24
#define NAND_CMD_LOCK_STATUS	0x7a

/* extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff

#define NAND_CMD_NONE		-1

/*
 * Status bits */
#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

/*
 * Constants for Hardware ECC */
/* Reset Hardware ECC for read */
#define NAND_ECC_READ		0

/* Reset Hardware ECC for write */
#define NAND_ECC_WRITE		1

/* Enable Hardware ECC before syndrome is read back from flash */
#define NAND_ECC_READSYN	2

/* Bit mask for flags passed to do_nand_read_ecc */
#define NAND_GET_DEVICE		0x80


/*
 * Option constants for bizarre disfunctionality and real
 * features. */
/* Buswidth is 16 bit */
#define NAND_BUSWIDTH_16	0x00000002

/* Device supports partial programming without padding */
#define NAND_NO_PADDING		0x00000004

/* Chip has cache program function */
#define NAND_CACHEPRG		0x00000008

/* Chip has copy back function */
#define NAND_COPYBACK		0x00000010

/*
 * AND Chip which has 4 banks and a confusing page / block
 * assignment. See Renesas datasheet for further information.
 */
#define NAND_IS_AND		0x00000020

/*
 * Chip has a array of 4 pages which can be read without
 * additional ready /busy waits.
 */
#define NAND_4PAGE_ARRAY	0x00000040

/*
 * Chip requires that BBT is periodically rewritten to prevent
 * bits from adjacent blocks from 'leaking' in altering data.
 * This happens with the Renesas AG-AND chips, possibly others.
 */
#define BBT_AUTO_REFRESH	0x00000080

/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	0x00000200

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		0x00000800

/* Device supports subpage reads */
#define NAND_SUBPAGE_READ       0x00001000

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS \
	(NAND_NO_PADDING | NAND_CACHEPRG | NAND_COPYBACK)
#define LP_OPTIONS		NAND_SAMSUNG_LP_OPTIONS
#define LP_OPTIONS16		(LP_OPTIONS | NAND_BUSWIDTH_16)

/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01

/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02

/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)


/* Constants for oob configuration */
#define NAND_SMALL_BADBLOCK_POS		5
#define NAND_LARGE_BADBLOCK_POS		0

struct nand_flash_desc {
	char		*name;
	int		id;
	unsigned long	pagesize;
	unsigned long	chipsize;
	unsigned long	erasesize;
	unsigned long	options;
};

extern const struct nand_flash_desc nand_flash_ids[];

/* for now, not support write */
struct nand_chip {

	void *cmd_port;
	void *adr_port;
	void *dat_port;

	unsigned long erasesize;
	unsigned long writesize;
	unsigned long chipsize;

	void (*read_buf)(struct nand_chip *chip, u8 *buf, int len);
	void (*select_chip)(struct nand_chip *chip, int nr);

	int  (*block_bad)(struct nand_chip *chip, int ofs);
	void (*cmd_ctrl)(struct nand_chip *chip, int dat, u32 ctrl);
	int (*dev_ready)(struct nand_chip *chip);
	void (*cmdfunc)(struct nand_chip *chip, u32 cmd, int col, int page);

	int chip_delay;
	unsigned int options;
	int badblockpos;
};


#endif

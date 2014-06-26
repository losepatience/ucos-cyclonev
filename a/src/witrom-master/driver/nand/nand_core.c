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
#include <flash/nand.h>

static void *base_address = (void *)GPMC_BASE + GPMC_CS0;

static int omap3_nand_ready(struct nand_chip *chip)
{
	void *gpmc = (void *)GPMC_BASE;

	return readl(gpmc + GPMC_STATUS) & (1 << 8);
}

static void omap3_cmd_ctrl(struct nand_chip *chip, u32 cmd, u32 ctrl)
{
	switch (ctrl) {

	case NAND_CTRL_CLE:
		writeb(cmd, chip->cmd_port);
		break;

	case NAND_CTRL_ALE:
		writeb(cmd, chip->adr_port);
		break;

	case NAND_NCE:
		writeb(cmd, chip->dat_port);
		break;
	}

	writeb(cmd, chip->IO_ADDR_W);
}



#define is_large_page(nand)	((nand)->write_size >= 1024)

static const struct nand_desc chip_desc[] = {
	{ "16MB 1.8V 08-bit",	0x33, 512, 16, 0x4000, 0 },
	{ "16MB 3.3V 08-bit",	0x73, 512, 16, 0x4000, 0 },
	{ "16MB 1.8V 16-bit",	0x43, 512, 16, 0x4000, NAND_BUSWIDTH_16 },
	{ "16MB 3.3V 16-bit",	0x53, 512, 16, 0x4000, NAND_BUSWIDTH_16 },

	{ "32MB 1.8V 08-bit",	0x35, 512, 32, 0x4000, 0 },
	{ "32MB 3.3V 08-bit",	0x75, 512, 32, 0x4000, 0 },
	{ "32MB 1.8V 16-bit",	0x45, 512, 32, 0x4000, NAND_BUSWIDTH_16 },
	{ "32MB 3.3V 16-bit",	0x55, 512, 32, 0x4000, NAND_BUSWIDTH_16 },

	{ "64MB 1.8V 08-bit", 	0x36, 512, 64, 0x4000, 0 },
	{ "64MB 3.3V 08-bit", 	0x76, 512, 64, 0x4000, 0 },
	{ "64MB 1.8V 16-bit",	0x46, 512, 64, 0x4000, NAND_BUSWIDTH_16 },
	{ "64MB 3.3V 16-bit",	0x56, 512, 64, 0x4000, NAND_BUSWIDTH_16 },

	{ "128MB 1.8V 08-bit",	0x78, 512, 128, 0x4000, 0 },
	{ "128MB 1.8V 08-bit",	0x39, 512, 128, 0x4000, 0 },
	{ "128MB 3.3V 08-bit",	0x79, 512, 128, 0x4000, 0 },
	{ "128MB 1.8V 16-bit",	0x72, 512, 128, 0x4000, NAND_BUSWIDTH_16 },
	{ "128MB 1.8V 16-bit",	0x49, 512, 128, 0x4000, NAND_BUSWIDTH_16 },
	{ "128MB 3.3V 16-bit",	0x74, 512, 128, 0x4000, NAND_BUSWIDTH_16 },
	{ "128MB 3.3V 16-bit",	0x59, 512, 128, 0x4000, NAND_BUSWIDTH_16 },

	{ "256MB 3.3V 08-bit",	0x71, 512, 256, 0x4000, 0 },

	{ "64MB 1.8V 08-bit",	0xA2, 0,  64, 0, LP_OPTIONS },
	{ "64MB 3.3V 08-bit",	0xF2, 0,  64, 0, LP_OPTIONS },
	{ "64MB 1.8V 16-bit",	0xB2, 0,  64, 0, LP_OPTIONS16 },
	{ "64MB 3.3V 16-bit",	0xC2, 0,  64, 0, LP_OPTIONS16 },

	{ "128MB 1.8V 08-bit",	0xA1, 0, 128, 0, LP_OPTIONS },
	{ "128MB 3.3V 08-bit",	0xF1, 0, 128, 0, LP_OPTIONS },
	{ "128MB 1.8V 16-bit",	0xB1, 0, 128, 0, LP_OPTIONS16 },
	{ "128MB 3.3V 16-bit",	0xC1, 0, 128, 0, LP_OPTIONS16 },

	{ "256MB 1.8V 08-bit",	0xAA, 0, 256, 0, LP_OPTIONS },
	{ "256MB 3.3V 08-bit",	0xDA, 0, 256, 0, LP_OPTIONS },
	{ "256MB 1.8V 16-bit",	0xBA, 0, 256, 0, LP_OPTIONS16 },
	{ "256MB 3.3V 16-bit",	0xCA, 0, 256, 0, LP_OPTIONS16 },

	{ "512MB 1.8V 08-bit",	0xAC, 0, 512, 0, LP_OPTIONS },
	{ "512MB 3.3V 08-bit",	0xDC, 0, 512, 0, LP_OPTIONS },
	{ "512MB 1.8V 16-bit",	0xBC, 0, 512, 0, LP_OPTIONS16 },
	{ "512MB 3.3V 16-bit",	0xCC, 0, 512, 0, LP_OPTIONS16 },

	{ "1GiB 1.8V 08-bit",	0xA3, 0, 1024, 0, LP_OPTIONS },
	{ "1GiB 3.3V 08-bit",	0xD3, 0, 1024, 0, LP_OPTIONS },
	{ "1GiB 1.8V 16-bit",	0xB3, 0, 1024, 0, LP_OPTIONS16 },
	{ "1GiB 3.3V 16-bit",	0xC3, 0, 1024, 0, LP_OPTIONS16 },

	{ "2GiB 1.8V 08-bit",	0xA5, 0, 2048, 0, LP_OPTIONS },
	{ "2GiB 3.3V 08-bit",	0xD5, 0, 2048, 0, LP_OPTIONS },
	{ "2GiB 1.8V 16-bit",	0xB5, 0, 2048, 0, LP_OPTIONS16 },
	{ "2GiB 3.3V 16-bit",	0xC5, 0, 2048, 0, LP_OPTIONS16 },
	{ 0 };
};

static int nand_wait_ready(struct nand_chip *chip)
{
	int ready;
	int retries = 100;

	do {
		ready = chip->nand_ready(chip);
		if (ready)
			return 1;

		udelay(0x500);

	} while (retries--);

	return 0;
}

static void config_nand(struct nand_chip *chip, const struct nand_desc *desc)
{
	u8 ext_id;

	chip->chip_size  = desc->size & 0xFFF00000;
	chip->write_size = desc->size & 0x000FFFFF;

	if (!chip->write_size) {
		ext_id = readb(chip->dat_port);
		ext_id = readb(chip->dat_port);

		chip->write_size = KB(1) << (ext_id & 0x3);
	}

	if (!chip->read_buf) {
		if (desc->flags & NAND_BUSWIDTH_16)
			chip->read_buf = nand_read_buf16;
		else
			chip->read_buf = nand_read_buf8;
	}
}

int nand_probe(struct nand_chip *nand)
{
	int i;
	u8 dev_id, ven_id;

	nand_command(nand, NAND_CMMD_RESET, -1, -1);
	nand_command(nand, NAND_CMMD_READID, -1, 0);

	ven_id = readb(nand->data_port);
	dev_id = readb(nand->data_port);

	for (i = 0; i < ARRAY_ELEM_NUM(g_nand_chip_desc); i++) {
		if (g_nand_chip_desc[i].id == dev_id) {
			config_nand(nand, &g_nand_chip_desc[i]);
			return 0;
		}
	}

	printf("NAND not found!\n");
	return -1;
}


/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01

/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02

/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)

static void nand_cmd(struct nand_chip *chip, u32 cmd, int row, int col)
{
	chip->cmd_ctrl(chip, cmd, NAND_CTRL_CTRL_CLE);

	if (col != -1) {
		chip->cmd_ctrl(chip, col & 0xff, NAND_CTRL_ALE);

		if (is_large_page(chip))
			chip->cmd_ctrl(chip, (col >> 8) & 0xff, NAND_CTRL_ALE);
	}

	if (row != -1) {
		chip->cmd_ctrl(chip, row & 0xff, NAND_CTRL_ALE);
		chip->cmd_ctrl(chip, (row >> 8) & 0xff, NAND_CTRL_ALE);

		if (chip->chip_size > (1 << 27))
			chip->cmd_ctrl(chip, (row >> 16) & 0xff, NAND_CTRL_ALE);
	}

	switch (cmd) {
	case NAND_CMD_READ0:
		if (is_large_page(chip))
			chip->cmd_ctrl(chip, NAND_CMD_READSTART, NAND_CTRL_CLE);

	default:
		break;
	}

	nand_wait_ready(chip); // fixme: not need for READID
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
	u16 *p = (u16 *) buf;
	len >>= 1;

	for (i = 0; i < len; i++)
		p[i] = readw(chip->dat_port);
}

void *nand_read_page(struct nand_chip *chip, int page, void *buf)
{
	nand_command(chip, NAND_CMD_READ0, page, 0);
	return nand->read_buf(chip, buf, nand->write_size);
}

void nand_init(void)
{
	void *base_addr = base_address;	
	u8 dev_id, ven_id;

	chip->dat_port = base_addr + GPMC_CS_NAND_DAT;

	/* reset the chip */
	nand_cmd(chip, NAND_CMD_RESET, -1, -1);

	/* read vendor ID and device ID */
	nand_cmd(chip, NAND_CMD_READID, -1, 0);
	ven_id = readb(nand->data_port);
	dev_id = readb(nand->data_port);

	for (i = 0; i < ARRAY_ELEM_NUM(g_nand_chip_desc); i++) {
		if (g_nand_chip_desc[i].id == dev_id) {
			config_nand(nand, &g_nand_chip_desc[i]);
			return 0;
		}
	}
}

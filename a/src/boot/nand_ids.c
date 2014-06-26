/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Thu Jun 26 12:00:02 CST 2014
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

#include "nand.h"

const struct nand_flash_desc nand_flash_ids[] = {
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
	{ (void *)0 },
};

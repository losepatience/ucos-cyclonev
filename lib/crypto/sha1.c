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

#include <crypto.h>

/* --------------------------------------------------------------------- */
/* --------------------------- SHA1 ------------------------------------ */
/* --------------------------------------------------------------------- */
void __sha1(u32 sha1[5], const u8 data[64])
{
	int i;
	u32 tmp;
	u32 mt[80];
	u32 ktn[4] = {0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6};

	for (i = 0; i < 16; i++) {
		mt[i] = ((long)data[i * 4] & 0xff) << 24
			| ((long)data[i * 4 + 1] & 0xff) << 16
			| ((long)data[i * 4 + 2] & 0xff) << 8
			| ((long)data[i * 4 + 3] & 0xff);
	}

	for (i = 16; i < 80; i++) {
		tmp = mt[i - 3] ^ mt[i - 8] ^ mt[i - 14] ^ mt[i - 16];
		mt[i] = (tmp << 1 & 0xfffffffe) | (tmp >> 31 & 0x1);
	}

	sha1[0] = 0x67452301;
	sha1[1] = 0xefcdab89;
	sha1[2] = 0x98badcfe;
	sha1[3] = 0x10325476;
	sha1[4] = 0xc3d2e1f0;

	for (i = 0; i < 80; i++) {
		tmp = (sha1[0] << 5 & 0xffffffe0) | (sha1[0] >> 27 & 0x1f);

		if (i < 20)
			tmp += (sha1[1] & sha1[2]) | (~sha1[1] & sha1[3]);
		else if (i < 40)
			tmp += sha1[1] ^ sha1[2] ^ sha1[3];
		else if (i < 60)
			tmp += (sha1[1] & sha1[2])
				| (sha1[1] & sha1[3]) | (sha1[2] & sha1[3]);
		else
			tmp += sha1[1] ^ sha1[2] ^ sha1[3];

		tmp += sha1[4] + ktn[i / 20] + mt[i];
		sha1[4] = sha1[3];
		sha1[3] = sha1[2];
		sha1[2] = (sha1[1] << 30 & 0xc0000000)
			| (sha1[1] >> 2 & 0x3fffffff);
		sha1[1] = sha1[0];
		sha1[0] = tmp;
	}
}


/*
 * iButtons use LSB first, so we have to turn the result around.
 * a-b-c-d-e should be turn to e-d-c-b-a, where each letter
 * represents four bytes ofthe result.
 */
void w1_sha1(u8 *mac, const u8 data[64])
{
	int i;
	u32 tmp;
	u32 sha1[5];

	__sha1(sha1, data);

	for (i = 0; i < 5; i++) {
		tmp = sha1[4 - i];
		mac[i * 4 + 0] = (u8)tmp;
		tmp >>= 8;
		mac[i * 4 + 1] = (u8)tmp;
		tmp >>= 8;
		mac[i * 4 + 2] = (u8)tmp;
		tmp >>= 8;
		mac[i * 4 + 3] = (u8)tmp;
	}
}

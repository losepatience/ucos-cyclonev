/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Wed Jun 25 10:06:57 CST 2014
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
#include "clock.h"

/* MPU max frequencies */
#define AM335X_ZCZ_300			0x1FEF
#define AM335X_ZCZ_600			0x1FAF
#define AM335X_ZCZ_720			0x1F2F
#define AM335X_ZCZ_800			0x1E2F
#define AM335X_ZCZ_1000			0x1C2F
#define AM335X_ZCE_300			0x1FDF
#define AM335X_ZCE_600			0x1F9F

#define DEVICE_ID_MASK			0x1FFF

int get_efuse_mpu_max_freq(void)
{
	void *cdev = (void *)CTRL_DEVICE_BASE;
	int sil_rev = readl(cdev + CDEV_DEVICEID) >> 28;

	if (sil_rev == 1) {

		/* PG 2.0, efuse may not be set. */
		return MPUPLL_M_800;
	} else if (sil_rev >= 2) {

		/* Check what the efuse says our max speed is. */
		int max_freq = readl(cdev + CDEV_EFUSE_SMA);

		switch (max_freq & DEVICE_ID_MASK) {
		case AM335X_ZCZ_1000:
			return MPUPLL_M_1000;
		case AM335X_ZCZ_800:
			return MPUPLL_M_800;
		case AM335X_ZCZ_720:
			return MPUPLL_M_720;
		case AM335X_ZCZ_600:
		case AM335X_ZCE_600:
			return MPUPLL_M_600;
		case AM335X_ZCZ_300:
		case AM335X_ZCE_300:
			return MPUPLL_M_300;
		}
	}

	/* PG 1.0 or otherwise unknown, use the PG1.0 max */
	return MPUPLL_M_720;
}
#if 0
int get_tps65910_mpu_vdd(int sil_rev, int freq)
{
	/* For PG2.1 and later, we have one set of values. */
	if (sil_rev >= 2) {
		switch (freq) {
		case MPUPLL_M_1000:
			return TPS65910_OP_REG_SEL_1_3_2_5;
		case MPUPLL_M_800:
			return TPS65910_OP_REG_SEL_1_2_6;
		case MPUPLL_M_720:
			return TPS65910_OP_REG_SEL_1_2_0;
		case MPUPLL_M_600:
		case MPUPLL_M_300:
			return TPS65910_OP_REG_SEL_1_1_3;
		}
	}

	/* Default to PG1.0/PG2.0 values. */
	return TPS65910_OP_REG_SEL_1_1_3;
}
#endif

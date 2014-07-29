/* ~.~ *-h-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 28 14:49:33 CST 2014
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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <os_includes.h>

#ifndef __ASSEMBLY__
#include "printers.h"
#endif

/* -----------------------------------------------------------------------------
 * For compatibility only
 * -----------------------------------------------------------------------------
 */
#define FAST_DATA_SECTION
#define FAST_FUNC_SECTION
#define SLOW_DATA_SECTION

#define BOARD_MCK			100000000	/* 100MHZ */

/* -----------------------------------------------------------------------------
 * IRQ PRI
 * -----------------------------------------------------------------------------
 */
#define IRQ_PRI_PORT			3

/* -----------------------------------------------------------------------------
 * PIN. definitions
 * -----------------------------------------------------------------------------
 */
#define IO_MR(n, id, dir, type)		\
	{ 1 << n, DW_BASE_##id, DW_ID_##id, PIO_##dir, PIO_##type }

#define PIN_FPGA_POSITION_IRQ		IO_MR(21, PIOB, INPUT, DEFAULT)
/*#define PIN_LVDS_LOCK_EINT		IO_MR(26, PIOA, INPUT, PULLUP)*/

/* -----------------------------------------------------------------------------
 * Mother board interface customized PIN definition.
 * -----------------------------------------------------------------------------
 */
/* UV LED Control and Status Pins */
#define PIN_UV_STAT_LP1_SH_CL_I		IO_MR(16, PIOB, INPUT, DEFAULT)
#define PIN_UV_STAT_LP2_SH_OP_I		IO_MR(24, PIOA, INPUT, DEFAULT)
#define PIN_UV_STAT_LP2_SH_CL_I		IO_MR(18, PIOA, INPUT, DEFAULT)
#define PIN_UV_STAT_LP1_SH_OP_I		IO_MR(14, PIOA, INPUT, DEFAULT)
#define PIN_UV_STAT_UV_RDY		IO_MR(28, PIOE, INPUT, DEFAULT)
#define PIN_UV_STAT_LP1_FAULT		IO_MR(19, PIOA, INPUT, DEFAULT)
#define PIN_UV_STAT_LP2_FAULT		IO_MR(10, PIOB, INPUT, DEFAULT)

#define PIN_UV_CTRL_LP1_ON		IO_MR(20, PIOB, INPUT, PULLUP)
#define PIN_UV_CTRL_LP1_HIGHPOWER	IO_MR(21, PIOB, INPUT, PULLUP)
#define PIN_UV_CTRL_LP2_ON		IO_MR(26, PIOA, INPUT, PULLUP)
#define PIN_UV_CTRL_LP2_HIGHPOWER	IO_MR(6, PIOB, INPUT, PULLUP)
#define PIN_UV_CTRL_SHUTTER_1_OPEN	IO_MR(23, PIOA, INPUT, PULLUP)
#define PIN_UV_CTRL_SHUTTER_2_OPEN	IO_MR(15, PIOA, INPUT, PULLUP)

/* Bell Pins */
#define MB_CUST_PIN_BELL		IO_MR(25, PIOB, OUTPUT_1, DEFAULT)

/* Pump */
/* there are max 8 colors for now, one ctrl for one pump */
#define PIN_INK_SUPPLY_CTRL2		IO_MR(0, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL4		IO_MR(1, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL1		IO_MR(2, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL3		IO_MR(3, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL6		IO_MR(4, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL8		IO_MR(5, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL5		IO_MR(6, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL7		IO_MR(7, PIOD, OUTPUT_0, DEFAULT)

/* for deposition prevention(loop ink feed)|enforce suck|other use */
#define PIN_INK_SUPPLY_CTRL10		IO_MR(8, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL12		IO_MR(9, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL9		IO_MR(10, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL11		IO_MR(11, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL14		IO_MR(12, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL16		IO_MR(13, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL13		IO_MR(14, PIOD, OUTPUT_0, DEFAULT)
#define PIN_INK_SUPPLY_CTRL15		IO_MR(15, PIOD, OUTPUT_0, DEFAULT)

/* Cleaning Pins */
#define PIN_CLEAN_RELEASE_1		PIN_INK_SUPPLY_CTRL9
#define PIN_CLEAN_SUCK_1		PIN_INK_SUPPLY_CTRL10

/* Lcd & Keyboard Pins */
/* (Bkl) irq pin to indicate that the keyboard has been reset */
#define PIN_KB_LCD_nLED			IO_MR(11, PIOB, INPUT, DEFAULT)

/* (D0) irq pin to indicate that the lcd start to work */
#define PIN_KB_LCD_BUSY			IO_MR(8, PIOB, INPUT, DEFAULT)

/* (Key_int)irq pin to indicate that key has been pushed */
#define PIN_KB_CTRL_KB_IRQ		IO_MR(16, PIOA, INPUT, DEFAULT)

/* used to reset keyboard */
#define PIN_KB_CTRL_KB_RST		IO_MR(12, PIOB, OUTPUT_0, DEFAULT)

void os_preinit(void);
void os_postinit(void);
#endif

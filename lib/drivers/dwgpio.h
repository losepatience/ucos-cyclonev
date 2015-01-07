/* ~.~ *-h-*
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

#ifndef __DWGPIO_H__
#define __DWGPIO_H__

#include <asm/types.h>
#include <gpio.h>

#define DW_IRQ_PIOA	196
#define DWGPIO_CHIP_NUM	(2)

int dw_gpio_init(void);

#define PIO_INPUT	0
#define PIO_OUTPUT_0	1
#define PIO_OUTPUT_1	2

#define PIO_DEFAULT	(0 << 0)
#define PIO_PULLUP	(1 << 0)
#define PIO_DEGLITCH	(1 << 1)
#define PIO_OPENDRAIN	(1 << 2)

#define DW_ID_PIOA	(0)
#define DW_ID_PIOB	(1)
#define DW_ID_PIOC	(2)
#define DW_ID_PIOD	(3)
#define DW_ID_PIOE	(4)
#define DW_ID_PIOF	(5)
#define DW_BASE_PIOA	DW_ID_PIOA
#define DW_BASE_PIOB	DW_ID_PIOB
#define DW_BASE_PIOC	DW_ID_PIOC
#define DW_BASE_PIOD	DW_ID_PIOD
#define DW_BASE_PIOE	DW_ID_PIOE
#define DW_BASE_PIOF	DW_ID_PIOF

typedef struct pio_imode {
	unsigned int type;	/* 0 is Edge, 1 is Level */
	unsigned int polarity;	/* 0 is Falling Edge */
	unsigned int dual;	/* dual active */
} Pio_Imode;

typedef struct {
	unsigned int mask;
	unsigned int pio;
	unsigned char id;
	unsigned char type;
	unsigned char attribute;
	Pio_Imode imode;
} Pin;

unsigned char PIO_Configure(const Pin *list, unsigned int size);
void PIO_Set(const Pin *pin);
void PIO_Clear(const Pin *pin);
char PIO_Get(const Pin *pin);
void PIO_InitializeInterrupts(unsigned int priority);
void PIO_ConfigureIt(const Pin *pin, void (*handler)(const Pin *));
void PIO_UnConfigureIt(const Pin *pin);
void PIO_EnableIt(const Pin *pin);
void PIO_DisableIt(const Pin *pin);

#endif /* __DWGPIO_H__ */

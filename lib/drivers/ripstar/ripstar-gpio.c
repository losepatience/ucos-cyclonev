/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Mon Jul 14 09:54:50 CST 2014
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

#define FPGA_GPIO_BASE	0xff204000

#define FPGA_GPIO_RWIO	(0x0)
#define FPGA_GPIO_RPIN	(0x4)

#define FPGA_GPIO_IER	(0xC)
#define FPGA_GPIO_IMR	(0x10)

#define FPGA_GPIO_TM	(0x18)
#define FPGA_GPIO_TP	(0x1C)
#define FPGA_GPIO_MR	(0x20)

#define FPGA_GPIO_IRQ	((72 + 7))

#include <dwgpio.h>
#include <asm/io.h>

struct gpio_chip __gc[DWGPIO_CHIP_NUM + 4];
static int __ngpio[] = { 13, 18, 14, 15 };

/* FIXME */
static int rsgpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (__ngpio[i] == gc->ngpio)
			break;
	}

	return FPGA_GPIO_IRQ + i;
}

static int rsgpio_get(struct gpio_chip *gc, unsigned offset)
{
	return (readl(gc->priv + FPGA_GPIO_RPIN) >> offset) & 1;
}

static void rsgpio_set(struct gpio_chip *gc, unsigned offset, int val)
{
	unsigned int reg;
	
	local_irq_save(gc->irqflags);
	reg = readl(gc->priv + FPGA_GPIO_RWIO);
	if (val)
		reg |= 1 << offset;
	else
		reg &= ~(1 << offset);
	writel(reg, gc->priv + FPGA_GPIO_RWIO);
	local_irq_restore(gc->irqflags);
}

static int rsgpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	unsigned int mr;
	
	local_irq_save(gc->irqflags);
	/* Set pin as input, assumes software controlled IP */
	mr = readl(gc->priv + FPGA_GPIO_MR);
	mr &= ~(1 << offset);
	writel(mr, gc->priv + FPGA_GPIO_MR);
	local_irq_restore(gc->irqflags);

	return 0;
}

static int rsgpio_direction_output(struct gpio_chip *gc,
				unsigned int offset, int val)
{
	unsigned int mr;
	
	rsgpio_set(gc, offset, val);

	local_irq_save(gc->irqflags);
	/* Set pin as output, assumes software controlled IP */
	mr = readl(gc->priv + FPGA_GPIO_MR);
	mr |= 1 << offset;
	writel(mr, gc->priv + FPGA_GPIO_MR);
	local_irq_restore(gc->irqflags);
	return 0;
}

/* ---------------------------------------------------------
 * array operations
 * --------------------------------------------------------- */
static int rsgpio_get_array(struct gpio_chip *gc, unsigned mask)
{
	return readl(gc->priv + FPGA_GPIO_RPIN) & mask;
}

static void rsgpio_set_array(struct gpio_chip *gc, unsigned mask, int val)
{
	unsigned int reg;
	
	local_irq_save(gc->irqflags);
	reg = readl(gc->priv + FPGA_GPIO_RWIO);
	if (val)
		reg |= mask;
	else
		reg &= ~mask;

	writel(reg, gc->priv + FPGA_GPIO_RWIO);
	local_irq_restore(gc->irqflags);
}

static int rsgpio_direction_input_array(struct gpio_chip *gc, unsigned mask)
{
	unsigned int ddr;
	
	local_irq_save(gc->irqflags);
	/* Set pin as input, assumes software controlled IP */
	ddr = readl(gc->priv + FPGA_GPIO_MR);
	ddr &= ~mask;
	writel(ddr, gc->priv + FPGA_GPIO_MR);
	local_irq_restore(gc->irqflags);

	return 0;
}

static int rsgpio_direction_output_array(struct gpio_chip *gc,
				unsigned int mask, int val)
{
	unsigned int ddr;
	
	rsgpio_set_array(gc, mask, val);

	local_irq_save(gc->irqflags);
	/* Set pin as output, assumes software controlled IP */
	ddr = readl(gc->priv + FPGA_GPIO_MR);
	ddr |= mask;
	writel(ddr, gc->priv + FPGA_GPIO_MR);
	local_irq_restore(gc->irqflags);
	return 0;
}

int rsgpio_init(void)
{
	/* there are 2 * 29 gpio in hps */
	int tmp = 2 * 29;
	int i;

	for (i = 0; i < 4; i++) {

		struct gpio_chip *gc = &__gc[2 + i];

		gc->priv	= (void *)(FPGA_GPIO_BASE + i * 0x200);
		gc->ngpio	= __ngpio[i];
		gc->base	= tmp;
		tmp		+= gc->ngpio;

		gc->to_irq		= rsgpio_to_irq;
		gc->direction_input	= rsgpio_direction_input;
		gc->direction_output	= rsgpio_direction_output;
		gc->get			= rsgpio_get;
		gc->set			= rsgpio_set;

		gc->get_array		= rsgpio_get_array;
		gc->set_array		= rsgpio_set_array;
		gc->direction_input_array  = rsgpio_direction_input_array;
		gc->direction_output_array = rsgpio_direction_output_array;

		/* edge-sensitive only */
		writel(0xFFFF, gc->priv + FPGA_GPIO_TM);
		gpiochip_add(gc);
	}

	return 0;
}

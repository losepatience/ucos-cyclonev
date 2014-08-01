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

static spinlock_t gpio_lock;

struct gpio_chip __gc[DWGPIO_CHIP_NUM + 4];
static int __ngpio[] = { 13, 18, 14, 15 };

/* FIXME */
static int cycgpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (__ngpio[i] == gc->ngpio)
			break;
	}

	return FPGA_GPIO_IRQ + i;
}

static int cycgpio_get(struct gpio_chip *gc, unsigned offset)
{
	return (readl(gc->priv + FPGA_GPIO_RPIN) >> offset) & 1;
}

static void cycgpio_set(struct gpio_chip *gc, unsigned offset, int val)
{
	unsigned int data_reg;
	unsigned long flags = 0;

	val &= 1;

	spin_lock_irqsave(&gpio_lock, flags);
	data_reg = readl(gc->priv + FPGA_GPIO_RWIO);
	data_reg = (data_reg & ~(1 << offset)) | (val << offset);
	writel(data_reg, gc->priv + FPGA_GPIO_RWIO);
	spin_unlock_irqrestore(&gpio_lock, flags);
}

static int cycgpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	unsigned int mr;
	unsigned long flags = 0;

	spin_lock_irqsave(&gpio_lock, flags);
	/* Set pin as input, assumes software controlled IP */
	mr = readl(gc->priv + FPGA_GPIO_MR);
	mr &= ~(1 << offset);
	writel(mr, gc->priv + FPGA_GPIO_MR);
	spin_unlock_irqrestore(&gpio_lock, flags);

	return 0;
}

static int cycgpio_direction_output(struct gpio_chip *gc,
				unsigned int offset, int val)
{
	unsigned int mr;
	unsigned long flags = 0;

	cycgpio_set(gc, offset, val);

	spin_lock_irqsave(&gpio_lock, flags);
	/* Set pin as output, assumes software controlled IP */
	mr = readl(gc->priv + FPGA_GPIO_MR);
	mr |= (1 << offset);
	writel(mr, gc->priv + FPGA_GPIO_MR);
	spin_unlock_irqrestore(&gpio_lock, flags);
	return 0;
}

/* ---------------------------------------------------------
 * array operations
 * --------------------------------------------------------- */
static int cycgpio_get_array(struct gpio_chip *gc, unsigned mask)
{
	return readl(gc->priv + FPGA_GPIO_RPIN) & mask;
}

static void cycgpio_set_array(struct gpio_chip *gc, unsigned mask, int val)
{
	unsigned int reg;
	unsigned long flags = 0;

	val &= 1;

	spin_lock_irqsave(&gpio_lock, flags);

	reg = readl(gc->priv + FPGA_GPIO_RWIO);
	if (val)
		reg |= mask;
	else
		reg &= ~mask;

	writel(reg, gc->priv + FPGA_GPIO_RWIO);
	spin_unlock_irqrestore(&gpio_lock, flags);
}

static int cycgpio_direction_input_array(struct gpio_chip *gc, unsigned mask)
{
	unsigned int ddr;
	unsigned long flags = 0;

	spin_lock_irqsave(&gpio_lock, flags);
	/* Set pin as input, assumes software controlled IP */
	ddr = readl(gc->priv + FPGA_GPIO_MR);
	ddr &= ~mask;
	writel(ddr, gc->priv + FPGA_GPIO_MR);
	spin_unlock_irqrestore(&gpio_lock, flags);

	return 0;
}

static int cycgpio_direction_output_array(struct gpio_chip *gc,
				unsigned int mask, int val)
{
	unsigned int ddr;
	unsigned long flags = 0;

	cycgpio_set_array(gc, mask, val);

	spin_lock_irqsave(&gpio_lock, flags);
	/* Set pin as output, assumes software controlled IP */
	ddr = readl(gc->priv + FPGA_GPIO_MR);
	ddr |= mask;
	writel(ddr, gc->priv + FPGA_GPIO_MR);
	spin_unlock_irqrestore(&gpio_lock, flags);
	return 0;
}

static void cycgpio_set_imode(struct gpio_chip *gc, int en, unsigned mask,
		unsigned itmask, unsigned imode, unsigned pol)
{
	unsigned long flags = 0;
	unsigned int reg;

	/* disable irq */
	if (en == 0) {
		reg = readl(gc->priv + FPGA_GPIO_IER);
		reg &= ~mask;
		writel(reg, gc->priv + FPGA_GPIO_IER);
		return;
	}

	if(mask & itmask) {

		spin_lock_irqsave(&gpio_lock, flags);
		if (mask & imode)	/* level */
			writel(~imode, gc->priv + FPGA_GPIO_TM);
		else	/* edge */
			writel(imode, gc->priv + FPGA_GPIO_TM);

		if (mask & pol)	/* rise|high */
			writel(~pol, gc->priv + FPGA_GPIO_TP);
		else
			writel(pol, gc->priv + FPGA_GPIO_TP);
		spin_unlock_irqrestore(&gpio_lock, flags);
	}

	writel(mask, gc->priv + FPGA_GPIO_IER);
}

int cycgpio_init(void)
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

		gc->to_irq		= cycgpio_to_irq;
		gc->direction_input	= cycgpio_direction_input;
		gc->direction_output	= cycgpio_direction_output;
		gc->get			= cycgpio_get;
		gc->set			= cycgpio_set;

		gc->set_imode		= cycgpio_set_imode;
		gc->get_array		= cycgpio_get_array;
		gc->set_array		= cycgpio_set_array;
		gc->direction_input_array  = cycgpio_direction_input_array;
		gc->direction_output_array = cycgpio_direction_output_array;
		gpiochip_add(gc);
	}

	return 0;
}

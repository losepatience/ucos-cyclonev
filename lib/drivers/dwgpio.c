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

#include <stdio.h>
#include <stddef.h>
#include <platform.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <ripstar-gpio.h>
#include <dwgpio.h>

#define GPIO_INT_EN_REG_OFFSET		(0x30)
#define GPIO_INT_MASK_REG_OFFSET	(0x34)
#define GPIO_INT_TYPE_LEVEL_REG_OFFSET	(0x38)
#define GPIO_INT_POLARITY_REG_OFFSET	(0x3c)
#define GPIO_INT_STATUS_REG_OFFSET	(0x40)
#define GPIO_INT_EOI_REG_OFFSET		(0x4C)

#define DW_GPIO_DR			(0x0)
#define GPIO_DDR_OFFSET_PORT		(0x4)
#define DW_GPIO_DEBOUNCE		(0x48)
#define DW_GPIO_EXT			(0x50)

#define MAX_INTERRUPT_SOURCES		10

typedef struct {

	const Pin *pin;
	void (*handler)(const Pin *);

} InterruptSource;

static unsigned int numSources;
static InterruptSource pSources[MAX_INTERRUPT_SOURCES];
static unsigned long pio_irqflags;

extern struct gpio_chip __gc[DWGPIO_CHIP_NUM + 4];

static int dw_gpio_to_irq(struct gpio_chip *gc, unsigned offset)
{
	return gc->base / 29 + DW_IRQ_PIOA;
}

static int dw_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	return (readl(gc->priv + DW_GPIO_EXT) >> offset) & 1;
}

static void dw_gpio_set(struct gpio_chip *gc, unsigned offset, int val)
{
	unsigned int reg;

	local_irq_save(pio_irqflags);
	reg = readl(gc->priv + DW_GPIO_DR);
	if (val)
		reg |= 1 << offset;
	else
		reg &= ~(1 << offset);
	writel(reg, gc->priv + DW_GPIO_DR);
	local_irq_restore(pio_irqflags);
}

static int dw_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	unsigned int ddr;

	local_irq_save(pio_irqflags);
	/* Set pin as input, assumes software controlled IP */
	ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	ddr &= ~(1 << offset);
	writel(ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	local_irq_restore(pio_irqflags);

	return 0;
}

static int dw_gpio_direction_output(struct gpio_chip *gc,
		unsigned int offset, int val)
{
	unsigned int ddr;

	dw_gpio_set(gc, offset, val);

	local_irq_save(pio_irqflags);
	/* Set pin as output, assumes software controlled IP */
	ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	ddr |= (1 << offset);
	writel(ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	local_irq_restore(pio_irqflags);
	return 0;
}

/* ---------------------------------------------------------
 * array operations
 * --------------------------------------------------------- */
static int dw_gpio_get_array(struct gpio_chip *gc, unsigned mask)
{
	return readl(gc->priv + DW_GPIO_EXT) & mask;
}

static void dw_gpio_set_array(struct gpio_chip *gc, unsigned mask, int val)
{
	unsigned int reg;

	local_irq_save(pio_irqflags);

	reg = readl(gc->priv + DW_GPIO_DR);
	if (val)
		reg |= mask;
	else
		reg &= ~mask;

	writel(reg, gc->priv + DW_GPIO_DR);
	local_irq_restore(pio_irqflags);
}

static int dw_gpio_direction_input_array(struct gpio_chip *gc, unsigned mask)
{
	unsigned int ddr;

	local_irq_save(pio_irqflags);
	/* Set pin as input, assumes software controlled IP */
	ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	ddr &= ~mask;
	writel(ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	local_irq_restore(pio_irqflags);

	return 0;
}

static int dw_gpio_direction_output_array(struct gpio_chip *gc,
		unsigned int mask, int val)
{
	unsigned int ddr;

	dw_gpio_set_array(gc, mask, val);

	local_irq_save(pio_irqflags);
	/* Set pin as output, assumes software controlled IP */
	ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	ddr |= mask;
	writel(ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	local_irq_restore(pio_irqflags);
	return 0;
}

int dw_gpio_init(void)
{
	int i;

	for (i = 0; i < DWGPIO_CHIP_NUM; i++) {

		struct gpio_chip *gc = &__gc[i];

		gc->priv	= (void *)(SOCFPGA_GPIO0_ADDRESS + 0x1000 * i);
		gc->ngpio	= 29;
		gc->base	= 29 * i;

		gc->to_irq		= dw_gpio_to_irq;
		gc->get			= dw_gpio_get;
		gc->set			= dw_gpio_set;
		gc->direction_input	= dw_gpio_direction_input;
		gc->direction_output	= dw_gpio_direction_output;

		gc->get_array		= dw_gpio_get_array;
		gc->set_array		= dw_gpio_set_array;
		gc->direction_input_array  = dw_gpio_direction_input_array;
		gc->direction_output_array = dw_gpio_direction_output_array;

		/* edge-sensitive only */
		writel(0x1FFFFFFF, gc->priv + GPIO_INT_TYPE_LEVEL_REG_OFFSET);
		gpiochip_add(gc);

	}

	return 0;
}

/* --------------------------------------------------------------------------
 * Old APIs
 * -------------------------------------------------------------------------- */
static struct gpio_chip *__get_gpiochip(const Pin *pin)
{
	return (pin == NULL) ? NULL : &__gc[pin->pio];
}

unsigned char PIO_Configure(const Pin *list, unsigned int size)
{
	for (; size > 0; list++, size--) {

		struct gpio_chip *gc = __get_gpiochip(list);
		unsigned int type = list->type;
		int i;

		for (i = 0; i < gc->ngpio; i++) {

			if (type == PIO_INPUT) {
				gc->direction_input_array(gc, list->mask);
			} else {
				int val = type & 0x2;
				gc->direction_output_array(gc, list->mask, val);
			}
		}

	}

	return 1;
}

void PIO_Set(const Pin *pin)
{
	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc != 0)
		gc->direction_output_array(gc, pin->mask, 1);
}

void PIO_Clear(const Pin *pin)
{
	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc != 0)
		gc->direction_output_array(gc, pin->mask, 0);
}

char PIO_Get(const Pin *pin)
{
	struct gpio_chip *gc;

	gc = __get_gpiochip(pin);
	if (gc == 0)
		return -1;

	if (gc->direction_input_array(gc, pin->mask))
		return 1;

	return 0;
}

static void gpio_interrupt(void *arg)
{
	unsigned int status;
	struct gpio_chip *gc;
	int i;

	for (i = 0; i < numSources; i++) {
		unsigned int IR, CIR;
		unsigned int val;

		gc = __get_gpiochip(pSources[i].pin);

		if (gc->base <= 29 * (DWGPIO_CHIP_NUM - 1)) {
			IR = GPIO_INT_STATUS_REG_OFFSET;
			CIR = GPIO_INT_EOI_REG_OFFSET;
		} else {
			IR = CYCGPIO_IR;
			CIR = CYCGPIO_CIR;
		}

		status = readl(gc->priv + IR);
		if ((status & pSources[i].pin->mask) == 0)
			continue;

		pSources[i].handler(pSources[i].pin);
		status &= ~(pSources[i].pin->mask);

		val = readl(gc->priv + CIR);
		val |= pSources[i].pin->mask;
		writel(val, gc->priv + CIR);
	}
}

void PIO_InitializeInterrupts(unsigned int priority)
{
	static Pin pin[ARCH_NR_GPIOCHIPS];
	int i;

	dw_gpio_init();
	rsgpio_init();

	for (i = 0; i < ARCH_NR_GPIOCHIPS; i++) {

		struct gpio_chip *gc = &__gc[i];

		pin[i].pio = DW_BASE_PIOA + i;
		pin[i].id = DW_ID_PIOA + i;

		request_irq(gc->to_irq(gc, 0), gpio_interrupt, &pin[i]);
	}
}

void PIO_ConfigureIt(const Pin *pin, void (*handler)(const Pin *))
{
	InterruptSource *pSource;

	if (numSources >= MAX_INTERRUPT_SOURCES) {
		pr_err("%s: numSources >= MAX_INTERRUPT_SOURCES\n", __func__);
		return;
	}

	local_irq_save(pio_irqflags);
	PIO_Configure(pin, 1);
	pSource = &(pSources[numSources]);
	pSource->pin = pin;
	pSource->handler = handler;
	numSources++;
	local_irq_restore(pio_irqflags);
}

void PIO_UnConfigureIt(const Pin *pin)
{
	int i, j;

	local_irq_save(pio_irqflags);
	for (i = 0; i < numSources; i++) {
		if (pSources[i].pin == pin) {
			for (j = i; j < numSources - 1; j++) {
				pSources[j].pin = pSources[j + 1].pin;
				pSources[j].handler = pSources[j + 1].handler;
			}
			numSources--;
			break;
		}
	}
	local_irq_restore(pio_irqflags);
}

void PIO_EnableIt(const Pin *pin)
{
	unsigned reg, IER;
	struct gpio_chip *gc = __get_gpiochip(pin);

	if (!gc)
		return;

	if (gc->base <= 29 * (DWGPIO_CHIP_NUM - 1))
		IER = GPIO_INT_EN_REG_OFFSET;
	else
		IER = CYCGPIO_IER;

	local_irq_save(pio_irqflags);
	reg = readl(gc->priv + IER);
	reg |= pin->mask;
	writel(reg, gc->priv + IER);
	local_irq_restore(pio_irqflags);
}

void PIO_DisableIt(const Pin *pin)
{
	unsigned reg, IER;
	struct gpio_chip *gc = __get_gpiochip(pin);

	if (!gc)
		return;

	if (gc->base <= 29 * (DWGPIO_CHIP_NUM - 1))
		IER = GPIO_INT_EN_REG_OFFSET;
	else
		IER = CYCGPIO_IER;

	local_irq_save(pio_irqflags);
	reg = readl(gc->priv + IER);
	reg &= ~pin->mask;
	writel(reg, gc->priv + IER);
	local_irq_restore(pio_irqflags);
}


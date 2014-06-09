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
#include <dwgpio.h>
#include <platform.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/regs.h>
#include <csp.h>

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
static spinlock_t pio_lock;

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
	unsigned int data_reg;
	unsigned long flags = 0;

	val &= 1;

	spin_lock_irqsave(&pio_lock, flags);
	data_reg = readl(gc->priv + DW_GPIO_DR);
	data_reg = (data_reg & ~(1 << offset)) | (val << offset);
	writel(data_reg, gc->priv + DW_GPIO_DR);
	spin_unlock_irqrestore(&pio_lock, flags);
}

static int dw_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	unsigned int gpio_ddr;
	unsigned long flags = 0;

	spin_lock_irqsave(&pio_lock, flags);
	/* Set pin as input, assumes software controlled IP */
	gpio_ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	gpio_ddr &= ~(1 << offset);
	writel(gpio_ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	spin_unlock_irqrestore(&pio_lock, flags);

	return 0;
}

static int dw_gpio_direction_output(struct gpio_chip *gc,
				unsigned int offset, int val)
{
	unsigned int gpio_ddr;
	unsigned long flags = 0;

	dw_gpio_set(gc, offset, val);

	spin_lock_irqsave(&pio_lock, flags);
	/* Set pin as output, assumes software controlled IP */
	gpio_ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);
	gpio_ddr |= (1 << offset);
	writel(gpio_ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
	spin_unlock_irqrestore(&pio_lock, flags);
	return 0;
}

static struct gpio_chip dwgc[ARCH_NR_GPIOCHIPS];

int dw_gpio_init(void)
{
	int i;

	for (i = 0; i < ARCH_NR_GPIOCHIPS; i++) {

		struct gpio_chip *gc = &dwgc[i];

		gc->priv = (void *)(SOCFPGA_GPIO0_ADDRESS + i * 0x1000);
		gc->ngpio = 29;
		gc->base = i * 29;

		gc->to_irq		= dw_gpio_to_irq;
		gc->direction_input	= dw_gpio_direction_input;
		gc->direction_output	= dw_gpio_direction_output;
		gc->get			= dw_gpio_get;
		gc->set			= dw_gpio_set;

		gpiochip_add(gc);
	}

	return 0;
}

/* --------------------------------------------------------------------------
 * Old APIs
 * -------------------------------------------------------------------------- */

#ifndef INCOMPATIBLE_WITH_ATMEL
static struct gpio_chip *__get_gpiochip(const Pin *pin)
{
	return (pin == NULL) ? NULL : &dwgc[pin->pio];
}

unsigned char PIO_Configure(const Pin *list, unsigned int size)
{
	unsigned int dr;
	unsigned int ddr;
	unsigned long flags = 0;

	for (; size > 0; list++, size--) {
		struct gpio_chip *gc = __get_gpiochip(list);
		unsigned int type = list->type;

		if (gc == 0) {
			pr_warn("%s: invalid parameter\n", __func__);
			continue;
		}

		spin_lock_irqsave(&pio_lock, flags);
		dr = readl(gc->priv + DW_GPIO_DR);
		ddr = readl(gc->priv + GPIO_DDR_OFFSET_PORT);

		if (type == PIO_INPUT) {
			ddr &= ~list->mask;
		} else if (type == PIO_OUTPUT_0) {
			dr &= ~list->mask;
			ddr |= list->mask;
		} else if (type == PIO_OUTPUT_1) {
			dr |= list->mask;
			ddr |= list->mask;
		} else {
			spin_unlock_irqrestore(&pio_lock, flags);
			return 0;
		}

		writel(dr, gc->priv + DW_GPIO_DR);
		writel(ddr, gc->priv + GPIO_DDR_OFFSET_PORT);
		spin_unlock_irqrestore(&pio_lock, flags);
	}

	return 1;
}

void PIO_Set(const Pin *pin)
{
	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc != 0) {
		unsigned long flags = 0;

		spin_lock_irqsave(&pio_lock, flags);
		unsigned int reg = readl(gc->priv + DW_GPIO_DR);
		reg |= pin->mask;
		writel(reg, gc->priv + DW_GPIO_DR);
		spin_unlock_irqrestore(&pio_lock, flags);
	}
}

void PIO_Clear(const Pin *pin)
{
	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc != 0) {
		unsigned long flags = 0;

		spin_lock_irqsave(&pio_lock, flags);
		unsigned int reg = readl(gc->priv + DW_GPIO_DR);
		reg &= ~pin->mask;
		writel(reg, gc->priv + DW_GPIO_DR);
		spin_unlock_irqrestore(&pio_lock, flags);
	}
}

char PIO_Get(const Pin *pin)
{
	unsigned int reg;
	struct gpio_chip *gc;

	gc = __get_gpiochip(pin);
	if (gc == 0)
		return -1;

	reg = readl(gc->priv + DW_GPIO_EXT);

	return (reg & pin->mask) == 0 ? 0 : 1;
}

static void gpio_interrupt(void *arg)
{
	int i;
	static unsigned int status;
	const Pin *pin = (const Pin *)arg;
	struct gpio_chip *gc = __get_gpiochip(pin);

	status = readl(gc->priv + GPIO_INT_STATUS_REG_OFFSET);
	for (i = 0; (status != 0) && (i < numSources); i++) {

		if (pSources[i].pin->id != pin->id)
			continue;

		if ((status & pSources[i].pin->mask) != 0) {
			unsigned int reg;
			reg = (unsigned int)gc->priv + GPIO_INT_EOI_REG_OFFSET;

			pSources[i].handler(pSources[i].pin);
			status &= ~(pSources[i].pin->mask);

			if (!(pSources[i].pin->mask
				& pSources[i].pin->imode.type))
				writel(readl(reg) | pSources[i].pin->mask, reg);
		}
	}
}

void PIO_InitializeInterrupts(unsigned int priority)
{
	static Pin pin[ARCH_NR_GPIOCHIPS];
	int i;

	dw_gpio_init();

	for (i = 0; i < ARCH_NR_GPIOCHIPS; i++) {

		struct gpio_chip *gc = &dwgc[i];

		pin[i].pio = DW_BASE_PIOA + i;
		pin[i].id = DW_ID_PIOA + i;

		request_irq(gc->to_irq(gc, 0), gpio_interrupt, &pin[i]);
	}
}

void PIO_ConfigureIt(const Pin *pin, void (*handler)(const Pin *))
{
	unsigned long flags = 0;
	InterruptSource *pSource;

	if (numSources >= MAX_INTERRUPT_SOURCES) {
		pr_err("%s: numSources >= MAX_INTERRUPT_SOURCES\n", __func__);
		return;
	}

	spin_lock_irqsave(&pio_lock, flags);
	pSource = &(pSources[numSources]);
	pSource->pin = pin;
	pSource->handler = handler;
	numSources++;
	spin_unlock_irqrestore(&pio_lock, flags);
}

void PIO_UnConfigureIt(const Pin *pin)
{
	int i, j;
	unsigned long flags = 0;

	spin_lock_irqsave(&pio_lock, flags);
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
	spin_unlock_irqrestore(&pio_lock, flags);
}

static void PIO_Set_debounce(const Pin *pin)
{
	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc != 0) {
		unsigned long flags = 0;

		spin_lock_irqsave(&pio_lock, flags);
		unsigned int reg = readl(gc->priv + DW_GPIO_DEBOUNCE);
		reg |= pin->mask;
		writel(reg, gc->priv + DW_GPIO_DEBOUNCE);
		spin_unlock_irqrestore(&pio_lock, flags);
	}
}

void PIO_EnableIt(const Pin *pin)
{
	unsigned int reg;
	unsigned long flags = 0;

	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc == NULL)
		return;

	if (pin->type != PIO_INPUT) {
		pr_err("%s: pin must be of a type PIO_INPUT", __func__);
		return;
	}

	PIO_Configure(pin, 1);
	PIO_Set_debounce(pin);

	spin_lock_irqsave(&pio_lock, flags);
	if (pin->mask & pin->imode.imask) {

		reg = readl(gc->priv + GPIO_INT_TYPE_LEVEL_REG_OFFSET);
		if (pin->mask & pin->imode.type)
			reg &= ~pin->imode.type;
		else
			reg |= pin->mask;
		writel(reg, gc->priv + GPIO_INT_TYPE_LEVEL_REG_OFFSET);

		reg = readl(gc->priv + GPIO_INT_POLARITY_REG_OFFSET);
		if (pin->mask & pin->imode.polarity)
			reg |= pin->imode.polarity;
		else
			reg &= ~pin->mask;
		writel(reg, gc->priv + GPIO_INT_POLARITY_REG_OFFSET);
	}

	reg = readl(gc->priv + GPIO_INT_MASK_REG_OFFSET);
	reg &= ~pin->mask;
	writel(reg, gc->priv + GPIO_INT_MASK_REG_OFFSET);

	reg = readl(gc->priv + GPIO_INT_EN_REG_OFFSET);
	reg |= pin->mask;
	writel(reg, gc->priv + GPIO_INT_EN_REG_OFFSET);
	spin_unlock_irqrestore(&pio_lock, flags);
}

void PIO_DisableIt(const Pin *pin)
{
	unsigned int reg;
	unsigned long flags = 0;

	struct gpio_chip *gc = __get_gpiochip(pin);
	if (gc == NULL)
		return;

	spin_lock_irqsave(&pio_lock, flags);
	reg = readl(gc->priv + GPIO_INT_EN_REG_OFFSET);
	reg &= ~pin->mask;
	writel(reg, gc->priv + GPIO_INT_EN_REG_OFFSET);
	spin_unlock_irqrestore(&pio_lock, flags);
}

#endif /* INCOMPATIBLE_WITH_ATMEL */

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
#include <gpio.h>
#include <errno.h>

struct gpio_desc {
	struct gpio_chip	*chip;
	unsigned long		flags;
/* flag symbols are bit numbers */
#define FLAG_OPEN_DRAIN	1	/* gpio is open drain type */
#define FLAG_OPEN_SOURCE 2	/* gpio is open source type */
};

static struct gpio_desc gpio_desc[ARCH_NR_GPIOS];

struct gpio_chip *gpio_to_chip(unsigned gpio)
{
	return gpio_desc[gpio].chip;
}

int gpio_set_debounce(unsigned gpio, unsigned debounce)
{
	struct gpio_chip *chip;

	if (!gpio_is_valid(gpio))
		return -EINVAL;

	chip = gpio_to_chip(gpio);
	if (!chip || !chip->set || !chip->set_debounce)
		return -EINVAL;

	gpio -= chip->base;
	if (gpio >= chip->ngpio)
		return -EINVAL;

	return chip->set_debounce(chip, gpio, debounce);

}

int gpio_to_irq(unsigned gpio)
{
	struct gpio_chip *chip;

	chip = gpio_to_chip(gpio);

	if (!chip->to_irq)
		return -ENXIO;

	return chip->to_irq(chip, gpio - chip->base);
}

int gpio_direction_input(unsigned gpio)
{
	struct gpio_chip *chip;

	if (!gpio_is_valid(gpio))
		return -EINVAL;

	chip = gpio_to_chip(gpio);
	if (!chip || !chip->get || !chip->direction_input)
		return -EINVAL;

	gpio -= chip->base;
	if (gpio >= chip->ngpio)
		return -EINVAL;

	return chip->direction_input(chip, gpio);
}

int gpio_direction_output(unsigned gpio, int value)
{
	struct gpio_chip *chip;

	if (!gpio_is_valid(gpio))
		return -EINVAL;

	chip = gpio_to_chip(gpio);
	if (!chip || !chip->get || !chip->direction_input)
		return -EINVAL;

	gpio -= chip->base;
	if (gpio >= chip->ngpio)
		return -EINVAL;

	return chip->direction_output(chip, gpio, value);
}

int gpio_get_value(unsigned gpio)
{
	struct gpio_chip *chip = gpio_to_chip(gpio);

	if (!chip->get)
		return 0;

	return chip->get(chip, gpio - chip->base);
}

/* open drain: set input to output high */
static void gpio_set_open_drain_value(unsigned gpio, int value)
{
	struct gpio_chip *chip = gpio_to_chip(gpio);
	int err = 0;

	if (value)
		err = chip->direction_input(chip, gpio - chip->base);
	else
		err = chip->direction_output(chip, gpio - chip->base, 0);

	if (err < 0)
		pr_err("%s: Error for gpio%d err %d\n", __func__, gpio, err);
}

/* open source: set input mode to output low */
static void gpio_set_open_source_value(unsigned gpio, int value)
{
	struct gpio_chip *chip = gpio_to_chip(gpio);
	int err = 0;

	if (value)
		err = chip->direction_output(chip, gpio - chip->base, 1);
	else
		err = chip->direction_input(chip, gpio - chip->base);

	if (err < 0)
		pr_err("%s: Error for gpio%d err %d\n", __func__, gpio, err);
}

void gpio_set_value(unsigned gpio, int value)
{
	unsigned long flags;
	struct gpio_chip *chip;

	chip = gpio_to_chip(gpio);

	if (GPIOF_OPEN_DRAIN & gpio_desc[gpio].flags) {
		spin_lock_irqsave(&chip->lock, flags);
		gpio_set_open_drain_value(gpio, value);
		spin_unlock_irqrestore(&chip->lock, flags);
	} else if (GPIOF_OPEN_SOURCE & gpio_desc[gpio].flags) {
		spin_lock_irqsave(&chip->lock, flags);
		gpio_set_open_source_value(gpio, value);
		spin_unlock_irqrestore(&chip->lock, flags);
	} else {
		chip->set(chip, gpio - chip->base, value);
	}
}

void gpio_set_flags(unsigned gpio, unsigned long flags)
{
	unsigned long flag;
	struct gpio_chip *chip;

	chip = gpio_to_chip(gpio);

	spin_lock_irqsave(&chip->lock, flag);
	gpio_desc[gpio].flags = flags;
	spin_unlock_irqrestore(&chip->lock, flag);
}

int gpiochip_add(struct gpio_chip *chip)
{
	int base = chip->base;
	unsigned id;

	for (id = base; id < base + chip->ngpio; id++)
		gpio_desc[id].chip = chip;
	return 0;
}

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

#ifndef __GPIO_H__
#define __GPIO_H__

#include <platform.h>

#define ARCH_NR_GPIOS		(3 * 29)
#define ARCH_NR_GPIOCHIPS	(3)

#define GPIOF_OPEN_DRAIN	(1 << 0)
#define GPIOF_OPEN_SOURCE	(1 << 1)

static inline int gpio_is_valid(int number)
{
	return number >= 0 && number < ARCH_NR_GPIOS;
}

struct gpio_chip {
	int		base;
	u16		ngpio;
	spinlock_t	lock;
	void		*priv;

	int (*direction_input)(struct gpio_chip *chip, unsigned offset);
	int (*get)(struct gpio_chip *chip, unsigned offset);
	int (*direction_output)(struct gpio_chip *chip,
				unsigned offset, int value);
	void (*set)(struct gpio_chip *chip, unsigned offset, int value);

	int (*to_irq)(struct gpio_chip *chip, unsigned offset);
	int (*set_debounce)(struct gpio_chip *chip,
			    unsigned offset, unsigned debounce);
};

int gpio_set_debounce(unsigned gpio, unsigned debounce);
int gpio_to_irq(unsigned gpio);
void gpio_set_value(unsigned gpio, int value);
int gpio_get_value(unsigned gpio);
int gpio_direction_output(unsigned gpio, int value);
int gpio_direction_input(unsigned gpio);
int gpiochip_add(struct gpio_chip *chip);

#endif /* _ALT_GPIO_H_ */
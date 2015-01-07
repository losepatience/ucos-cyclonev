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
#include <errno.h>
#include "i2c.h"

static LIST_HEAD(i2c_adap_list);

int i2c_is_numbered_adapter_registered(int busnum)
{
	struct i2c_adapter *tmp;
	unsigned long flags = 0;

	local_irq_save(flags);
	list_for_each_entry(tmp, &i2c_adap_list, list) {
		if (tmp->busnum == busnum) {
			local_irq_restore(flags);
			pr_info("i2c adapter%d already registered\n");
			return 1;
		}
	}
	local_irq_restore(flags);
	return 0;
}

int i2c_register_adapter(struct i2c_adapter *adap, int busnum)
{
	unsigned long flags = 0;

	if (!adap->xfer) {
		pr_err("i2c: Attempt to register adapter with no xfer!\n");
		return -EINVAL;
	}

	local_irq_save(flags);
	if (i2c_is_numbered_adapter_registered(busnum)) {
		local_irq_restore(flags);
		return -EINVAL;
	}

	adap->busnum = busnum;
	mutex_init(&adap->lock);
	list_add_tail(&adap->list, &i2c_adap_list);

	local_irq_restore(flags);
	return 0;
}

struct i2c_adapter *i2c_get_adapter(int busnum)
{
	struct i2c_adapter *tmp;
	unsigned long flags = 0;

	/* this lock is unneccessary for now, it is here for to add del func */
	local_irq_save(flags);
	list_for_each_entry(tmp, &i2c_adap_list, list) {
		if (tmp->busnum == busnum) {
			local_irq_restore(flags);
			return tmp;
		}

	}

	local_irq_restore(flags);
	return NULL;
}

int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	int rval;
	int retries = adap->retries;

	mutex_lock(&adap->lock);
	while (retries--) {
		rval = adap->xfer(adap, msgs, num);
		if (rval > 0)
			break;
	}
	mutex_unlock(&adap->lock);

	return rval;
}

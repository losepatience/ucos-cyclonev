/* ~.~ *-c-*
 *
 * Copyright (c) 2013, John Lee <furious_tauren@163.com>
 * Tue Apr 15 10:35:07 CST 2014
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
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <delay.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <platform.h>
#include <cycfx3.h>

#define CONFIG_DMA_BASE	(SOCFPGA_LWH2F_ADDRESS + 0x200)
#define CONFIG_DMA_IRQ		74

#define DMA_CR_OFFS		0x04
#define DMA_IR_OFFS		0x08
#define DMA_IER_OFFS		0x0C
#define DMA_IMR_OFFS		0x10
#define DMA_CIR_OFFS		0x14
#define DMA_WADDR_OFFS		0x18
#define DMA_WLEN_OFFS		0x1C
#define DMA_RADDR_OFFS		0x20
#define DMA_RLEN_OFFS		0x24

struct fpga_dma {
	void			*base;
	int			irq;

	struct mutex		rxlock;
	struct mutex		txlock;
	struct completion	rxwait;
	struct completion	txwait;

	unsigned long		timeout;
};

static struct fpga_dma *__dma;

static inline struct fpga_dma *__get_dma(void)
{
	return __dma;
}

static void dma_irq_handler(void *arg)
{
	struct fpga_dma *dma = (struct fpga_dma *)arg;
	u32 stat = readl(dma->base + DMA_IR_OFFS);

	if (stat & 0x1)
		complete(&dma->rxwait);

	if (stat & 0x2)
		complete(&dma->txwait);
}

int dma_receive(void *addr, size_t len)
{
	int ret;
	struct fpga_dma *dma = __get_dma();

	mutex_lock(&dma->rxlock);

	INIT_COMPLETION(dma->rxwait);

	writel((u32)addr, dma->base + DMA_WADDR_OFFS);
	writel(len, dma->base + DMA_WLEN_OFFS);
	writel(0x1, dma->base + DMA_CR_OFFS);

	ret = wait_for_completion_timeout(&dma->rxwait, dma->timeout);

	mutex_unlock(&dma->rxlock);

	if (ret <= 0) {
		pr_err("%s: receive failed\n", __func__);
		return -ETIMEDOUT;
	}

	return len;
}

int dma_send(void *addr, size_t len)
{
	int ret;
	struct fpga_dma *dma = __get_dma();

	mutex_lock(&dma->txlock);
	INIT_COMPLETION(dma->txwait);

	writel((u32)addr, dma->base + DMA_RADDR_OFFS);
	writel(len, dma->base + DMA_RLEN_OFFS);
	writel(0x2, dma->base + DMA_CR_OFFS);

	ret = wait_for_completion_timeout(&dma->txwait, dma->timeout);

	mutex_unlock(&dma->txlock);

	if (ret <= 0) {
		pr_err("%s: receive failed\n", __func__);
		return -ETIMEDOUT;
	}

	return len;
}

int dma_init(void)
{
	__dma = malloc(sizeof(struct fpga_dma));
	if (!__dma) {
		pr_err("%s: nomem\n", __func__);
		return -ENOMEM;
	}

	__dma->base = (void *)CONFIG_DMA_BASE;
	__dma->irq = CONFIG_DMA_IRQ;
	__dma->timeout = 5 * HZ;

	mutex_init(&__dma->rxlock);
	mutex_init(&__dma->txlock);
	init_completion(&__dma->rxwait);
	init_completion(&__dma->txwait);

	request_irq(__dma->irq, dma_irq_handler, __dma);
	writel(2, __dma->base +DMA_IER_OFFS);
	return 0;
}

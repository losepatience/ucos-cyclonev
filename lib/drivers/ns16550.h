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

#ifndef _NS16550_H_
#define _NS16550_H_

#include <stdarg.h>
#include <asm/regs.h>
#include <asm/io.h>

/* These are the definitions for the FIFO Control Register */
#define UART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define UART_FCR_TXSR		0x04 /* Transmitter soft reset */

/* These are the definitions for the Modem Control Register */
#define UART_MCR_DTR	0x01		/* DTR   */
#define UART_MCR_RTS	0x02		/* RTS   */

#define UART_LSR_THRE	0x20		/* Xmit holding register empty */
#define UART_LCR_BKSE	0x80		/* Bank select enable */

#define UART_LCRVAL	0x3		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL	(UART_MCR_DTR | UART_MCR_RTS)

/* Clear & enable FIFOs */
#define UART_FCRVAL	(UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)

#ifndef CONFIG_SYS_NS16550_IER
#define CONFIG_SYS_NS16550_IER  0x00
#endif /* CONFIG_SYS_NS16550_IER */

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

/*XXX modify it according to pll_config.h */
#define CONFIG_HPS_CLK_L4_SP_HZ		(100000000)
#define CONFIG_SYS_NS16550_CLK		CONFIG_HPS_CLK_L4_SP_HZ
#define CONFIG_BAUDRATE			57600


#define UART_REG(x)	u8 x; int pad_##x:24;

struct ns16550 {
	UART_REG(rbr);		/* 0 */
	UART_REG(ier);		/* 1 */
	UART_REG(fcr);		/* 2 */
	UART_REG(lcr);		/* 3 */
	UART_REG(mcr);		/* 4 */
	UART_REG(lsr);		/* 5 */
	UART_REG(msr);		/* 6 */
	UART_REG(spr);		/* 7 */
	UART_REG(mdr1);		/* 8 */
	UART_REG(reg9);		/* 9 */
	UART_REG(regA);		/* A */
	UART_REG(regB);		/* B */
	UART_REG(regC);		/* C */
	UART_REG(regD);		/* D */
	UART_REG(regE);		/* E */
	UART_REG(uasr);		/* F */
	UART_REG(scr);		/* 10*/
	UART_REG(ssr);		/* 11*/
	UART_REG(reg12);	/* 12*/
	UART_REG(osc_12m_sel);	/* 13*/
} __attribute__((packed));

void ns16550_init(void);
#endif

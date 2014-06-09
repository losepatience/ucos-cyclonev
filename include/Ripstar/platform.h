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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <asm/types.h>
#include <ucos_ii.h>
#include <csp.h>

#ifndef HZ
#  define HZ	OS_TICKS_PER_SEC
#endif


static inline void __msleep(unsigned int msecs)
{
	OSTimeDlyHMSM(0, 0, 0, msecs);
}

/* simple implement, not support SMP */

struct mutex {
	OS_EVENT *lock;
};

void mutex_init(struct mutex *mutex);
void mutex_destroy(struct mutex *mutex);
void mutex_lock(struct mutex *mutex);
int mutex_trylock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

typedef struct spinlock {
	u32 lock;
} spinlock_t;

#define spin_lock_irqsave(lock, flags)		\
	do {					\
		flags = OS_CPU_SR_Save();	\
		(void)lock;			\
	} while (0)

#define spin_unlock_irqrestore(lock, flags)	\
	do {					\
		OS_CPU_SR_Restore(flags);	\
	} while (0)


struct completion {
	OS_EVENT *com;
};

#define INIT_COMPLETION(x) do {		\
	INT8U err = 0;			\
	OSSemSet((x).com, 0, &err);	\
} while (0)

void init_completion(struct completion *x);
long wait_for_completion_timeout(struct completion *x, unsigned long timeout);
void complete(struct completion *x);


/* -------------------------------------------------------- */
/* ---------------------- interrupt ----------------------- */
/* -------------------------------------------------------- */
typedef void irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(void *);
int request_irq(u32 irq, irq_handler_t handler, void *arg);
void free_irq(u32 irq);

/* -------------------------------------------------------- */
/* ---------------------- timer ----------------------- */
/* -------------------------------------------------------- */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)

typedef void (*timer_callback_t)(void *, void *);
typedef struct timer {
	char			name[32];
	u32			period;

	OS_TMR			*t;
	timer_callback_t	callback; 
	void			*arg;
} timer_t;

void CPU_TS_TmrInit(void);
u32 CPU_TS_TmrRd(void);
u64 CPU_TS32_to_uSec(u32 ts_cnts);
u64 CPU_TS64_to_uSec(u64 ts_cnts);
int create_timer(timer_t *tmr);
int del_timer(timer_t *tmr);
int start_timer(timer_t *tmr);
int stop_timer(timer_t *tmr);

#endif

#endif

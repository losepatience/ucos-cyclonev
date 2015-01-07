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

#include <stdbool.h>
#include <asm/types.h>
#include <errno.h>
#include <ucos_ii.h>
#include <csp.h>

#ifndef HZ
#  define HZ	OS_TICKS_PER_SEC
#endif

static inline void __msleep(unsigned int msecs)
{
	OSTimeDlyHMSM(0, 0, 0, msecs);
}

/* --------------------------------------------------------
 * mutex:
 *   1. simple implement, not support SMP
 *   2. the count of mutexes can not be large than
 *      OS_MAX_EVENTS
 * -------------------------------------------------------- */
struct mutex {
	OS_EVENT *lock;
};

static inline void mutex_init(struct mutex *mutex)
{
	mutex->lock = OSSemCreate(1);
}

static inline void mutex_destroy(struct mutex *mutex)
{
	u8 err = 0;
	OSSemDel(mutex->lock, OS_DEL_ALWAYS, &err);
}

static inline void mutex_lock(struct mutex *mutex)
{
	u8 err = 0;
	OSSemPend(mutex->lock, 0, &err);
}

/*XXX 1 if sucessfully, others 0 */
static inline int mutex_trylock(struct mutex *mutex)
{
	u8 err = 0;
	OSSemPend(mutex->lock, 100, &err);
	return err == OS_ERR_NONE;
}

static inline void mutex_unlock(struct mutex *mutex)
{
	OSSemPost(mutex->lock);
}

/* --------------------------------------------------------
 * spinlock:
 * -------------------------------------------------------- */
typedef struct spinlock {
	u32 lock;
} spinlock_t;

#define local_irq_save(flags)		\
	do {					\
		flags = OS_CPU_SR_Save();	\
	} while (0)
#define local_irq_restore(flags)		\
	do {					\
		OS_CPU_SR_Restore(flags);	\
	} while (0)

#define spin_lock_irqsave(lock, flags)		\
	do {					\
		flags = OS_CPU_SR_Save();	\
		(void)lock;			\
	} while (0)
#define spin_unlock_irqrestore(lock, flags)	\
	do {					\
		OS_CPU_SR_Restore(flags);	\
	} while (0)


/* --------------------------------------------------------
 * completion:
 * -------------------------------------------------------- */
struct completion {
	OS_EVENT *com;
};

#define INIT_COMPLETION(x) do {		\
	u8 err = 0;			\
	OSSemSet((x).com, 0, &err);	\
} while (0)

static inline void init_completion(struct completion *x)
{
	x->com = OSSemCreate(0);
	if (!x->com)
		while (1);
}

static inline void complete(struct completion *x)
{
	OSSemPost(x->com);
}

long wait_for_completion_timeout(struct completion *x, unsigned long timeout);
bool wait_for_condition(volatile bool *x, unsigned long timeout);

/* -------------------------------------------------------- */
/* ---------------------- interrupt ----------------------- */
/* -------------------------------------------------------- */
typedef void irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(void *);

void OS_CSP_BSP_ExceptHandler(u8 exc);
int request_irq(u32 irq, irq_handler_t handler, void *arg);

static inline void free_irq(u32 irq)
{
	CSP_IntVectUnreg(0, (u16)irq);
}

#endif

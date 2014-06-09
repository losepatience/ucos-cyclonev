/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

/*HANDLER is defined in ucOS II*/
#include "ucos_ii.h"

typedef OS_EVENT			*HANDLER;
typedef HANDLER				sys_sem_t;
#define SYS_SEM_NULL			NULL
#define sys_sem_valid(sema)		((sema) != NULL)
#define sys_sem_set_invalid(sema)	((sema) == NULL)


/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX		1

typedef HANDLER sys_mbox_t;
#define SYS_MBOX_NULL NULL
#define sys_mbox_set_invalid(mbox)	(mbox == NULL)

static inline int sys_mbox_valid(sys_mbox_t *mbox)
{
	return (int)(*mbox);
}

#define T_LWIP_THREAD_START_PRIO	7
#define T_LWIP_THREAD_MAX_NB		1
#define T_LWIP_THREAD_STKSIZE		256
typedef u8_t				sys_thread_t;

#endif /* __ARCH_SYS_ARCH_H__ */


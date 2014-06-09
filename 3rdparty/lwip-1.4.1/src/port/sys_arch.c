#include <arch/cc.h>
#include <arch/sys_arch.h>
#include <lwip/sys.h>
#include <lwip/opt.h>
#include <lwip/def.h>
#include <string.h>

#define TRY_GET_BEST_QUEUE_ENTRIES

#define MAX_QUEUE_ENTRIES TCPIP_MBOX_SIZE
#define MAX_MBOX_ENTRIES   3

static void *msg_buffer[MAX_MBOX_ENTRIES][MAX_QUEUE_ENTRIES];
static u32_t free_msg_buffer_mask;
#ifdef TRY_GET_BEST_QUEUE_ENTRIES
static u32_t blocked_mbox_mask;
#endif
#ifdef PEND_FLAG_WHEN_POST_MSG
static OS_FLAG_GRP *flg_mbox_full;
#else
#define DELAY_TIME_WHEN_POST_MSG_FULL   100
#endif

OS_STK T_LWIP_THREAD_STK[T_LWIP_THREAD_MAX_NB][T_LWIP_THREAD_STKSIZE];

/* sys_init() must be called before anthing else. */
void sys_init(void)
{
	/* init mbox */
	memset(msg_buffer, 0, sizeof(msg_buffer));
	free_msg_buffer_mask = 0;
#ifdef TRY_GET_BEST_QUEUE_ENTRIES
	blocked_mbox_mask = 0;
#endif
#ifdef PEND_FLAG_WHEN_POST_MSG
	flg_mbox_full = OSFlagCreate(0, &err);
#endif
}

/** Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (miminum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
	u8_t index;

#if OS_CRITICAL_METHOD == 3                         /* Allocate storage for CPU status register        */
	OS_CPU_SR    cpu_sr = 0;
#endif

	if (size > MAX_QUEUE_ENTRIES)
		return ERR_MEM;

	OS_ENTER_CRITICAL();
	for (index = 0; index < MAX_MBOX_ENTRIES; index++) {
		if (!(free_msg_buffer_mask & (1 << index))) {
			free_msg_buffer_mask |= (1 << index);
			break;
		}
	}

	if (index < MAX_MBOX_ENTRIES)
		*mbox = OSQCreate(msg_buffer[index], MAX_QUEUE_ENTRIES);

	OS_EXIT_CRITICAL();

	if (index >= MAX_MBOX_ENTRIES || *mbox == 0)
		return ERR_MEM;

	return ERR_OK;
}

/** Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
	INT8U err;
#if OS_CRITICAL_METHOD == 3                         /* Allocate storage for CPU status register        */
	OS_CPU_SR    cpu_sr = 0;
#endif

	do {
		err = OSQPost(*mbox, msg);
		if(err != OS_ERR_Q_FULL)
			return;

#if defined(TRY_GET_BEST_QUEUE_ENTRIES) || defined(PEND_FLAG_WHEN_POST_MSG)
		OS_Q * pq;
		u8_t index;

		OS_ENTER_CRITICAL();
		pq = ((OS_Q *)((*mbox)->OSEventPtr));
		index = (pq->OSQStart - msg_buffer[0]) / (sizeof(void*) * MAX_QUEUE_ENTRIES);
#endif
#if defined(TRY_GET_BEST_QUEUE_ENTRIES)
		blocked_mbox_mask |= 1 << index;
#endif
#if defined(PEND_FLAG_WHEN_POST_MSG)
		if (pq->OSQEntries >= pq->OSQSize) {
			OSFlagPost (flg_mbox_full, (1 << index), OS_FLAG_SET, &err);
		}
#endif
#if defined(TRY_GET_BEST_QUEUE_ENTRIES) || defined(PEND_FLAG_WHEN_POST_MSG)
		OS_EXIT_CRITICAL();
#endif
#if defined(PEND_FLAG_WHEN_POST_MSG)
		OSFlagPend(flg_mbox_full, (1 << index), OS_FLAG_CLR, 0, &err);
#else
		OSTimeDly(DELAY_TIME_WHEN_POST_MSG_FULL);
#endif
	} while (1);
}

/** Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL) */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
	INT8U err;
	err = OSQPost (*mbox, msg);

	if(err == OS_ERR_NONE)
		return ERR_OK;
	else if(err == OS_ERR_Q_FULL)
		return ERR_MEM;
	else
		return ERR_VAL;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
 or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter! */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	INT8U err;
#if OS_CRITICAL_METHOD == 3		/* Allocate storage for CPU status register        */
	OS_CPU_SR   __attribute__((unused)) cpu_sr = 0;
#endif

	u32_t curTime = OSTimeGet();

	while (timeout > 0xFFFF) {
		timeout -= 0xFFFF;

		*msg = OSQPend(*mbox, 0xFFFF, &err);
		switch (err)
		{
		case OS_ERR_TIMEOUT:
			continue;
		case OS_ERR_NONE:
#if defined(PEND_FLAG_WHEN_POST_MSG)
		{
			void ** pStart;
			u8_t index;

			OS_ENTER_CRITICAL();
			pStart = ((OS_Q *)((*mbox)->OSEventPtr))->OSQStart;
			index = (pStart - msg_buffer[0]) / (sizeof(void*) * MAX_QUEUE_ENTRIES);
			OS_EXIT_CRITICAL();
			OSFlagPost (flg_mbox_full, (1 << index), OS_FLAG_CLR, &err);
		}
#endif
			return OSTimeGet () - curTime;
		default:
			return SYS_ARCH_TIMEOUT;
		}
	}

	*msg = OSQPend(*mbox, timeout, &err);
	if (err != OS_ERR_NONE)
		return SYS_ARCH_TIMEOUT;

#if defined(PEND_FLAG_WHEN_POST_MSG)
	void ** pStart;
	u8_t index;

	OS_ENTER_CRITICAL();
	pStart = ((OS_Q *)((*mbox)->OSEventPtr))->OSQStart;
	index = (pStart - msg_buffer[0]) / (sizeof(void*) * MAX_QUEUE_ENTRIES);
	OS_EXIT_CRITICAL();
	OSFlagPost(flg_mbox_full, (1 << index), OS_FLAG_CLR, &err);
#endif
	return OSTimeGet () - curTime;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	INT8U err;

	*msg = OSQAccept(*mbox, &err);
	return (err == OS_ERR_NONE) ? 0 : SYS_MBOX_EMPTY;
}

/** Delete an mbox
 * @param mbox mbox to delete */
void sys_mbox_free(sys_mbox_t *mbox)
{
	INT8U err;
	u8_t index;
	void ** pStart;
#if OS_CRITICAL_METHOD == 3                         /* Allocate storage for CPU status register        */
	OS_CPU_SR    cpu_sr = 0;
#endif

	OS_ENTER_CRITICAL();
	pStart = ((OS_Q *)((*mbox)->OSEventPtr))->OSQStart;
	index = (pStart - msg_buffer[0]) / (sizeof(void *) * MAX_QUEUE_ENTRIES);
	free_msg_buffer_mask &= ~(1<<index);
	OSQDel(*mbox, OS_DEL_NO_PEND, &err);
	OS_EXIT_CRITICAL();
}


/** Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	*sem = OSSemCreate(count);
	return (*sem != 0) ? ERR_OK : ERR_MEM;
}

/** Signals a semaphore
 * @param sem the semaphore to signal */
void sys_sem_signal(sys_sem_t *sem)
{
	OSSemPost (*sem);
}

/** Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SYS_ARCH_TIMEOUT on timeout */
/* cheney: though the above comment said ought to return the waited time.
   really, the caller only check if SYS_ARCH_TIMEOUT or not.
   so, we just return 0 when owner this sem. */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	INT8U err;

	while (timeout > 0xFFFF) {
		timeout -= 0xFFFF;
		OSSemPend(*sem, 0xFFFF, &err);

		switch(err) {

		case OS_ERR_TIMEOUT:
			continue;
		case OS_ERR_NONE:
			return 0;
		default:
			return SYS_ARCH_TIMEOUT;
		}
	}

	OSSemPend(*sem, timeout, &err);
	return (err == OS_ERR_NONE) ? 0 : SYS_ARCH_TIMEOUT;
}

/** Delete a semaphore
 * @param sem semaphore to delete */
void sys_sem_free(sys_sem_t *sem)
{
	INT8U err;
	OSSemDel(*sem, OS_DEL_ALWAYS, &err);
}

/** The only thread function:
 * Creates a new thread
 * @param name human-readable name for the thread (used for debugging purposes)
 * @param thread thread-function
 * @param arg parameter passed to 'thread'
 * @param stacksize stack size in bytes for the new thread (may be ignored by ports)
 * @param prio priority of the new thread (may be ignored by ports) */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	sys_thread_t id = 0;
	INT8U err;

	if (prio <= 0 || prio > T_LWIP_THREAD_MAX_NB)
		return 0;

	id = prio - 1 + T_LWIP_THREAD_START_PRIO;

	err = OSTaskCreate(thread, arg, (void *)&(T_LWIP_THREAD_STK[prio-1][T_LWIP_THREAD_STKSIZE - 1]), id);
	if (err == OS_ERR_NONE)
		OSTaskNameSet(id, (INT8U*)name, &err);

	return 0;
}

sys_prot_t sys_arch_protect(void)
{
	return OS_CPU_SR_Save();
}

void sys_arch_unprotect(sys_prot_t pval)
{
	OS_CPU_SR_Restore(pval);
}

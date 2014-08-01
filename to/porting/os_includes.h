#ifndef __OS_INCLUDES_H__
#define __OS_INCLUDES_H__

#include <ucos_ii.h>
#include <board.h>
#include <asm/types.h>
#include <old_apis.h>
#include <stddef.h>
#include <string.h>

#define __packed
#define OS_NO_ERR		OS_ERR_NONE
#define OS_TIMEOUT		OS_ERR_TIMEOUT
#define OS_TASK_NOT_EXIST	OS_ERR_TASK_NOT_EXIST

#endif

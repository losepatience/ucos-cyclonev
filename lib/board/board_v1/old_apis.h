#ifndef __OLD_APIS_H__
#define __OLD_APIS_H__

#include <asm/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <i2c-dev.h>

#define BOOL	bool
#define False	false
#define FALSE	false
#define True	true
#define TRUE	true

#define U32	u32
#define U8	u8
#define UBYTE	u8

#define TRACE_INFO(fmt, args...) pr_info(fmt, ##args)

#endif


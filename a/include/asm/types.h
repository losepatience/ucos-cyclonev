#ifndef __ASM_ARM_TYPES_H
#define __ASM_ARM_TYPES_H

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef unsigned int ulong;
typedef unsigned int uint;

typedef signed long long s64;
typedef unsigned long long u64;

typedef s8 int8_t ;
typedef u8 uint8_t;

typedef s16 int16_t;
typedef u16 uint16_t;

typedef s32 int32_t;
typedef u32 uint32_t;

#define BITS_PER_LONG 32


typedef unsigned int		__kernel_size_t;
typedef int			__kernel_ssize_t;
typedef int			__kernel_ptrdiff_t;
typedef char *			__kernel_caddr_t;

typedef long long		__kernel_loff_t;

/*
 * The following typedefs are also protected by individual ifdefs for
 * historical reasons:
 */
#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t		size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __kernel_ssize_t	ssize_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef __kernel_ptrdiff_t	ptrdiff_t;
#endif

#ifndef _CADDR_T
#define _CADDR_T
typedef __kernel_caddr_t	caddr_t;
#endif

#if defined(__GNUC__)
typedef __kernel_loff_t		loff_t;
#endif

#if defined(__GNUC__)
#  define __noreturn		__attribute__((noreturn))
#endif

#endif

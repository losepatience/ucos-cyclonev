#ifndef __ASM_ARM_IO_H
#define __ASM_ARM_IO_H

#include <asm/types.h>
#include <asm/compiler.h>

static inline void __raw_writeb(u8 val, volatile void *addr)
{
	asm volatile("strb %1, %0"
		     : "+Qo" (*(volatile u8 *)addr)
		     : "r" (val));
}

static inline void __raw_writew(u16 val, volatile void *addr)
{
	asm volatile("strh %1, %0"
		     : "+Q" (*(volatile u16 *)addr)
		     : "r" (val));
}

static inline void __raw_writel(u32 val, volatile void *addr)
{
	asm volatile("str %1, %0"
		     : "+Qo" (*(volatile u32 *)addr)
		     : "r" (val));
}

static inline u8 __raw_readb(const volatile void *addr)
{
	u8 val;
	asm volatile("ldrb %1, %0"
		     : "+Qo" (*(volatile u8 *)addr),
		       "=r" (val));
	return val;
}

static inline u16 __raw_readw(const volatile void *addr)
{
	u16 val;
	asm volatile("ldrh %1, %0"
		     : "+Q" (*(volatile u16 *)addr),
		       "=r" (val));
	return val;
}

static inline u32 __raw_readl(const volatile void *addr)
{
	u32 val;
	asm volatile("ldr %1, %0"
		     : "+Qo" (*(volatile u32 *)addr),
		       "=r" (val));
	return val;
}

/*
 * TODO: The kernel offers some more advanced versions of barriers, it might
 * have some advantages to use them instead of the simple one here.
 */
#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()

#define outb(v,p)	({ __iowmb(); __raw_writeb(v,p); })
#define outw(v,p)	({ __iowmb(); __raw_writew((__u16)cpu_to_le16(v),p); })
#define outl(v,p)	({ __iowmb(); __raw_writel((__u32)cpu_to_le32(v),p); })

#define inb(p)	({ __u8 __v = __raw_readb(p); __iormb(); __v; })
#define inw(p)	({ __u16 __v = le16_to_cpu((__u16) \
			__raw_readw(p)); __iormb(); __v; })
#define inl(p)	({ __u32 __v = le32_to_cpu((__u32) \
			__raw_readl(p)); __iormb(); __v; })


#define outb_p(val,port)	outb((val),(port))
#define outw_p(val,port)	outw((val),(port))
#define outl_p(val,port)	outl((val),(port))
#define inb_p(port)		inb((port))
#define inw_p(port)		inw((port))
#define inl_p(port)		inl((port))

#ifndef readl
#define readb_relaxed(c) ({ u8  __r = __raw_readb(c); __r; })
#define readw_relaxed(c) ({ u16 __r = le16_to_cpu((__u16) \
					__raw_readw(c)); __r; })
#define readl_relaxed(c) ({ u32 __r = le32_to_cpu((__u32) \
					__raw_readl(c)); __r; })

#define writeb_relaxed(v,c)	__raw_writeb(v,c)
#define writew_relaxed(v,c)	__raw_writew((u16) cpu_to_le16(v),c)
#define writel_relaxed(v,c)	__raw_writel((u32) cpu_to_le32(v),c)

#define readb(c)		({ u8  __v = readb_relaxed(c); __iormb(); __v; })
#define readw(c)		({ u16 __v = readw_relaxed(c); __iormb(); __v; })
#define readl(c)		({ u32 __v = readl_relaxed(c); __iormb(); __v; })

#define writeb(v,c)		({ __iowmb(); writeb_relaxed(v,c); })
#define writew(v,c)		({ __iowmb(); writew_relaxed(v,c); })
#define writel(v,c)		({ __iowmb(); writel_relaxed(v,c); })
#endif	/* readl */

/* access ports */
#define setbits(p, v) outl(inl(p) | (v), p)
#define clrbits(p, v) outl(inl(p) & ~(v), p)
#define clrsetbits(addr, clr, set) outl((addr), (inl(addr) & ~(clr)) | (set))

#endif	/* __ASM_ARM_IO_H */

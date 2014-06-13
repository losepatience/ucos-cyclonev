#include <asm/io.h>
#include <asm/regs.h>
#include <malloc.h>
#include "cache.h"
#include "mm.h"

static volatile unsigned int __OS1_awake;
extern unsigned int __OS1_exceptions;
extern unsigned int __OS1_exceptions_end;
extern int main(void);

static void wakeup_OS1(void)
{
	unsigned int *v = (unsigned int *)0;
	unsigned int *p = &__OS1_exceptions;
	unsigned int b[&__OS1_exceptions_end - &__OS1_exceptions];
	int i;

	/* copy interrupt vectors of core1 to core1 reset address(0) */
	for (i = 0; &p[i] < &__OS1_exceptions_end; i++) {
		b[i] = v[i];
		v[i] = p[i];
	}

	/* Reset core1, it will execute the command in address 0 */
	*((volatile unsigned int *)0xFFD05010) = 0;

	/* Wait for core1 to wake up */
	while (__OS1_awake == 0) {
		/* XXX flush __OS1_awake for cache coherence between 2 cores */
		__asm__ __volatile__ ("" : : : "memory");
	}

	/* XXX warning: you would better not put ether image in address 0x0 */
	for (i = 0; &p[i] < &__OS1_exceptions_end; i++)
		v[i] = b[i];
}

/* this is called in start.S when OS1 starts to run */
void __OS1_reset(void)
{
	__OS1_awake = 0xa5a5a5a5;

	/* TODO */
	/* jump to OS1 */
	/* int (*jump_2_OS1)(void) = 0xabcddcba */
	/* (*jump_2_OS1)(); */
	while (1)
		;
}

void lowlevel_init(void)
{
	/* remap the SDRAM at lower memory instead on-chip RAM */
	writel(0x1, (void *)0xfffefC00);

	mem_malloc_init(0x1000000, 0x1000000);
	__enable_cache();
	pl310_init(0U, ~0UL);

	wakeup_OS1();

	main();
}


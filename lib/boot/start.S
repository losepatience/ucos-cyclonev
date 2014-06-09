#define Mode_USR 0x10
#define Mode_FIQ 0x11
#define Mode_IRQ 0x12
#define Mode_SVC 0x13
#define Mode_ABT 0x17
#define Mode_UND 0x1B
#define Mode_SYS 0x1F

#define I_Bit    0x80
#define F_Bit    0x40

	.section	.stext, "ax"

.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
        nop     @ Reserved for secure montor calls
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:	.word OS_CPU_ARM_ExceptUndefInstrHndlr
_software_interrupt:	.word OS_CPU_ARM_ExceptSwiHndlr
_prefetch_abort:	.word OS_CPU_ARM_ExceptPrefetchAbortHndlr
_data_abort:		.word OS_CPU_ARM_ExceptDataAbortHndlr
_not_used:		.word _not_used
_irq:			.word OS_CPU_ARM_ExceptIrqHndlr
_fiq:			.word OS_CPU_ARM_ExceptFiqHndlr
_pad:			.word 0x12345678 /* now 16*4=64 */

reset:

	@ set the cpu to SVC32 mode
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

	@ disable watchdog
	ldr	r0, =0xffd05014
	ldr	r1, [r0]
	add	r1, r1, #0xc0
	str	r1, [r0]

	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	bic	r0, r0, #0x00000002	@ clear bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0
	dsb
	isb

	@ set cp10, cp11 privileged and user mode access
        mrc     p15, 0, r0, c1, c0, 2
        orr     r0, r0, #(0x3 << 20)
        orr     r0, r0, #(0x3 << 22)
        mcr     p15, 0, r0, c1, c0, 2
        isb

        mov     r0, #(0x1 << 30)
	mcr	p10, 7, r0, cr8, cr0, 0	@ enable FPU

	@ set vbar and mvbar
	adr	r0, _start
	mcr	p15, 0, r0, c12, c0, 0	@ vbar
	mcr	p15, 0, r0, c12, c0, 1	@ mvbar

setup_stack:
	ldr	sp, =usr_stack_end

	msr	cpsr_c, #Mode_IRQ | I_Bit | F_Bit
	ldr	sp, =irq_stack_end
        msr	cpsr_c, #Mode_SVC | I_Bit | F_Bit
	/*XXX dont do anything that needs a stack in the other modes */

	@ enable i,d cache
	bl	__enable_cache
	bl	__enable_l2_cache

clear_bss:
	ldr	r0, =__bss_start	@ find start of bss segment
	ldr	r1, =__bss_end		@ stop here
	mov	r2, #0x0		@ clear value

clbss_l:
	str	r2, [r0]		@ clear BSS location
	cmp	r0, r1			@ are we at the end yet

	add	r0, r0, #4		@ increment clear index pointer
	bne	clbss_l			@ keep clearing till at end
clbss_e:


jump_2_C:
	@ldr	pc, =lowlevel_init	@ jump to OS entry
	ldr	pc, =main		@ jump to OS entry
	nop
	nop

/*----------------------------------------------------------------------------*/
/*
 *************************************************************************
 *
 * CPU1 exception vector table
 *
 *************************************************************************
 */
	.align 5
.globl __OS1_exceptions
__OS1_exceptions:
	.rept	8
	ldr	pc, [pc, #24]	@ reset
	.endr

	.word	__OS1_reset
	.rept	7
	b	.		@ infinite loop
	.endr
.globl __OS1_exceptions_end
__OS1_exceptions_end:


/*----------------------------------------------------------------------------*/
	.data
	.align 5
fiq_stack_start:
	.space 1024
fiq_stack_end:
	.space 1024
irq_stack_end:
	.space 4096
usr_stack_end:

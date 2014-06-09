#ifndef __LINKAGE_H__
#define __LINKAGE_H__

#ifdef __ASSEMBLY__

#define ALIGN	.align 4

#ifndef ENTRY
# define ENTRY(name) \
	.globl name; \
	ALIGN; \
	name:
#endif

#ifndef END
# define END(name) \
	.size name, .-name
#endif

#ifndef ENDPROC
# define ENDPROC(name) \
	.type name, %function; \
	END(name)
#endif

#ifndef WEAK
# define WEAK(name)	   \
	.weak name;	   \
	name:
#endif

#endif	/* __ASSEMBLY__ */
#endif	/* __LINKAGE_H__ */

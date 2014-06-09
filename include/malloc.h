#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <asm/types.h>	/* for size_t */

#define Void_t      void

#ifdef __cplusplus
extern "C" {
#endif



/*
  INTERNAL_SIZE_T is the word-size used for internal bookkeeping
  of chunk sizes. On a 64-bit machine, you can reduce malloc
  overhead by defining INTERNAL_SIZE_T to be a 32 bit `unsigned int'
  at the expense of not being able to handle requests greater than
  2^31. This limitation is hardly ever a concern; you are encouraged
  to set this. However, the default version is the same as size_t.
*/
#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif

/*
  HAVE_MEMCPY should be defined if you are not otherwise using
  ANSI STD C, but still have memcpy and memset in your C library
  and want to use them in calloc and realloc. Otherwise simple
  macro versions are defined here.

  USE_MEMCPY should be defined as 1 if you actually want to
  have memset and memcpy called. People report that the macro
  versions are often enough faster than libc versions on many
  systems that it is better to use them.

*/
#define HAVE_MEMCPY

#ifndef USE_MEMCPY
#ifdef HAVE_MEMCPY
#define USE_MEMCPY 1
#else
#define USE_MEMCPY 0
#endif
#endif


void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);

#if USE_MEMCPY

/* The following macros are only invoked with (2n+1)-multiples of
   INTERNAL_SIZE_T units, with a positive integer n. This is exploited
   for fast inline execution when n is small. */

#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T mzsz = (nbytes);                                            \
  if(mzsz <= 9*sizeof(mzsz)) {                                                \
    INTERNAL_SIZE_T* mz = (INTERNAL_SIZE_T*) (charp);                         \
    if(mzsz >= 5*sizeof(mzsz)) {     *mz++ = 0;                               \
				     *mz++ = 0;                               \
      if(mzsz >= 7*sizeof(mzsz)) {   *mz++ = 0;                               \
				     *mz++ = 0;                               \
	if(mzsz >= 9*sizeof(mzsz)) { *mz++ = 0;                               \
				     *mz++ = 0; }}}                           \
				     *mz++ = 0;                               \
				     *mz++ = 0;                               \
				     *mz   = 0;                               \
  } else memset((charp), 0, mzsz);                                            \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  INTERNAL_SIZE_T mcsz = (nbytes);                                            \
  if(mcsz <= 9*sizeof(mcsz)) {                                                \
    INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) (src);                        \
    INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) (dest);                       \
    if(mcsz >= 5*sizeof(mcsz)) {     *mcdst++ = *mcsrc++;                     \
				     *mcdst++ = *mcsrc++;                     \
      if(mcsz >= 7*sizeof(mcsz)) {   *mcdst++ = *mcsrc++;                     \
				     *mcdst++ = *mcsrc++;                     \
	if(mcsz >= 9*sizeof(mcsz)) { *mcdst++ = *mcsrc++;                     \
				     *mcdst++ = *mcsrc++; }}}                 \
				     *mcdst++ = *mcsrc++;                     \
				     *mcdst++ = *mcsrc++;                     \
				     *mcdst   = *mcsrc  ;                     \
  } else memcpy(dest, src, mcsz);                                             \
} while(0)

#else /* !USE_MEMCPY */

/* Use Duff's device for good zeroing/copying performance. */

#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T* mzp = (INTERNAL_SIZE_T*)(charp);                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mzp++ = 0;                                             \
    case 7:           *mzp++ = 0;                                             \
    case 6:           *mzp++ = 0;                                             \
    case 5:           *mzp++ = 0;                                             \
    case 4:           *mzp++ = 0;                                             \
    case 3:           *mzp++ = 0;                                             \
    case 2:           *mzp++ = 0;                                             \
    case 1:           *mzp++ = 0; if(mcn <= 0) break; mcn--; }                \
  }                                                                           \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) src;                            \
  INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) dest;                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mcdst++ = *mcsrc++;                                    \
    case 7:           *mcdst++ = *mcsrc++;                                    \
    case 6:           *mcdst++ = *mcsrc++;                                    \
    case 5:           *mcdst++ = *mcsrc++;                                    \
    case 4:           *mcdst++ = *mcsrc++;                                    \
    case 3:           *mcdst++ = *mcsrc++;                                    \
    case 2:           *mcdst++ = *mcsrc++;                                    \
    case 1:           *mcdst++ = *mcsrc++; if(mcn <= 0) break; mcn--; }       \
  }                                                                           \
} while(0)

#endif


#undef	HAVE_MMAP
#undef	HAVE_MREMAP

/*
  Access to system page size. To the extent possible, this malloc
  manages memory from the system in page-size units.

  The following mechanics for getpagesize were adapted from
  bsd/gnu getpagesize.h
*/

#define	LACKS_UNISTD_H	/* Shortcut for U-Boot */
#define	malloc_getpagesize	4096

#ifndef LACKS_UNISTD_H
#  include <unistd.h>
#endif

#ifndef malloc_getpagesize
#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif
#  ifdef _SC_PAGE_SIZE
#    define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define malloc_getpagesize getpagesize()
#    else
#      ifdef WIN32
#        define malloc_getpagesize (4096) /* TBD: Use 'GetSystemInfo' instead */
#      else
#        ifndef LACKS_SYS_PARAM_H
#          include <sys/param.h>
#        endif
#        ifdef EXEC_PAGESIZE
#          define malloc_getpagesize EXEC_PAGESIZE
#        else
#          ifdef NBPG
#            ifndef CLSIZE
#              define malloc_getpagesize NBPG
#            else
#              define malloc_getpagesize (NBPG * CLSIZE)
#            endif
#          else
#            ifdef NBPC
#              define malloc_getpagesize NBPC
#            else
#              ifdef PAGESIZE
#                define malloc_getpagesize PAGESIZE
#              else
#                define malloc_getpagesize (4096) /* just guess */
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif


/*

  This version of malloc supports the standard SVID/XPG mallinfo
  routine that returns a struct containing the same kind of
  information you can get from malloc_stats. It should work on
  any SVID/XPG compliant system that has a /usr/include/malloc.h
  defining struct mallinfo. (If you'd like to install such a thing
  yourself, cut out the preliminary declarations as described above
  and below and save them in a malloc.h file. But there's no
  compelling reason to bother to do this.)

  The main declaration needed is the mallinfo struct that is returned
  (by-copy) by mallinfo().  The SVID/XPG malloinfo struct contains a
  bunch of fields, most of which are not even meaningful in this
  version of malloc. Some of these fields are are instead filled by
  mallinfo() with other numbers that might possibly be of interest.

  HAVE_USR_INCLUDE_MALLOC_H should be set if you have a
  /usr/include/malloc.h file that includes a declaration of struct
  mallinfo.  If so, it is included; else an SVID2/XPG2 compliant
  version is declared below.  These must be precisely the same for
  mallinfo() to work.

*/

/* #define HAVE_USR_INCLUDE_MALLOC_H */

#ifdef HAVE_USR_INCLUDE_MALLOC_H
#include "/usr/include/malloc.h"
#else

/* SVID2/XPG mallinfo structure */

struct mallinfo {
  int arena;    /* total space allocated from system */
  int ordblks;  /* number of non-inuse chunks */
  int smblks;   /* unused -- always zero */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* total space in mmapped regions */
  int usmblks;  /* unused -- always zero */
  int fsmblks;  /* unused -- always zero */
  int uordblks; /* total allocated space */
  int fordblks; /* total non-inuse space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};

/* SVID2/XPG mallopt options */

#define M_MXFAST  1    /* UNUSED in this malloc */
#define M_NLBLKS  2    /* UNUSED in this malloc */
#define M_GRAIN   3    /* UNUSED in this malloc */
#define M_KEEP    4    /* UNUSED in this malloc */

#endif

/* mallopt options that actually do something */

#define M_TRIM_THRESHOLD    -1
#define M_TOP_PAD           -2
#define M_MMAP_THRESHOLD    -3
#define M_MMAP_MAX          -4


#ifndef DEFAULT_TRIM_THRESHOLD
#define DEFAULT_TRIM_THRESHOLD (128 * 1024)
#endif

/*
    M_TRIM_THRESHOLD is the maximum amount of unused top-most memory
      to keep before releasing via malloc_trim in free().

      Automatic trimming is mainly useful in long-lived programs.
      Because trimming via sbrk can be slow on some systems, and can
      sometimes be wasteful (in cases where programs immediately
      afterward allocate more large chunks) the value should be high
      enough so that your overall system performance would improve by
      releasing.

      The trim threshold and the mmap control parameters (see below)
      can be traded off with one another. Trimming and mmapping are
      two different ways of releasing unused memory back to the
      system. Between these two, it is often possible to keep
      system-level demands of a long-lived program down to a bare
      minimum. For example, in one test suite of sessions measuring
      the XF86 X server on Linux, using a trim threshold of 128K and a
      mmap threshold of 192K led to near-minimal long term resource
      consumption.

      If you are using this malloc in a long-lived program, it should
      pay to experiment with these values.  As a rough guide, you
      might set to a value close to the average size of a process
      (program) running on your system.  Releasing this much memory
      would allow such a process to run in memory.  Generally, it's
      worth it to tune for trimming rather tham memory mapping when a
      program undergoes phases where several large chunks are
      allocated and released in ways that can reuse each other's
      storage, perhaps mixed with phases where there are no such
      chunks at all.  And in well-behaved long-lived programs,
      controlling release of large blocks via trimming versus mapping
      is usually faster.

      However, in most programs, these parameters serve mainly as
      protection against the system-level effects of carrying around
      massive amounts of unneeded memory. Since frequent calls to
      sbrk, mmap, and munmap otherwise degrade performance, the default
      parameters are set to relatively high values that serve only as
      safeguards.

      The default trim value is high enough to cause trimming only in
      fairly extreme (by current memory consumption standards) cases.
      It must be greater than page size to have any useful effect.  To
      disable trimming completely, you can set to (unsigned long)(-1);


*/


#ifndef DEFAULT_TOP_PAD
#define DEFAULT_TOP_PAD        (0)
#endif

/*
    M_TOP_PAD is the amount of extra `padding' space to allocate or
      retain whenever sbrk is called. It is used in two ways internally:

      * When sbrk is called to extend the top of the arena to satisfy
	a new malloc request, this much padding is added to the sbrk
	request.

      * When malloc_trim is called automatically from free(),
	it is used as the `pad' argument.

      In both cases, the actual amount of padding is rounded
      so that the end of the arena is always a system page boundary.

      The main reason for using padding is to avoid calling sbrk so
      often. Having even a small pad greatly reduces the likelihood
      that nearly every malloc request during program start-up (or
      after trimming) will invoke sbrk, which needlessly wastes
      time.

      Automatic rounding-up to page-size units is normally sufficient
      to avoid measurable overhead, so the default is 0.  However, in
      systems where sbrk is relatively slow, it can pay to increase
      this value, at the expense of carrying around more memory than
      the program needs.

*/


#ifndef DEFAULT_MMAP_THRESHOLD
#define DEFAULT_MMAP_THRESHOLD (128 * 1024)
#endif

/*

    M_MMAP_THRESHOLD is the request size threshold for using mmap()
      to service a request. Requests of at least this size that cannot
      be allocated using already-existing space will be serviced via mmap.
      (If enough normal freed space already exists it is used instead.)

      Using mmap segregates relatively large chunks of memory so that
      they can be individually obtained and released from the host
      system. A request serviced through mmap is never reused by any
      other request (at least not directly; the system may just so
      happen to remap successive requests to the same locations).

      Segregating space in this way has the benefit that mmapped space
      can ALWAYS be individually released back to the system, which
      helps keep the system level memory demands of a long-lived
      program low. Mapped memory can never become `locked' between
      other chunks, as can happen with normally allocated chunks, which
      menas that even trimming via malloc_trim would not release them.

      However, it has the disadvantages that:

	 1. The space cannot be reclaimed, consolidated, and then
	    used to service later requests, as happens with normal chunks.
	 2. It can lead to more wastage because of mmap page alignment
	    requirements
	 3. It causes malloc performance to be more dependent on host
	    system memory management support routines which may vary in
	    implementation quality and may impose arbitrary
	    limitations. Generally, servicing a request via normal
	    malloc steps is faster than going through a system's mmap.

      All together, these considerations should lead you to use mmap
      only for relatively large requests.


*/


#ifndef DEFAULT_MMAP_MAX
#ifdef HAVE_MMAP
#define DEFAULT_MMAP_MAX       (64)
#else
#define DEFAULT_MMAP_MAX       (0)
#endif
#endif

/*
    M_MMAP_MAX is the maximum number of requests to simultaneously
      service using mmap. This parameter exists because:

	 1. Some systems have a limited number of internal tables for
	    use by mmap.
	 2. In most systems, overreliance on mmap can degrade overall
	    performance.
	 3. If a program allocates many large regions, it is probably
	    better off using normal sbrk-based allocation routines that
	    can reclaim and reallocate normal heap memory. Using a
	    small value allows transition into this mode after the
	    first few allocations.

      Setting to 0 disables all use of mmap.  If HAVE_MMAP is not set,
      the default value is 0, and attempts to set it to non-zero values
      in mallopt will fail.
*/


/*
    USE_DL_PREFIX will prefix all public routines with the string 'dl'.
      Useful to quickly avoid procedure declaration conflicts and linker
      symbol conflicts with existing memory allocation routines.

*/

/* #define USE_DL_PREFIX */


/*

  Special defines for linux libc

  Except when compiled using these special defines for Linux libc
  using weak aliases, this malloc is NOT designed to work in
  multithreaded applications.  No semaphores or other concurrency
  control are provided to ensure that multiple malloc or free calls
  don't run at the same time, which could be disasterous. A single
  semaphore could be used across malloc, realloc, and free (which is
  essentially the effect of the linux weak alias approach). It would
  be hard to obtain finer granularity.

*/

#ifdef INTERNAL_LINUX_C_LIB

Void_t * __default_morecore_init (ptrdiff_t);
Void_t *(*__morecore)(ptrdiff_t) = __default_morecore_init;

#define MORECORE (*__morecore)
#define MORECORE_FAILURE 0
#define MORECORE_CLEARS 1

#else /* INTERNAL_LINUX_C_LIB */

extern Void_t*     sbrk(ptrdiff_t);

#ifndef MORECORE
#define MORECORE sbrk
#endif

#ifndef MORECORE_FAILURE
#define MORECORE_FAILURE -1
#endif

#ifndef MORECORE_CLEARS
#define MORECORE_CLEARS 1
#endif

#endif /* INTERNAL_LINUX_C_LIB */

#if defined(INTERNAL_LINUX_C_LIB) && defined(__ELF__)

#define cALLOc		__libc_calloc
#define fREe		__libc_free
#define mALLOc		__libc_malloc
#define mEMALIGn	__libc_memalign
#define rEALLOc		__libc_realloc
#define vALLOc		__libc_valloc
#define pvALLOc		__libc_pvalloc
#define mALLINFo	__libc_mallinfo
#define mALLOPt		__libc_mallopt

#pragma weak calloc = __libc_calloc
#pragma weak free = __libc_free
#pragma weak cfree = __libc_free
#pragma weak malloc = __libc_malloc
#pragma weak memalign = __libc_memalign
#pragma weak realloc = __libc_realloc
#pragma weak valloc = __libc_valloc
#pragma weak pvalloc = __libc_pvalloc
#pragma weak mallinfo = __libc_mallinfo
#pragma weak mallopt = __libc_mallopt

#else

#ifdef USE_DL_PREFIX
#define cALLOc		dlcalloc
#define fREe		dlfree
#define mALLOc		dlmalloc
#define mEMALIGn	dlmemalign
#define rEALLOc		dlrealloc
#define vALLOc		dlvalloc
#define pvALLOc		dlpvalloc
#define mALLINFo	dlmallinfo
#define mALLOPt		dlmallopt
#else /* USE_DL_PREFIX */
#define cALLOc		calloc
#define fREe		free
#define mALLOc		malloc
#define mEMALIGn	memalign
#define rEALLOc		realloc
#define vALLOc		valloc
#define pvALLOc		pvalloc
#define mALLINFo	mallinfo
#define mALLOPt		mallopt
#endif /* USE_DL_PREFIX */

#endif

/* Public routines */
Void_t* mALLOc(size_t);
void    fREe(Void_t*);
Void_t* rEALLOc(Void_t*, size_t);
Void_t* mEMALIGn(size_t, size_t);
Void_t* vALLOc(size_t);
Void_t* pvALLOc(size_t);
Void_t* cALLOc(size_t, size_t);
void    cfree(Void_t*);
int     malloc_trim(size_t);
size_t  malloc_usable_size(Void_t*);
void    malloc_stats(void);
int     mALLOPt(int, int);
struct mallinfo mALLINFo(void);

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
extern ulong mem_malloc_start;
extern ulong mem_malloc_end;
extern ulong mem_malloc_brk;

void mem_malloc_init(ulong start, ulong size);
void malloc_bin_reloc(void);

#ifdef __cplusplus
};  /* end of extern "C" */
#endif

#endif /* __MALLOC_H__ */

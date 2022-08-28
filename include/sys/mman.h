/* sys/mman.h
*/

#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include <stddef.h>
#include <sys/types.h>

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_FILE 0
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_TYPE 0xF
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
/* Non-standard flag */
#define MAP_NORESERVE 0x4000	/* Don't reserve swap space for this mapping.
				   Page protection must be set explicitely
				   to access page. Only supported for anonymous
				   private mappings. */
#define MAP_AUTOGROW 0x8000	/* Grow underlying object to mapping size.
				   File must be opened for writing. */

#define MAP_FAILED ((void *)-1)

/* Flags for msync. */
#define MS_ASYNC 1
#define MS_INVALIDATE 2
#define MS_SYNC 4


/* Flags for posix_madvise. */
#define POSIX_MADV_NORMAL 0	/* no further special treatment */
#define POSIX_MADV_RANDOM 1	/* expect random page references */
#define POSIX_MADV_SEQUENTIAL 2	/* expect sequential page references */
#define POSIX_MADV_WILLNEED 3	/* will need these pages */
#define POSIX_MADV_DONTNEED 4	/* don't need these pages */

/* Flags for madvise. BSD/Linux-specific. */
#define MADV_NORMAL 0		/* no further special treatment */
#define MADV_RANDOM 1		/* expect random page references */
#define MADV_SEQUENTIAL 2	/* expect sequential page references */
#define MADV_WILLNEED 3		/* will need these pages */
#define MADV_DONTNEED 4		/* don't need these pages */

/* common parameters: try to keep these consistent across architectures */
#define MADV_FREE 8		/* free pages only if memory pressure */
#define MADV_REMOVE 9		/* remove these pages & resources */
#define MADV_DONTFORK 10	/* don't inherit across fork */
#define MADV_DOFORK 11		/* do inherit across fork */
#define MADV_HWPOISON 100	/* poison a page for testing */
#define MADV_SOFT_OFFLINE 101	/* soft offline page for testing */

#define MADV_MERGEABLE 12	/* KSM may merge identical pages */
#define MADV_UNMERGEABLE 13	/* KSM may not merge identical pages */

#define MADV_HUGEPAGE 14	/* Worth backing with hugepages */
#define MADV_NOHUGEPAGE 15	/* Not worth backing with hugepages */

#define MADV_DONTDUMP 16	/* Explicity exclude from the core dump,
				   overrides the coredump filter bits */
#define MADV_DODUMP 17		/* Clear the MADV_DONTDUMP flag */

#define MADV_WIPEONFORK 18	/* Zero memory on fork, child only */
#define MADV_KEEPONFORK 19	/* Undo MADV_WIPEONFORK */

#define MADV_COLD 20		/* deactivate these pages */
#define MADV_PAGEOUT 21		/* reclaim these pages */

#define MADV_POPULATE_READ 22	/* populate (prefault) page tables readable */
#define MADV_POPULATE_WRITE 23	/* populate (prefault) page tables writable */

#define MADV_DONTNEED_LOCKED 24	/* like DONTNEED, but drop locked pages too */

  extern void *mmap (void *__addr, size_t __len, int __prot, int __flags,
		     int __fd, off_t __off);
  extern int munmap (void *__addr, size_t __len);
  extern int mprotect (void *__addr, size_t __len, int __prot);
  extern int msync (void *__addr, size_t __len, int __flags);
  extern int mlock (const void *__addr, size_t __len);
  extern int munlock (const void *__addr, size_t __len);

  extern int posix_madvise (void *__addr, size_t __len, int __advice);
  extern int madvise (void *__addr, size_t __len, int __advice);

  extern int shm_open (const char *__name, int __oflag, mode_t __mode);
  extern int shm_unlink (const char *__name);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _SYS_MMAN_H_ */

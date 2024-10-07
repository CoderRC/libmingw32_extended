#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_getpgroup (const posix_spawnattr_t * __restrict __attr,
			   pid_t * __restrict __pgroup)
{
  if (!__attr || !__pgroup)
    {
      return EINVAL;
    }
  *__pgroup = __attr->__pgrp;
  return 0;
}

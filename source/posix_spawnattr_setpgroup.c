#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_setpgroup (posix_spawnattr_t * __attr, pid_t __pgroup)
{
  if (!__attr)
    {
      return EINVAL;
    }
  __attr->__pgrp = __pgroup;
  return 0;
}

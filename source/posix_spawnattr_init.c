#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_init (posix_spawnattr_t * __attr)
{
  if (!__attr)
    {
      return EINVAL;
    }
  __attr->__flags = 0;
  __attr->__pgrp = 0;
  __attr->__sd = 0;
  __attr->__ss = 0;
  __attr->__sp.sched_priority = 0;
  __attr->__policy = 0;
  return 0;
}

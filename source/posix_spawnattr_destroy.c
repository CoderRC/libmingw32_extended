#include <spawn.h>
#include <errno.h>
#include <stdlib.h>

int
posix_spawnattr_destroy (posix_spawnattr_t * __attr)
{
  if (!__attr)
    {
      return EINVAL;
    }
  if (__attr->__sd)
    {
      free (__attr->__sd);
      __attr->__sd = 0;
    }
  if (__attr->__ss)
    {
      free (__attr->__ss);
      __attr->__ss = 0;
    }
  __attr->__flags = 0;
  __attr->__pgrp = 0;
  __attr->__sp.sched_priority = 0;
  __attr->__policy = 0;
  return 0;
}

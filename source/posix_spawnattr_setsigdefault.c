#include <spawn.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int
posix_spawnattr_setsigdefault (posix_spawnattr_t * __restrict __attr,
			       const void *__restrict __sigdefault)
{
  if (!__attr || !__sigdefault)
    {
      return EINVAL;
    }
  if (__attr->__sd)
    {
      free (__attr->__sd);
    }
  __attr->__sd = malloc (sizeof (__sigdefault));
  if (!__attr->__sd)
    {
      return ENOMEM;
    }
  memcpy (__attr->__sd, __sigdefault, sizeof (__sigdefault));
  return 0;
}

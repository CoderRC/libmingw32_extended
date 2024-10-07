#include <spawn.h>
#include <errno.h>
#include <string.h>

int
posix_spawnattr_getsigdefault (const posix_spawnattr_t *
			       __restrict __attr,
			       void *__restrict __sigdefault)
{
  if (!__attr || !__sigdefault)
    {
      return EINVAL;
    }
  if (__attr->__sd)
    {
      memcpy (__sigdefault, __attr->__sd, sizeof (__attr->__sd));
    }
  else
    {
      memset (__sigdefault, 0, sizeof (__attr->__sd));
    }
  return 0;
}

#include <spawn.h>
#include <errno.h>
#include <string.h>

int
posix_spawnattr_getsigmask (const posix_spawnattr_t * __restrict __attr,
			    void *__restrict __sigmask)
{
  if (!__attr || !__sigmask)
    {
      return EINVAL;
    }
  if (__attr->__sd)
    {
      memcpy (__sigmask, __attr->__sd, sizeof (__attr->__sd));
    }
  else
    {
      memset (__sigmask, 0, sizeof (__attr->__sd));
    }
  return 0;
}

#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_getflags (const posix_spawnattr_t * __restrict __attr,
			  short int *__restrict __flags)
{
  if (!__attr || !__flags)
    {
      return EINVAL;
    }
  *__flags = __attr->__flags;
  return 0;
}

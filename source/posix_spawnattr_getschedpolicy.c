#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_getschedpolicy (const posix_spawnattr_t * __restrict __attr,
				int *__restrict __schedpolicy)
{
  if (!__attr || !__schedpolicy)
    {
      return EINVAL;
    }
  *__schedpolicy = __attr->__policy;
  return 0;
}

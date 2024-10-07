#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_getschedparam (const posix_spawnattr_t * __restrict __attr,
			       struct sched_param *__restrict __schedparam)
{
  if (!__attr || !__schedparam)
    {
      return EINVAL;
    }
  *__schedparam = __attr->__sp;
  return 0;
}

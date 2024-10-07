#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_setschedpolicy (posix_spawnattr_t * __attr, int __schedpolicy)
{
  if (!__attr)
    {
      return EINVAL;
    }
  __attr->__policy = __schedpolicy;
  return 0;
}

#include <spawn.h>
#include <errno.h>

int
posix_spawnattr_setflags (posix_spawnattr_t * __attr, short int __flags)
{
  if (!__attr)
    {
      return EINVAL;
    }
  __attr->__flags = __flags;
  return 0;
}

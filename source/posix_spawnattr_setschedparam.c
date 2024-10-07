#include <errno.h>
#include "spawn.h"

/* Set the scheduling parameters in a posix_spawnattr_t structure */
int
posix_spawnattr_setschedparam (posix_spawnattr_t * __restrict __attr,
			       const struct sched_param *__restrict
			       __schedparam)
{
  if (!__attr || !__schedparam)
    {
      return EINVAL;		// Invalid argument
    }

  // Set the scheduling parameters in the __attr structure
  __attr->__sp = *__schedparam;

  return 0;			// Success
}

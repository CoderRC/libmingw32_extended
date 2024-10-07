#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "spawn.h"

/* Set the signal mask in a posix_spawnattr_t structure */
int
posix_spawnattr_setsigmask (posix_spawnattr_t * __restrict __attr,
			    const void *__restrict __sigmask)
{
  if (!__attr || !__sigmask)
    {
      return EINVAL;		// Invalid argument
    }

  // Free any previously allocated signal mask
  if (__attr->__sd)
    {
      free (__attr->__sd);
    }

  // Allocate memory for the new signal mask
  __attr->__sd = malloc (sizeof (__sigmask));	// Size depends on the signal mask type
  if (!__attr->__sd)
    {
      return ENOMEM;		// Out of memory
    }

  // Copy the signal mask into the structure
  memcpy (__attr->__sd, __sigmask, sizeof (__sigmask));	// Size depends on the signal mask type

  return 0;			// Success
}

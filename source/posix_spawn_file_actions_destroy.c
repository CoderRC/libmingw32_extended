#include <spawn.h>
#include <errno.h>
#include <stdlib.h>

int
posix_spawn_file_actions_destroy (posix_spawn_file_actions_t * __file_actions)
{
  if (!__file_actions)
    {
      return EINVAL;
    }
  if (__file_actions->__actions)
    {
      free (__file_actions->__actions);
    }
  __file_actions->__allocated = 0;
  __file_actions->__used = 0;
  __file_actions->__actions = 0;
  return 0;
}

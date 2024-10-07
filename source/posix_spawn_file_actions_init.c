#include <spawn.h>
#include <errno.h>

int
posix_spawn_file_actions_init (posix_spawn_file_actions_t * __file_actions)
{
  if (!__file_actions)
    {
      return EINVAL;
    }
  __file_actions->__allocated = 0;
  __file_actions->__used = 0;
  __file_actions->__actions = 0;
  return 0;
}

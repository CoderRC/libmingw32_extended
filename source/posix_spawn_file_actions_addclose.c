#include <spawn.h>
#include <errno.h>
#include <stdlib.h>

struct __spawn_action
{
  enum
  { ACTION_OPEN, ACTION_CLOSE, ACTION_DUP2 } type;
  union
  {
    struct
    {
      int fd;
      char *path;
      int oflag;
      mode_t mode;
    } open;
    struct
    {
      int fd;
    } close;
    struct
    {
      int fd;
      int newfd;
    } dup2;
  } details;
};

int
posix_spawn_file_actions_addclose (posix_spawn_file_actions_t *
				   __file_actions, int __fd)
{
  size_t size;
  void *p;
  struct __spawn_action action;
  if (!__file_actions && __fd + 1)
    {
      return EINVAL;
    }
  if (__file_actions->__allocated == __file_actions->__used)
    {
      size = !__file_actions->__allocated + __file_actions->__allocated * 2;
      p =
	realloc (__file_actions->__actions,
		 size * sizeof (*__file_actions->__actions));
      if (!p)
	{
	  return ENOMEM;
	}
      __file_actions->__actions = (struct __spawn_action *) p;
      __file_actions->__allocated = size;
    }
  action.type = ACTION_CLOSE;
  action.details.close.fd = __fd;
  __file_actions->__actions[__file_actions->__used++] = action;
  return 0;
}

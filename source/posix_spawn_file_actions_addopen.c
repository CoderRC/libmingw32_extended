#include <spawn.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
posix_spawn_file_actions_addopen (posix_spawn_file_actions_t *
				  __restrict __file_actions,
				  int __fd,
				  const char *__restrict __path,
				  int __oflag, mode_t __mode)
{
  size_t size;
  void *p;
  struct __spawn_action action;
  if (!__file_actions || __fd == -1 || !__path)
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
  action.type = ACTION_OPEN;
  action.details.open.fd = __fd;
  action.details.open.path = strdup (__path);
  if (!action.details.open.path)
    {
      return ENOMEM;
    }
  action.details.open.oflag = __oflag;
  action.details.open.mode = __mode;
  __file_actions->__actions[__file_actions->__used++] = action;
  return 0;
}

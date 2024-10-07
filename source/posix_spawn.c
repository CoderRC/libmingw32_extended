#include <spawn.h>
#include <fcntl.h>
#include <windows.h>

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

static char *
argvToCommandLinePart (const char *str)
{
  size_t i;
  size_t size;
  size_t hasS;
  size_t extraData;
  size_t c;
  char *p;
  char *out;
  size = strlen (str);
  for (hasS = c = extraData = i = 0; i != size; i++)
    {
      hasS |= str[i] == ' ' || str[i] == '\t';
      if (str[i] == '"')
	{
	  extraData += c + 1;
	}
      c++;
      c *= str[i] == '\\';
    }
  extraData += c * hasS + hasS * 2;
  out = (char *) malloc (size + extraData + 1);
  if (!out)
    {
      return 0;
    }
  p = out;
  *p = '"';
  p += hasS;
  for (c = i = 0; i != size; i++)
    {
      if (str[i] == '"')
	{
	  for (; c; c--)
	    {
	      *p++ = '\\';
	    }
	  *p++ = '\\';
	  *p++ = '"';
	}
      else
	{
	  *p++ = str[i];
	}
      c++;
      c *= str[i] == '\\';
    }
  c *= hasS;
  for (; c; c--)
    {
      *p++ = '\\';
    }
  *p = '"';
  p += hasS;
  *p = 0;
  return out;
}

static char *
argvToCommandLine (char *const argv[])
{
  size_t i;
  size_t size;
  size_t hasS;
  char **arg;
  char *eArg;
  char *out;
  if (!argv || !(*argv))
    {
      return 0;
    }
  size = 1;
  arg = (char **) argv;
  if (*arg)
    {
      for (size = 0;
	   arg[0][size] && arg[0][size] != ' ' && arg[0][size] != '\t';
	   size++);
      size = arg[0][size] != 0;
      size *= 2;
      size += strlen (*arg);
      size += arg[0][size] != 0;
      arg++;
      if (*arg)
	{
	  size++;
	}
    }
  while (*arg)
    {
      eArg = argvToCommandLinePart (*arg);
      if (!eArg)
	{
	  return eArg;
	}
      size += strlen (eArg) + 1;
      free (eArg);
      arg++;
    }
  out = (char *) malloc (size);
  if (!out)
    {
      return out;
    }
  arg = (char **) argv;
  i = 0;
  if (*arg)
    {
      for (i = 0; arg[0][i] && arg[0][i] != ' ' && arg[0][i] != '\t'; i++);
      hasS = arg[0][i] != 0;
      *out = '\"';
      i = strlen (*arg);
      i += arg[0][i] != 0;
      memcpy (out + hasS, *arg, i);
      arg++;
      i += hasS;
      out[i] = '\"';
      i += hasS;
      out[i] = ' ';
      i += *arg != 0;
    }
  while (*arg)
    {
      eArg = argvToCommandLinePart (*arg);
      if (!eArg)
	{
	  free (out);
	  return eArg;
	}
      size = strlen (eArg);
      memcpy (out + i, eArg, size);
      free (eArg);
      i += size;
      arg++;
      out[i] = ' ';
      i += *arg != 0;
    }
  out[i] = 0;
  return out;
}

static char *
envpToEnvironment (char *const __envp[])
{
  size_t i;
  size_t size;
  char *p;
  char *out;
  if (!__envp)
    {
      return 0;
    }
  for (size = i = 0; __envp[i]; i++)
    {
      if (__envp[i][0])
	{
	  size += strlen (__envp[i]) + 1;
	}
    }
  size += !size;
  size++;
  out = (char *) malloc (size);
  if (!out)
    {
      return out;
    }
  p = out;
  for (i = 0; __envp[i]; i++)
    {
      if (__envp[i][0])
	{
	  size = strlen (__envp[i]);
	  memcpy (p, __envp[i], size);
	  p += size;
	  *p++ = 0;
	}
    }
  *p = 0;
  p += p == out;
  *p = 0;
  return out;
}

int
posix_spawn (pid_t * __restrict __pid,
	     const char *__restrict __path,
	     const posix_spawn_file_actions_t * __restrict __file_actions,
	     const posix_spawnattr_t * __restrict __attrp,
	     char *const argv[], char *const __envp[])
{
  size_t i;
  char *commandLine;
  char *environment;
  pid_t spawnResult;
  errno_t fail;
  int value;
  short int flags;
  int policy;
  struct sched_param sp;
  HANDLE *restore;
  HANDLE override;
  HANDLE override1;
  HANDLE tempOverride;
  DWORD *dwFlags;
  int *fds;
  int fd;
  SECURITY_ATTRIBUTES sa;
  PROCESS_INFORMATION pi;
  STARTUPINFOEX si;
  size_t attrListSize;
  fail = 0;
  flags = 0;
  policy = 0;
  override = INVALID_HANDLE_VALUE;
  sa.nLength = sizeof (sa);
  sa.lpSecurityDescriptor = 0;
  sa.bInheritHandle = FALSE;
  si.lpAttributeList = 0;
  if (__attrp)
    {
      posix_spawnattr_getflags (__attrp, &flags);
    }
  commandLine = argvToCommandLine (argv);
  if (__file_actions && __file_actions->__used
      && (restore =
	  (HANDLE *) malloc (__file_actions->__used * sizeof (*restore))))
    {
      dwFlags = (DWORD *) malloc (__file_actions->__used * sizeof (*dwFlags));
      if (dwFlags
	  && (fds = (int *) malloc (__file_actions->__used * sizeof (*fds))))
	{
	  if (CreatePipe (&override, &override1, &sa, 1))
	    {
	      for (i = 0; (int) i != __file_actions->__used; i++)
		{
		  if (__file_actions->__actions[i].type == ACTION_OPEN)
		    {
		      fds[i] = __file_actions->__actions[i].details.open.fd;
		      restore[i] = (HANDLE) _get_osfhandle (fds[i]);
		      if (restore[i] != INVALID_HANDLE_VALUE
			  && DuplicateHandle (GetCurrentProcess (), override,
					      GetCurrentProcess (),
					      &tempOverride, 0, FALSE,
					      DUPLICATE_SAME_ACCESS))
			{
			  dwFlags[i] = 0;
			  GetHandleInformation (restore[i], dwFlags + i);
			  SetHandleInformation (restore[i],
						HANDLE_FLAG_INHERIT |
						HANDLE_FLAG_PROTECT_FROM_CLOSE,
						HANDLE_FLAG_PROTECT_FROM_CLOSE);
			  fd = _open_osfhandle ((intptr_t) tempOverride, 0);
			  if (fd + 1)
			    {
			      dup2 (fd, fds[i]);
			      if (fd - fds[i])
				{
				  close (fd);
				}
			    }
			  else
			    {
			      fds[i] = fd;
			    }
			}
		      else
			{
			  fds[i] = -1;
			}
		      fd =
			open (__file_actions->__actions[i].details.open.path,
			      __file_actions->__actions[i].details.open.oflag,
			      __file_actions->__actions[i].details.open.mode);
		      dup2 (fd, __file_actions->__actions[i].details.open.fd);
		      if (fd - __file_actions->__actions[i].details.open.fd)
			{
			  close (fd);
			}
		    }
		  if (__file_actions->__actions[i].type == ACTION_CLOSE)
		    {
		      fds[i] = __file_actions->__actions[i].details.close.fd;
		      restore[i] = (HANDLE) _get_osfhandle (fds[i]);
		      if (restore[i] != INVALID_HANDLE_VALUE
			  && DuplicateHandle (GetCurrentProcess (), override,
					      GetCurrentProcess (),
					      &tempOverride, 0, FALSE,
					      DUPLICATE_SAME_ACCESS))
			{
			  dwFlags[i] = 0;
			  GetHandleInformation (restore[i], dwFlags + i);
			  SetHandleInformation (restore[i],
						HANDLE_FLAG_INHERIT |
						HANDLE_FLAG_PROTECT_FROM_CLOSE,
						HANDLE_FLAG_PROTECT_FROM_CLOSE);
			  fd = _open_osfhandle ((intptr_t) tempOverride, 0);
			  if (fd + 1)
			    {
			      dup2 (fd, fds[i]);
			      if (fd - fds[i])
				{
				  close (fd);
				}
			    }
			  else
			    {
			      fds[i] = fd;
			    }
			}
		      else
			{
			  fds[i] = -1;
			}
		      close (fds[i]);
		    }
		  if (__file_actions->__actions[i].type == ACTION_DUP2)
		    {
		      fds[i] =
			__file_actions->__actions[i].details.dup2.newfd;
		      restore[i] = (HANDLE) _get_osfhandle (fds[i]);
		      if (restore[i] != INVALID_HANDLE_VALUE)
			{
			  dwFlags[i] = 0;
			  GetHandleInformation (restore[i], dwFlags + i);
			  SetHandleInformation (restore[i],
						HANDLE_FLAG_INHERIT |
						HANDLE_FLAG_PROTECT_FROM_CLOSE,
						HANDLE_FLAG_PROTECT_FROM_CLOSE);
			}
		      else
			{
			  fds[i] = -1;
			}
		      dup2 (__file_actions->__actions[i].details.dup2.fd,
			    __file_actions->__actions[i].details.dup2.newfd);
		    }
		}
	    }
	  else
	    {
	      override = INVALID_HANDLE_VALUE;
	      free (restore);
	      free (dwFlags);
	      free (fds);
	    }
	}
      else
	{
	  if (dwFlags)
	    {
	      free (dwFlags);
	    }
	  free (restore);
	}
    }
  if (__path && flags & POSIX_SPAWN_SETPGROUP)
    {
      HANDLE parentProcessHandle =
	OpenProcess (PROCESS_CREATE_PROCESS | PROCESS_SET_INFORMATION, FALSE,
		     __attrp->__pgrp);
      if (parentProcessHandle)
	{
	  si = (STARTUPINFOEX)
	  {
	  0};
	  si.StartupInfo.cb = sizeof (si);
	  attrListSize = 0;
	  InitializeProcThreadAttributeList (0, 1, 0, &attrListSize);
	  si.lpAttributeList =
	    (LPPROC_THREAD_ATTRIBUTE_LIST) malloc (attrListSize);
	  if (si.lpAttributeList
	      && InitializeProcThreadAttributeList (si.lpAttributeList, 1, 0,
						    &attrListSize))
	    {
	      if (!UpdateProcThreadAttribute (si.lpAttributeList,
					      0,
					      PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
					      &parentProcessHandle,
					      sizeof (parentProcessHandle),
					      0, 0))
		{
		  DeleteProcThreadAttributeList (si.lpAttributeList);
		  free (si.lpAttributeList);
		  si.lpAttributeList = 0;
		}
	    }
	  else
	    {
	      if (si.lpAttributeList)
		{
		  free (si.lpAttributeList);
		  si.lpAttributeList = 0;
		}
	    }
	  if (si.lpAttributeList)
	    {
	      environment = envpToEnvironment (__envp);
	      if (!(argv && *argv && !commandLine)
		  && !(__envp && !environment)
		  && CreateProcess (__path, commandLine, 0, 0, TRUE,
				    EXTENDED_STARTUPINFO_PRESENT, environment,
				    0, &si.StartupInfo, &pi))
		{
		  spawnResult = pi.dwProcessId;
		  CloseHandle (pi.hProcess);
		  CloseHandle (pi.hThread);
		  free (si.lpAttributeList);
		}
	      else
		{
		  free (si.lpAttributeList);
		  si.lpAttributeList = 0;
		}
	      if (environment)
		{
		  free (environment);
		}
	    }
	  CloseHandle (parentProcessHandle);
	}
    }
  if (!si.lpAttributeList)
    {
      if (commandLine)
	{
	  pi.hProcess =
	    (HANDLE) _spawnle (_P_NOWAIT, __path, (const char *) commandLine,
			       0, (const char *const *) __envp);
	}
      else
	{
	  pi.hProcess =
	    (HANDLE) _spawnle (_P_NOWAIT, __path, 0,
			       (const char *const *) __envp);
	}
      spawnResult = (pid_t) pi.hProcess;
      if (pi.hProcess == INVALID_HANDLE_VALUE)
	{
	  fail = _get_errno (&value);
	  if (fail == E2BIG || value == E2BIG)
	    {
	      si = (STARTUPINFOEX)
	      {
	      0};
	      si.StartupInfo.cb = sizeof (si.StartupInfo);
	      environment = envpToEnvironment (__envp);
	      if (!(argv && *argv && !commandLine)
		  && !(__envp && !environment)
		  && CreateProcess (__path, commandLine, 0, 0, TRUE, 0,
				    environment, 0, &si.StartupInfo, &pi))
		{
		  spawnResult = (pid_t) pi.dwProcessId;
		  CloseHandle (pi.hProcess);
		  CloseHandle (pi.hThread);
		}
	      if (environment)
		{
		  free (environment);
		}
	    }
	}
      else
	{
	  spawnResult = GetProcessId (pi.hProcess);
	}
    }
  if (commandLine)
    {
      free (commandLine);
    }
  if (override != INVALID_HANDLE_VALUE)
    {
      for (i = 0; (int) i != __file_actions->__used; i++)
	{
	  if (fds[i] + 1)
	    {
	      SetHandleInformation (restore[i],
				    dwFlags[i] | HANDLE_FLAG_INHERIT |
				    HANDLE_FLAG_PROTECT_FROM_CLOSE,
				    dwFlags[i]);
	      fd = _open_osfhandle ((intptr_t) restore[i], 0);
	      dup2 (fd, fds[i]);
	      SetHandleInformation (restore[i],
				    HANDLE_FLAG_INHERIT |
				    HANDLE_FLAG_PROTECT_FROM_CLOSE,
				    HANDLE_FLAG_PROTECT_FROM_CLOSE);
	      if (fd - fds[i])
		{
		  close (fd);
		}
	      SetHandleInformation (restore[i],
				    dwFlags[i] | HANDLE_FLAG_INHERIT |
				    HANDLE_FLAG_PROTECT_FROM_CLOSE,
				    dwFlags[i]);
	    }
	}
      free (restore);
      free (dwFlags);
      free (fds);
      CloseHandle (override);
      CloseHandle (override1);
    }
  if (spawnResult == (pid_t) - 1)
    {
      if (fail)
	{
	  return fail;
	}
      else
	{
	  return value;
	}
    }
  if (flags & POSIX_SPAWN_SETSCHEDULER)
    {
      posix_spawnattr_getschedpolicy (__attrp, &policy);
    }
  else if (flags & POSIX_SPAWN_SETSCHEDPARAM)
    {
      policy = sched_getscheduler (spawnResult);
    }
  if (flags & (POSIX_SPAWN_SETSCHEDPARAM | POSIX_SPAWN_SETSCHEDULER))
    {
      posix_spawnattr_getschedparam (__attrp, &sp);
      sched_setscheduler (spawnResult, policy, &sp);
    }
  if (__pid)
    {
      *__pid = spawnResult;
    }
  return 0;
}

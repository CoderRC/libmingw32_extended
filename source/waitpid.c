#include <sys/wait.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdbool.h>

pid_t
waitpid (pid_t __pid, int *__status, int __options)
{
  bool allSuspended;
  bool suspendedChecked;
  HANDLE hThreadSnap;
  HANDLE hThread;
  THREADENTRY32 te32;
  HANDLE hProcess;
  DWORD dwMilliseconds;
  DWORD state;
  DWORD dwExitCode;
  allSuspended = false;
  if (__options & WUNTRACED)
    {
      hThreadSnap = CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD, __pid);
      if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
	  switch (GetLastError ())
	    {
	    case ERROR_INVALID_PARAMETER:
	      _set_errno (EINVAL);
	      break;
	    case ERROR_OUTOFMEMORY:
	      _set_errno (ENOMEM);
	      break;
	    default:
	      _set_errno (EIO);
	      break;
	    }
	  return -1;
	}
      te32.dwSize = sizeof (te32);
      if (!Thread32First (hThreadSnap, &te32))
	{
	  switch (GetLastError ())
	    {
	    case ERROR_INVALID_PARAMETER:
	      _set_errno (EINVAL);
	      break;
	    case ERROR_ACCESS_DENIED:
	      _set_errno (EACCES);
	      break;
	    case ERROR_OUTOFMEMORY:
	      _set_errno (ENOMEM);
	      break;
	    default:
	      _set_errno (EIO);
	      break;
	    }
	  CloseHandle (hThreadSnap);
	  return -1;
	}
      allSuspended = true;
      suspendedChecked = false;
      do
	{
	  if (allSuspended && te32.th32OwnerProcessID == __pid)
	    {
	      hThread =
		OpenThread (THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME,
			    FALSE, te32.th32ThreadID);
	      if (hThread)
		{
		  allSuspended &= SuspendThread (hThread) != 0;
		  ResumeThread (hThread);
		  CloseHandle (hThread);
		  suspendedChecked = true;
		}
	    }
	}
      while (allSuspended && Thread32Next (hThreadSnap, &te32));
      CloseHandle (hThreadSnap);
      if (suspendedChecked && allSuspended)
	{
	  if (__status)
	    {
	      *__status = 0x137F;
	    }
	  return __pid;
	}
      allSuspended = false;
    }
  hProcess =
    OpenProcess (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
		 __pid);
  if (!hProcess)
    {
      _set_errno (ECHILD);
      return -1;
    }
  dwMilliseconds = 0;
  dwMilliseconds -= (__options & WNOHANG) == 0;
  dwMilliseconds &= INFINITE;
  state = WaitForSingleObject (hProcess, dwMilliseconds);
  if (state == WAIT_OBJECT_0)
    {
      if (GetExitCodeProcess (hProcess, &dwExitCode))
	{
	  if (__status)
	    {
	      *__status = dwExitCode << 8;
	    }
	  CloseHandle (hProcess);
	  return __pid;
	}
      else
	{
	  _set_errno (ECHILD);
	  CloseHandle (hProcess);
	  return -1;
	}
    }
  else if (__options & WNOHANG && state == WAIT_TIMEOUT)
    {
      CloseHandle (hProcess);
      return 0;
    }
  else
    {
      _set_errno (EINVAL);
      CloseHandle (hProcess);
      return -1;
    }
}

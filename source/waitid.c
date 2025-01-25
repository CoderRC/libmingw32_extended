#include <sys/wait.h>
#include <windows.h>
#include <tlhelp32.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct WaitForProcessesStruct
{
  HANDLE hProcessSnap;
  DWORD pid;
  DWORD dwExitCode;
  DWORD processID;
  bool done;
} WaitForProcessesStruct;

static DWORD WINAPI
WaitForProcesses (LPVOID lpParam)
{
  WaitForProcessesStruct *wfps = (WaitForProcessesStruct *) lpParam;
  WaitForProcessesStruct wfpsCopy;
  pid_t __pid;
  HANDLE hThread;
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD state;
  DWORD dwExitCode;
  size_t i;
  HANDLE hHandles[MAXIMUM_WAIT_OBJECTS];
  i = 0;
  hProcessSnap = wfps->hProcessSnap;
  __pid = wfps->pid;
  pe32.dwSize = sizeof (PROCESSENTRY32);
  while (!wfps->done && Process32Next (hProcessSnap, &pe32))
    {
      if (pe32.th32ParentProcessID == __pid)
	{
	  hProcess =
	    OpenProcess (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
			 FALSE, pe32.th32ProcessID);
	  if (!hProcess)
	    {
	      continue;
	    }
	  hHandles[i++] = hProcess;
	  if (i == MAXIMUM_WAIT_OBJECTS - 1)
	    {
	      wfpsCopy = *wfps;
	      hThread =
		CreateThread (NULL, 0, WaitForProcesses, &wfpsCopy, 0, NULL);
	      if (!hThread)
		{
		  break;
		}
	      hHandles[i++] = hThread;
	      state = WaitForMultipleObjects (i, hHandles, FALSE, INFINITE);
	      if (state >= WAIT_OBJECT_0 && state < WAIT_OBJECT_0 + i - 1)
		{
		  if (GetExitCodeProcess
		      (hHandles[state - WAIT_OBJECT_0], &dwExitCode))
		    {
		      wfps->dwExitCode = dwExitCode;
		      wfps->processID =
			GetProcessId (hHandles[state - WAIT_OBJECT_0]);
		      wfps->done = true;
		      wfpsCopy.done = true;
		      WaitForSingleObject (hThread, INFINITE);
		    }
		}
	      else if (state == WAIT_OBJECT_0 + i - 1)
		{
		  *wfps = wfpsCopy;
		}
	      for (; i; i--)
		{
		  CloseHandle (hHandles[i - 1]);
		}
	    }
	}
    }
  if (!wfps->done && i)
    {
      state = WaitForMultipleObjects (i, hHandles, FALSE, INFINITE);
      if (state >= WAIT_OBJECT_0 && state < WAIT_OBJECT_0 + i)
	{
	  if (GetExitCodeProcess
	      (hHandles[state - WAIT_OBJECT_0], &dwExitCode))
	    {
	      wfps->dwExitCode = dwExitCode;
	      wfps->processID =
		GetProcessId (hHandles[state - WAIT_OBJECT_0]);
	      wfps->done = true;
	    }
	}
    }
  for (; i; i--)
    {
      CloseHandle (hHandles[i - 1]);
    }
  return 0;
}

int
waitid (idtype_t __idtype, id_t __id, siginfo_t * __infop, int __options)
{
  pid_t __pid;
  bool allSuspended;
  bool suspendedChecked;
  HANDLE hThreadSnap;
  HANDLE hThread;
  THREADENTRY32 te32;
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwMilliseconds;
  DWORD state;
  DWORD dwExitCode;
  size_t i;
  WaitForProcessesStruct wfps;
  WaitForProcessesStruct wfpsCopy;
  HANDLE hHandles[MAXIMUM_WAIT_OBJECTS];
  if (__idtype == P_PID)
    {
      __pid = (pid_t) __id;
      allSuspended = false;
      if (__options & (WUNTRACED | WSTOPPED))
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
		    OpenThread (THREAD_QUERY_INFORMATION |
				THREAD_SUSPEND_RESUME, FALSE,
				te32.th32ThreadID);
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
	      if (__infop)
		{
		  __infop->si_status = __infop->si_signo = SIGSTOP;
		  __infop->si_code = CLD_STOPPED;
		  __infop->si_errno = 0;
		  __infop->si_pid = __pid;
		  __infop->si_uid = 1000;
		  __infop->si_addr = 0;
		  __infop->si_band = 0;
		  __infop->si_value.sival_ptr = 0;
		}
	      return 0;
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
	      if (__infop)
		{
		  __infop->si_signo = SIGCHLD;
		  __infop->si_code = CLD_EXITED;
		  __infop->si_errno = 0;
		  __infop->si_pid = __pid;
		  __infop->si_uid = 1000;
		  __infop->si_addr = 0;
		  __infop->si_status = dwExitCode;
		  __infop->si_band = 0;
		  __infop->si_value.sival_ptr = 0;
		}
	      CloseHandle (hProcess);
	      return 0;
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
  __pid = (pid_t) __id;
  if (__idtype == P_ALL)
    {
      __pid = getpid ();
      __idtype = P_PGID;
    }
  if (__idtype == P_PGID)
    {
      allSuspended = false;
      hProcessSnap = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
      if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
	  _set_errno (EIO);
	  return -1;
	}
      pe32.dwSize = sizeof (PROCESSENTRY32);
      if (__options & (WUNTRACED | WSTOPPED))
	{
	  if (Process32First (hProcessSnap, &pe32))
	    {
	      do
		{
		  if (pe32.th32ParentProcessID == __pid)
		    {
		      hThreadSnap =
			CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD,
						  pe32.th32ProcessID);
		      if (hThreadSnap == INVALID_HANDLE_VALUE)
			{
			  continue;
			}
		      te32.dwSize = sizeof (te32);
		      if (!Thread32First (hThreadSnap, &te32))
			{
			  continue;
			}
		      allSuspended = true;
		      suspendedChecked = false;
		      do
			{
			  if (allSuspended
			      && te32.th32OwnerProcessID ==
			      pe32.th32ProcessID)
			    {
			      hThread =
				OpenThread (THREAD_QUERY_INFORMATION |
					    THREAD_SUSPEND_RESUME, FALSE,
					    te32.th32ThreadID);
			      if (hThread)
				{
				  allSuspended &=
				    SuspendThread (hThread) != 0;
				  ResumeThread (hThread);
				  CloseHandle (hThread);
				  suspendedChecked = true;
				}
			    }
			}
		      while (allSuspended
			     && Thread32Next (hThreadSnap, &te32));
		      CloseHandle (hThreadSnap);
		      if (suspendedChecked && allSuspended)
			{
			  if (__infop)
			    {
			      __infop->si_status = __infop->si_signo =
				SIGSTOP;
			      __infop->si_code = CLD_STOPPED;
			      __infop->si_errno = 0;
			      __infop->si_pid = pe32.th32ProcessID;
			      __infop->si_uid = 1000;
			      __infop->si_addr = 0;
			      __infop->si_band = 0;
			      __infop->si_value.sival_ptr = 0;
			    }
			  CloseHandle (hProcessSnap);
			  return 0;
			}
		      allSuspended = false;
		    }
		}
	      while (Process32Next (hProcessSnap, &pe32));
	    }
	}
      if (Process32First (hProcessSnap, &pe32))
	{
	  if (__options & WNOHANG)
	    {
	      do
		{
		  if (pe32.th32ParentProcessID == __pid)
		    {
		      hProcess =
			OpenProcess (SYNCHRONIZE |
				     PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
				     pe32.th32ProcessID);
		      if (!hProcess)
			{
			  continue;
			}
		      state = WaitForSingleObject (hProcess, 0);
		      if (state == WAIT_OBJECT_0)
			{
			  if (GetExitCodeProcess (hProcess, &dwExitCode))
			    {
			      if (__infop)
				{
				  __infop->si_signo = SIGCHLD;
				  __infop->si_code = CLD_EXITED;
				  __infop->si_errno = 0;
				  __infop->si_pid = pe32.th32ProcessID;
				  __infop->si_uid = 1000;
				  __infop->si_addr = 0;
				  __infop->si_status = dwExitCode;
				  __infop->si_band = 0;
				  __infop->si_value.sival_ptr = 0;
				}
			      CloseHandle (hProcessSnap);
			      CloseHandle (hProcess);
			      return 0;
			    }
			  else
			    {
			      _set_errno (ECHILD);
			      CloseHandle (hProcessSnap);
			      CloseHandle (hProcess);
			      return -1;
			    }
			}
		      else if (state == WAIT_TIMEOUT)
			{
			  CloseHandle (hProcessSnap);
			  CloseHandle (hProcess);
			  return 0;
			}
		      else
			{
			  _set_errno (EINVAL);
			  CloseHandle (hProcessSnap);
			  CloseHandle (hProcess);
			  return -1;
			}
		    }
		}
	      while (Process32Next (hProcessSnap, &pe32));
	      CloseHandle (hProcessSnap);
	    }
	  else
	    {
	      i = 0;
	      wfps.hProcessSnap = hProcessSnap;
	      wfps.pid = __pid;
	      wfps.done = false;
	      do
		{
		  if (pe32.th32ParentProcessID == __pid)
		    {
		      hProcess =
			OpenProcess (SYNCHRONIZE |
				     PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
				     pe32.th32ProcessID);
		      if (!hProcess)
			{
			  continue;
			}
		      hHandles[i++] = hProcess;
		      if (i == MAXIMUM_WAIT_OBJECTS - 1)
			{
			  wfpsCopy = wfps;
			  hThread =
			    CreateThread (NULL, 0, WaitForProcesses,
					  &wfpsCopy, 0, NULL);
			  if (!hThread)
			    {
			      _set_errno (EIO);
			      return -1;
			    }
			  hHandles[i++] = hThread;
			  state =
			    WaitForMultipleObjects (i, hHandles, FALSE,
						    INFINITE);
			  if (state >= WAIT_OBJECT_0
			      && state < WAIT_OBJECT_0 + i - 1)
			    {
			      if (GetExitCodeProcess
				  (hHandles[state - WAIT_OBJECT_0],
				   &dwExitCode))
				{
				  wfps.dwExitCode = dwExitCode;
				  wfps.processID =
				    GetProcessId (hHandles
						  [state - WAIT_OBJECT_0]);
				  wfps.done = true;
				  wfpsCopy.done = true;
				  WaitForSingleObject (hThread, INFINITE);
				}
			    }
			  else if (state == WAIT_OBJECT_0 + i - 1)
			    {
			      wfps = wfpsCopy;
			    }
			  for (; i; i--)
			    {
			      CloseHandle (hHandles[i - 1]);
			    }
			}
		    }
		}
	      while (!wfps.done && Process32Next (hProcessSnap, &pe32));
	      if (!wfps.done && i)
		{
		  state =
		    WaitForMultipleObjects (i, hHandles, FALSE, INFINITE);
		  if (state >= WAIT_OBJECT_0 && state < WAIT_OBJECT_0 + i)
		    {
		      if (GetExitCodeProcess
			  (hHandles[state - WAIT_OBJECT_0], &dwExitCode))
			{
			  wfps.dwExitCode = dwExitCode;
			  wfps.processID =
			    GetProcessId (hHandles[state - WAIT_OBJECT_0]);
			  wfps.done = true;
			}
		    }
		}
	      for (; i; i--)
		{
		  CloseHandle (hHandles[i - 1]);
		}
	      if (wfps.done && __infop)
		{
		  __infop->si_signo = SIGCHLD;
		  __infop->si_code = CLD_EXITED;
		  __infop->si_errno = 0;
		  __infop->si_pid = wfps.processID;
		  __infop->si_uid = 1000;
		  __infop->si_addr = 0;
		  __infop->si_status = wfps.dwExitCode;
		  __infop->si_band = 0;
		  __infop->si_value.sival_ptr = 0;
		}
	      CloseHandle (hProcessSnap);
	      return 0;
	    }
	}
    }
  return 0;
}

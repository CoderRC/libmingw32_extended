#include <sys/mman.h>
#include <windows.h>
#include <unistd.h>
#include <psapi.h>

int
msync (void *__addr, size_t __len, int __flags)
{
  const char *root = "\\\\?\\GLOBALROOT";
  if (__flags & (MS_ASYNC | MS_SYNC))
    {
      if (FlushViewOfFile (__addr, __len))
	{
	  if (__flags & MS_SYNC)
	    {
	      pid_t mepid = getpid ();
	      HANDLE hProcess =
		OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, mepid);
	      DWORD nSize = 4096;
	      LPSTR lpFilename = malloc (nSize - strlen (root));
	      lpFilename += strlen (root) / sizeof (*lpFilename);
	      DWORD dwErrCode = GetLastError ();
	      SetLastError (ERROR_SUCCESS);
	      GetMappedFileName (hProcess, __addr, lpFilename, nSize);
	      while (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
		{
		  nSize += 4096;
		  lpFilename -= strlen (root) / sizeof (*lpFilename);
		  free (lpFilename);
		  lpFilename = malloc (nSize);
		  lpFilename += strlen (root) / sizeof (*lpFilename);
		  GetMappedFileName (hProcess, __addr, lpFilename,
				     nSize - strlen (root));
		}
	      SetLastError (dwErrCode);
	      lpFilename -= strlen (root) / sizeof (*lpFilename);
	      memcpy (lpFilename, root, strlen (root));
	      HANDLE hFile =
		CreateFile (lpFilename, GENERIC_WRITE,
			    FILE_SHARE_READ | FILE_SHARE_WRITE |
			    FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	      FlushFileBuffers (hFile);
	      free (lpFilename);
	      CloseHandle (hFile);
	      CloseHandle (hProcess);
	    }
	}
    }
  if (__flags & MS_INVALIDATE)
    {
      for (size_t i = 0; i != __len; i++)
	{
	  _mm_clflush ((void *) (((size_t) __addr) + i));
	}
    }
  return 0;
}

#include <dlfcn.h>
#include <windows.h>
#include <psapi.h>
#include <unistd.h>

static PIMAGE_NT_HEADERS
ImageNtHeader (PVOID Base)
{
  PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER) Base;
  if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE)
    return NULL;

  PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS) ((char *) Base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE)
    return NULL;

  return nt;
}

int
dladdr (const void *addr, Dl_info * info)
{
  if (!addr || !info)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      return 0;
    }

  DWORD oldErr = GetLastError ();
  int resultCode = 0;
  HANDLE hProcess = NULL;
  LPSTR lpFilename = NULL;
  LPSTR resolvedPath = NULL;

  HANDLE hProcess = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, getpid ());
  if (!hProcess)
    {
      SetLastError (ERROR_INVALID_HANDLE);
      return 0;
    }

  LPCSTR root = "\\\\?\\GLOBALROOT";
  size_t rootlen = strlen (root);
  DWORD nSize = MAX_PATH;
  lpFilename = (LPSTR) malloc (nSize + rootlen + 1);
  if (!lpFilename)
    {
      SetLastError (ERROR_NOT_ENOUGH_MEMORY);
      goto cleanup;
    }

  LPSTR filenamePtr = lpFilename + rootlen;
  DWORD result =
    GetMappedFileNameA (hProcess, (LPVOID) addr, filenamePtr, nSize);
  if (result == 0)
    {
      goto cleanup;
    }

  memcpy (lpFilename, root, rootlen);
  lpFilename[rootlen + result] = '\0';

  HANDLE hFile = CreateFileA (lpFilename,
			      GENERIC_READ,
			      FILE_SHARE_READ | FILE_SHARE_WRITE |
			      FILE_SHARE_DELETE,
			      NULL,
			      OPEN_EXISTING,
			      FILE_FLAG_BACKUP_SEMANTICS,
			      NULL);

  resolvedPath = lpFilename;
  if (hFile != INVALID_HANDLE_VALUE)
    {
      DWORD finalSize =
	GetFinalPathNameByHandleA (hFile, NULL, 0, VOLUME_NAME_DOS);
      if (finalSize)
	{
	  LPSTR tmpPath = (LPSTR) malloc (finalSize + 1);
	  if (tmpPath
	      && GetFinalPathNameByHandleA (hFile, tmpPath, finalSize,
					    VOLUME_NAME_DOS))
	    {
	      free (lpFilename);
	      resolvedPath = tmpPath;
	    }
	  else
	    {
	      free (tmpPath);
	    }
	}
      CloseHandle (hFile);
    }

  MEMORY_BASIC_INFORMATION mbi;
  if (!VirtualQuery (addr, &mbi, sizeof (mbi)))
    {
      mbi.AllocationBase = (LPVOID) addr;
    }

  PIMAGE_NT_HEADERS ntHeaders = ImageNtHeader (mbi.AllocationBase);
  if (!ntHeaders)
    {
      goto cleanup;
    }

  DWORD exportDirRVA =
    ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].
    VirtualAddress;
  if (!exportDirRVA)
    {
      goto cleanup;
    }

  PIMAGE_EXPORT_DIRECTORY exportDir =
    (PIMAGE_EXPORT_DIRECTORY) ((size_t) mbi.AllocationBase + exportDirRVA);

  DWORD *nameRVAs =
    (DWORD *) ((size_t) mbi.AllocationBase + exportDir->AddressOfNames);
  DWORD *funcRVAs =
    (DWORD *) ((size_t) mbi.AllocationBase + exportDir->AddressOfFunctions);
  WORD *ordinals =
    (WORD *) ((size_t) mbi.AllocationBase + exportDir->AddressOfNameOrdinals);

  LPSTR closestName = NULL;
  LPVOID closestAddr = NULL;
  size_t addrOffset = (size_t) addr - (size_t) mbi.AllocationBase;
  DWORD bestDistance = (DWORD) - 1;

  for (DWORD i = 0; i < exportDir->NumberOfNames; ++i)
    {
      WORD ordinal = ordinals[i];
      DWORD funcRVA = funcRVAs[ordinal];
      if (funcRVA <= addrOffset)
	{
	  DWORD distance = addrOffset - funcRVA;
	  if (distance < bestDistance)
	    {
	      bestDistance = distance;
	      closestName =
		(LPSTR) ((size_t) mbi.AllocationBase + nameRVAs[i]);
	      closestAddr = (LPVOID) ((size_t) mbi.AllocationBase + funcRVA);
	    }
	}
    }

  info->dli_fname = resolvedPath;
  info->dli_fbase = mbi.AllocationBase;
  info->dli_sname = closestName;
  info->dli_saddr = closestAddr;

  resultCode = 1;

cleanup:
  SetLastError (oldErr);
  if (hProcess)
    {
      CloseHandle (hProcess);
    }

  if (resultCode == 0 && resolvedPath != lpFilename)
    {
      free (resolvedPath);
    }

  if ((resultCode == 1 && resolvedPath != lpFilename) || resultCode == 0)
    {
      // User takes ownership of resolvedPath
    }

  else if (lpFilename)
    {
      free (lpFilename);
    }

  return resultCode;
}

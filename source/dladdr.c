#include <dlfcn.h>
#include <windows.h>
#include <psapi.h>
#include <unistd.h>

static PIMAGE_NT_HEADERS
ImageNtHeader (PVOID Base)
{
  PIMAGE_DOS_HEADER imageDosHeader = Base;
  return (PIMAGE_NT_HEADERS) ((char *) (imageDosHeader) +
			      imageDosHeader->e_lfanew);
}

int
dladdr (const void *addr, Dl_info * info)
{
  DWORD dwErrCode = GetLastError ();
  pid_t mepid = getpid ();
  HANDLE hProcess = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, mepid);
  const char *root = "\\\\?\\GLOBALROOT";
  LPSTR lpFilename;
  DWORD nSize = 1;
  HANDLE hFile;
  MEMORY_BASIC_INFORMATION lpBuffer;
  LPSTR lpszFilePath;
  PIMAGE_NT_HEADERS imageNtHeaders;
  PIMAGE_EXPORT_DIRECTORY imageExportDirectory;
  DWORD i;
  DWORD NumberOfNames;
  DWORD *AddressOfNames;
  DWORD *AddressOfFunctions;
  DWORD AddressOfFunction;
  char *dli_sname;
  void *dli_saddr;
  SetLastError (ERROR_SUCCESS);
  nSize += strlen (root) / sizeof (*lpFilename);
  lpFilename = malloc (nSize);
  lpFilename += strlen (root) / sizeof (*lpFilename);
  nSize -= strlen (root) / sizeof (*lpFilename);
  GetMappedFileName (hProcess, (LPVOID) addr, lpFilename, nSize);
  while (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
    {
      nSize += 4096;
      lpFilename -= strlen (root) / sizeof (*lpFilename);
      nSize += strlen (root) / sizeof (*lpFilename);
      free (lpFilename);
      lpFilename = malloc (nSize);
      if (lpFilename == 0)
	{
	  return 0;
	}
      lpFilename += strlen (root) / sizeof (*lpFilename);
      nSize -= strlen (root) / sizeof (*lpFilename);
      GetMappedFileName (hProcess, (LPVOID) addr, lpFilename, nSize);
    }
  if (GetLastError () != ERROR_SUCCESS)
    {
      return 0;
    }
  lpFilename -= strlen (root) / sizeof (*lpFilename);
  memcpy (lpFilename, root, strlen (root));
  nSize = 1;

  hFile = CreateFile (lpFilename, GENERIC_READ,
		      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
    {
      nSize = GetFinalPathNameByHandle (hFile, 0, 0, VOLUME_NAME_DOS);

      if (nSize != 0)
	{
	  nSize++;
	  lpszFilePath = (char *) malloc (nSize);
	  if (!lpszFilePath || !GetFinalPathNameByHandle (hFile,
							  lpszFilePath, nSize,
							  VOLUME_NAME_DOS))
	    {
	      free (lpszFilePath);
	      lpszFilePath = lpFilename;
	    }
	  else
	    {
	      free (lpFilename);
	    }
	}
      else
	{
	  lpszFilePath = lpFilename;
	}
    }
  else
    {
      lpszFilePath = lpFilename;
    }

  if (!VirtualQuery (addr, &lpBuffer, sizeof (lpBuffer)))
    {
      lpBuffer.AllocationBase = (void *) addr;
    }

  imageNtHeaders = ImageNtHeader (lpBuffer.AllocationBase);
  imageExportDirectory =
    (PIMAGE_EXPORT_DIRECTORY) ((size_t) lpBuffer.AllocationBase +
			       imageNtHeaders->OptionalHeader.DataDirectory
			       [IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
  if (imageExportDirectory->NumberOfNames <
      imageExportDirectory->NumberOfFunctions)
    {
      NumberOfNames = imageExportDirectory->NumberOfNames;
    }
  else
    {
      NumberOfNames = imageExportDirectory->NumberOfFunctions;
    }
  AddressOfNames =
    (DWORD *) ((size_t) lpBuffer.AllocationBase +
	       imageExportDirectory->AddressOfNames);
  AddressOfFunctions =
    (DWORD *) ((size_t) lpBuffer.AllocationBase +
	       imageExportDirectory->AddressOfFunctions);
  AddressOfFunction = (size_t) addr - (size_t) lpBuffer.AllocationBase;
  dli_sname = 0;
  dli_saddr = 0;

  for (i = 0; i != NumberOfNames; i++)
    {
      if (AddressOfFunctions[i] <= AddressOfFunction)
	{
	  dli_sname =
	    (char *) ((size_t) lpBuffer.AllocationBase + AddressOfNames[i]);
	  dli_saddr =
	    (void *) ((size_t) lpBuffer.AllocationBase +
		      AddressOfFunctions[i]);
	  i = NumberOfNames - 1;
	}
    }

  info->dli_fname = lpszFilePath;
  info->dli_fbase = lpBuffer.AllocationBase;
  info->dli_sname = dli_sname;
  info->dli_saddr = dli_saddr;
  SetLastError (dwErrCode);
  return 1;
}

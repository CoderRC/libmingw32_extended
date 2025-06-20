#include <dlfcn.h>
#include <windows.h>
#include <synchapi.h>

static
__declspec (thread)
     char dlerror_buffer[512] = { 0 };

static SRWLOCK g_imports_lock = SRWLOCK_INIT;

static void
set_dlerror_from_last_error (void)
{
  DWORD err = GetLastError ();
  if (err == 0)
    {
      dlerror_buffer[0] = '\0';
      return;
    }

  DWORD length =
    FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM |
		    FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL,
		    err,
		    MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		    dlerror_buffer,
		    sizeof (dlerror_buffer),
		    NULL);

  if (length == 0)
    snprintf (dlerror_buffer, sizeof (dlerror_buffer),
	      "Unknown error 0x%08lx", err);
  else
    while (length > 0
	   && (dlerror_buffer[length - 1] == '\n'
	       || dlerror_buffer[length - 1] == '\r'))
      {
	dlerror_buffer[--length] = '\0';
      }
  SetLastError (0);
}

static PIMAGE_NT_HEADERS
ImageNtHeader (void *base)
{
  if (!base)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      set_dlerror_from_last_error ();
      return NULL;
    }

  PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) base;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
      SetLastError (ERROR_BAD_FORMAT);
      set_dlerror_from_last_error ();
      return NULL;
    }

  PIMAGE_NT_HEADERS ntHeaders =
    (PIMAGE_NT_HEADERS) ((BYTE *) base + dosHeader->e_lfanew);
  if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
    {
      SetLastError (ERROR_BAD_FORMAT);
      set_dlerror_from_last_error ();
      return NULL;
    }

  return ntHeaders;
}

static BOOL
HasFixedImports (HMODULE module)
{
  static HMODULE patchedModules[64];
  static size_t count = 0;

  AcquireSRWLockExclusive (&g_imports_lock);

  for (size_t i = 0; i < count; i++)
    {
      if (patchedModules[i] == module)
	{
	  ReleaseSRWLockExclusive (&g_imports_lock);
	  return TRUE;
	}
    }

  if (count < sizeof (patchedModules) / sizeof (patchedModules[0]))
    patchedModules[count++] = module;

  ReleaseSRWLockExclusive (&g_imports_lock);
  return FALSE;
}

static inline void
PatchIATEntry (PIMAGE_THUNK_DATA thunk, FARPROC procAddress)
{
#ifdef _WIN64
  thunk->u1.Function = (ULONGLONG) procAddress;
#else
  thunk->u1.Function = (DWORD) procAddress;
#endif
}

typedef BOOL (WINAPI * DllEntryProc) (HINSTANCE, DWORD, LPVOID);

void *
dlsym (void *__restrict __handle, const char *__restrict __name)
{
  if (!__handle || !__name)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      set_dlerror_from_last_error ();
      return NULL;
    }

  HMODULE module = (HMODULE) __handle;

  PIMAGE_NT_HEADERS ntHeaders = ImageNtHeader (__handle);
  if (!ntHeaders)
    {
      return NULL;
    }

  IMAGE_DATA_DIRECTORY importDir =
    ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

  // no imports to patch, just return symbol
  if (importDir.VirtualAddress == 0 || importDir.Size == 0)
    return (LPVOID) GetProcAddress (module, __name);

  BYTE *baseAddr = (BYTE *) __handle;
  PIMAGE_IMPORT_DESCRIPTOR importDesc =
    (PIMAGE_IMPORT_DESCRIPTOR) (baseAddr + importDir.VirtualAddress);

  MEMORY_BASIC_INFORMATION mbi;
  if (VirtualQuery (importDesc, &mbi, sizeof (mbi)) == 0)
    {
      set_dlerror_from_last_error ();
      return NULL;
    }

  if ((mbi.Protect & PAGE_WRITECOPY) && !HasFixedImports (module))
    {
      for (; importDesc->Name != 0; importDesc++)
	{
	  LPCSTR libName = (LPCSTR) (baseAddr + importDesc->Name);
	  HMODULE hModule = LoadLibraryA (libName);
	  if (!hModule)
	    continue;

	  PIMAGE_THUNK_DATA importLookupTable =
	    (PIMAGE_THUNK_DATA) (baseAddr + importDesc->OriginalFirstThunk);
	  PIMAGE_THUNK_DATA importAddressTable =
	    (PIMAGE_THUNK_DATA) (baseAddr + importDesc->FirstThunk);

	  if (!importLookupTable)
	    importLookupTable = importAddressTable;

	  for (; importLookupTable->u1.AddressOfData != 0;
	       importLookupTable++, importAddressTable++)
	    {
	      FARPROC procAddress = NULL;

	      if (importLookupTable->u1.Ordinal & IMAGE_ORDINAL_FLAG)
		{
		  WORD ordinal =
		    (WORD) (importLookupTable->u1.Ordinal & 0xFFFF);
		  procAddress =
		    GetProcAddress (hModule, (LPCSTR) (uintptr_t) ordinal);
		}
	      else
		{
		  PIMAGE_IMPORT_BY_NAME importByName =
		    (PIMAGE_IMPORT_BY_NAME) (baseAddr +
					     importLookupTable->u1.
					     AddressOfData);
		  procAddress = GetProcAddress (hModule, importByName->Name);
		}

	      if (!procAddress)
		{
		  procAddress = NULL;
		}

	      PatchIATEntry (importAddressTable, procAddress);
	    }
	}

      FARPROC entryPoint =
	(FARPROC) (baseAddr + ntHeaders->OptionalHeader.AddressOfEntryPoint);
      if (entryPoint)
	{
	  DllEntryProc DllEntry = (DllEntryProc) entryPoint;

	  if (!DllEntry ((HINSTANCE) __handle, DLL_PROCESS_ATTACH, NULL))
	    {
	      SetLastError (ERROR_DLL_INIT_FAILED);
	      return NULL;
	    }
	}
    }

  void *result = (LPVOID) GetProcAddress (module, __name);
  if (!result)
    set_dlerror_from_last_error ();

  return result;
}

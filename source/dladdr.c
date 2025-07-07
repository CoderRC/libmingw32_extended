#include <dbghelp.h>
#include <dlfcn.h>
#include <psapi.h>
#include <synchapi.h>
#include <unistd.h>
#include <windows.h>

#pragma comment(lib, "dbghelp.lib")

/// @brief Shared Reader-Writer lock to guard dbghelp usage.
static SRWLOCK g_dbghelp_lock = SRWLOCK_INIT;

/// @brief One-time initialization for dbghelp symbols per process
static BOOL CALLBACK
SymInitOnceCallback (PINIT_ONCE InitOnce, PVOID Parameter, PVOID * Context)
{
  HANDLE process = GetCurrentProcess ();
  SymSetOptions (SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_NO_PROMPTS);
  return SymInitialize (process, NULL, TRUE);
}


int
dladdr (const LPVOID addr, Dl_info * info)
{
  if (!addr || !info)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      return 0;
    }

  memset (info, 0, sizeof (*info));
  HANDLE process = GetCurrentProcess ();

  static INIT_ONCE initOnce = INIT_ONCE_STATIC_INIT;

  if (!InitOnceExecuteOnce (&initOnce, SymInitOnceCallback, NULL, NULL))
    {
      return 0;
    }

  AcquireSRWLockShared (&g_dbghelp_lock);

  DWORD64 displacement = 0;
  BYTE symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME * sizeof (char)] =
    { 0 };
  PSYMBOL_INFO symbol = (PSYMBOL_INFO) symbolBuffer;
  symbol->SizeOfStruct = sizeof (SYMBOL_INFO);
  symbol->MaxNameLen = MAX_SYM_NAME;

  BOOL hasSymbol =
    SymFromAddr (process, (DWORD64) (uintptr_t) addr, &displacement, symbol);

  IMAGEHLP_MODULE64 moduleInfo = { 0 };
  moduleInfo.SizeOfStruct = sizeof (moduleInfo);
  BOOL hasModule =
    SymGetModuleInfo64 (process, (DWORD64) (uintptr_t) addr, &moduleInfo);

  ReleaseSRWLockShared (&g_dbghelp_lock);

  if (!hasModule)
    return 0;

  info->dli_fname = _strdup (moduleInfo.ImageName);
  if (!info->dli_fname)
    return 0;
  info->dli_fbase = (LPVOID) (uintptr_t) moduleInfo.BaseOfImage;

  if (hasSymbol)
    {
      char demangled[MAX_SYM_NAME];
      if (UnDecorateSymbolName
	  (symbol->Name, demangled, sizeof (demangled),
	   UNDNAME_COMPLETE | UNDNAME_NO_LEADING_UNDERSCORES))
	{
	  info->dli_sname = _strdup (demangled);
	}
      else
	{
	  info->dli_sname = _strdup (symbol->Name);
	}
      if (!info->dli_sname)
	{
	  free (info->dli_fname);
	  info->dli_fname = NULL;
	  return 0;
	}
      info->dli_saddr = (LPVOID) (uintptr_t) symbol->Address;
    }
  else
    {
      info->dli_sname = NULL;
      info->dli_saddr = NULL;
    }

  return 1;
}

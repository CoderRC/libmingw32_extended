#include <dlfcn.h>
#include <windows.h>
#include <psapi.h>
#include <unistd.h>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

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
  int ret = 0;
  DWORD lastErr = GetLastError ();

  if (!addr || !info)
    {
      lastErr = ERROR_INVALID_PARAMETER;
      goto out;
    }

  memset (info, 0, sizeof (*info));
  HANDLE process = GetCurrentProcess ();

  static INIT_ONCE initOnce = INIT_ONCE_STATIC_INIT;

  // SymInitialize failed or already initialized improperly
  if (!InitOnceExecuteOnce (&initOnce, SymInitOnceCallback, NULL, NULL))
    {
      goto out;
    }

  DWORD64 displacement = 0;
  BYTE symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME * sizeof (TCHAR)] =
    { 0 };
  PSYMBOL_INFO symbol = (PSYMBOL_INFO) symbolBuffer;
  symbol->SizeOfStruct = sizeof (SYMBOL_INFO);
  symbol->MaxNameLen = MAX_SYM_NAME;

  BOOL hasSymbol =
    SymFromAddr (process, (DWORD64) (uintptr_t) addr, &displacement, symbol);

  IMAGEHLP_MODULE64 moduleInfo = { 0 };
  moduleInfo.SizeOfStruct = sizeof (moduleInfo);

  if (!SymGetModuleInfo64 (process, (DWORD64) (uintptr_t) addr, &moduleInfo))
    goto out;

  info->dli_fname = _strdup (moduleInfo.ImageName);
  info->dli_fbase = (LPVOID) (uintptr_t) moduleInfo.BaseOfImage;

  if (hasSymbol)
    {
      char demangled[MAX_SYM_NAME];
      if (UnDecorateSymbolName
	  (symbol->Name, demangled, MAX_SYM_NAME,
	   UNDNAME_COMPLETE | UNDNAME_NO_LEADING_UNDERSCORES))
	{

	  info->dli_sname = _strdup (demangled);
	}
      else
	{
	  info->dli_sname = _strdup (symbol->Name);
	}

      info->dli_saddr = (LPVOID) (uintptr_t) symbol->Address;
    }
  else
    {
      info->dli_sname = NULL;
      info->dli_saddr = NULL;
    }

  ret = 1;

out:
  SetLastError (lastErr);
  return ret;
}

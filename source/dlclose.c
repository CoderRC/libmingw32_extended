#include <dlfcn.h>
#include <windows.h>

int
dlclose (LPVOID __handle)
{
  if (!__handle)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      return -1;
    }

  BOOL success = FreeLibrary ((HMODULE) __handle);
  if (!success)
    {
      SetLastError (GetLastError ());
      return -1;
    }

  return 0;
}

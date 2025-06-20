#include <dlfcn.h>
#include <windows.h>

static char dlerror_buffer[512] = { 0 };

static void
set_dlerror_from_last_error (void)
{
  DWORD err = GetLastError ();
  if (err == 0)
    {
      dlerror_buffer[0] = '\0';
      return;
    }
  FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,
		  err,
		  MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		  dlerror_buffer, sizeof (dlerror_buffer), NULL);
  SetLastError (0);
}

void *
dlopen (const char *__file, int __mode)
{
  DWORD dwFlags = 0;
  if (!__file)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      return NULL;
    }

  if (__mode & RTLD_LAZY)
    {
      dwFlags = DONT_RESOLVE_DLL_REFERENCES;
    }

  HMODULE module = LoadLibraryExA (__file, NULL, dwFlags);
  if (!module)
    {
      set_dlerror_from_last_error ();
    }
  else
    {
      dlerror_buffer[0] = '\0';
    }
  return (LPVOID) module;
}

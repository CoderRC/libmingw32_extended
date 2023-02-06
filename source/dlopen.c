#include <dlfcn.h>
#include <windows.h>

void *
dlopen (const char *__file, int __mode)
{
  DWORD dwFlags = 0;
  if (__mode == RTLD_LAZY)
    {
      dwFlags = DONT_RESOLVE_DLL_REFERENCES;
    }
  return LoadLibraryExA (__file, NULL, dwFlags);
}

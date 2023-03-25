#include <dlfcn.h>
#include <windows.h>

int
dlclose (void *__handle)
{
  return !FreeLibrary (__handle);
}

#include <sys/mman.h>
#include <windows.h>

int
mlock (const void *__addr, size_t __len)
{
  VirtualLock ((LPVOID) __addr, __len);
  return 0;
}

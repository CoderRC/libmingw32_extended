#include <sys/mman.h>
#include <windows.h>

int
munlock (const void *__addr, size_t __len)
{
  VirtualUnlock ((LPVOID) __addr, __len);
  return 0;
}

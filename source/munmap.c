#include <sys/mman.h>
#include <windows.h>

int
munmap (void *__addr, size_t __len)
{
  if (UnmapViewOfFile (__addr))
    {
      return 0;
    }
  VirtualFree (__addr, __len, MEM_DECOMMIT);
  VirtualFree (__addr, 0, MEM_RELEASE);
  return 0;
}

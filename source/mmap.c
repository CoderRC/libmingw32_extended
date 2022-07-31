#include <sys/mman.h>
#include <windows.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

void *
mmap (void *__addr, size_t __len, int __prot, int __flags, int __fd,
      off_t __off)
{
  void *buf;
  struct stat st;
  size_t size = __len;
  size_t maxSize = size;
  off_t off = 0;
  const DWORD flProtect = PAGE_EXECUTE_READWRITE;
  const DWORD flAllocationType = MEM_COMMIT | MEM_RESERVE;

  if (~__flags & MAP_ANONYMOUS)
    {
      fstat (__fd, &st);
      size = st.st_size;
      maxSize = size;
      off = __off;
      size -= off;
      if (size > __len)
	{
	  size = __len;
	}
    }

  if (__flags & MAP_PRIVATE)
    {
      if (__flags & MAP_ANONYMOUS)
	{
	  buf = VirtualAlloc (__addr, __len, flAllocationType, flProtect);
	  mprotect (buf, __len, __prot);
	  return buf;
	}
      else
	{
	  buf = VirtualAlloc (__addr, __len, flAllocationType, flProtect);
	  FILE *fp = fdopen (__fd, "rb");
	  fpos_t fpos;
	  fpos_t fposoff = __off;
	  fgetpos (fp, &fpos);
	  fsetpos (fp, &fposoff);
	  fread (buf, size, 1, fp);
	  fsetpos (fp, &fpos);
	  mprotect (buf, __len, __prot);
	  return buf;
	}
    }
  if (__flags & MAP_SHARED)
    {
      HANDLE hMapFile;
      HANDLE hFile = INVALID_HANDLE_VALUE;
      if (~__flags & MAP_ANONYMOUS)
	{
	  hFile = (HANDLE) _get_osfhandle (__fd);
	  hFile =
	    ReOpenFile (hFile, GENERIC_EXECUTE | GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0);
	  if (hFile == INVALID_HANDLE_VALUE)
	    {
	      _set_errno (EBADF);
	      return MAP_FAILED;
	    }
	}
      hMapFile = CreateFileMapping (hFile,
				    NULL,
				    flProtect, maxSize >> 32, maxSize, NULL);
      if (hMapFile == NULL)
	{
	  _set_errno (EPERM);
	  return MAP_FAILED;
	}
      buf = MapViewOfFileEx (hMapFile,
			     FILE_MAP_ALL_ACCESS | FILE_MAP_EXECUTE,
			     (off >> 31) >> 1, off, size, __addr);
      CloseHandle (hMapFile);
      if (buf == NULL)
	{
	  _set_errno (ENOMEM);
	  return MAP_FAILED;
	}
      mprotect (buf, __len, __prot);
      return buf;
    }
}

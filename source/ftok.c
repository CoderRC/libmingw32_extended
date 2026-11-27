#include <sys/ipc.h>
#include <windows.h>
#include <errno.h>

/* POSIX: the key comes from the identity of an existing file (st_dev and
   st_ino) and the low 8 bits of id, so every name for the same file gives
   the same key. On Windows the identity is the volume serial number and
   the 64-bit file index from GetFileInformationByHandle(). */

static int
errno_from_win32 (DWORD error)
{
  switch (error)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_NAME:
    case ERROR_INVALID_DRIVE:
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_PATHNAME:
      return ENOENT;
    case ERROR_ACCESS_DENIED:
    case ERROR_SHARING_VIOLATION:
      return EACCES;
    case ERROR_FILENAME_EXCED_RANGE:
      return ENAMETOOLONG;
    case ERROR_DIRECTORY:
      return ENOTDIR;
    case ERROR_CANT_RESOLVE_FILENAME:
      return ELOOP;
    default:
      return EIO;
    }
}

key_t
ftok (const char *pathname, int proj_id)
{
  DWORD initErrCode = GetLastError ();
  BY_HANDLE_FILE_INFORMATION info;
  HANDLE file;
  unsigned int mix;
  key_t key;

  if (!pathname)
    {
      _set_errno (EFAULT);
      return (key_t) - 1;
    }
  if (*pathname == '\0')
    {
      _set_errno (ENOENT);
      return (key_t) - 1;
    }

  /* No access rights are needed to read the file identity;
     FILE_FLAG_BACKUP_SEMANTICS lets directories be opened too. */
  file = CreateFileA (pathname, 0,
		      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (file == INVALID_HANDLE_VALUE)
    {
      int error = errno_from_win32 (GetLastError ());
      SetLastError (initErrCode);
      _set_errno (error);
      return (key_t) - 1;
    }
  if (!GetFileInformationByHandle (file, &info))
    {
      int error = errno_from_win32 (GetLastError ());
      CloseHandle (file);
      SetLastError (initErrCode);
      _set_errno (error);
      return (key_t) - 1;
    }
  CloseHandle (file);

  /* Fold the 32-bit volume serial and 64-bit file index into 24 bits */
  mix = info.dwVolumeSerialNumber;
  mix = mix * 0x9E3779B1U ^ info.nFileIndexHigh;
  mix = mix * 0x9E3779B1U ^ info.nFileIndexLow;
  mix ^= mix >> 15;
  mix *= 0x85EBCA6BU;
  mix ^= mix >> 13;

  key = (key_t) ((((unsigned int) proj_id & 0xFFU) << 24)
		 | (mix & 0xFFFFFFU));

  /* Never hand back IPC_PRIVATE or the error value */
  if (key == IPC_PRIVATE || key == (key_t) - 1)
    {
      key = (key_t) 1;
    }

  SetLastError (initErrCode);
  return key;
}

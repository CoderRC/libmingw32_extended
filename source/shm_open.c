#include <sys/mman.h>
#include <windows.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

int
shm_open (const char *__name, int __oflag, mode_t __mode)
{
  int error_value = 0;
  size_t i;
  size_t numerator;
  size_t denominator;
  char *filename;
  char *tempshm = "/Windows/Temp/shm/";
  char *systemdrive = getenv ("systemdrive");
  char *foldername =
    malloc (strlen (systemdrive) + strlen (tempshm) + sizeof (*foldername));

  memcpy (foldername, systemdrive, strlen (systemdrive));
  memcpy (foldername + strlen (systemdrive), tempshm, strlen (tempshm));

  foldername[strlen (systemdrive) + strlen (tempshm)] = 0;

  _get_errno (&error_value);
  mkdir (foldername);
  _set_errno (error_value);

  filename =
    malloc (strlen (systemdrive) + strlen (tempshm) + strlen (__name));
  memcpy (filename, foldername, strlen (systemdrive) + strlen (tempshm));
  memcpy (filename + strlen (systemdrive) + strlen (tempshm), __name,
	  strlen (__name));

  filename[strlen (systemdrive) + strlen (tempshm) + strlen (__name)] = 0;

  char *proccessed_path =
    _fullpath (NULL, filename,
	       strlen (systemdrive) + strlen (tempshm) + strlen (__name));
  numerator = 0;
  denominator = strlen (foldername);
  if (denominator <= strlen (proccessed_path))
    {
      for (i = 0; i != denominator; i++)
	{
	  numerator += proccessed_path[i] == foldername[i]
	    || foldername[i] == '\\' || foldername[i] == '/';
	}
    }
  if (numerator != denominator)
    {
      free (proccessed_path);
      free (filename);
      free (foldername);
      _set_errno (EINVAL);
      return -1;
    }

  int fd = open (filename, __oflag, __mode);
  _get_errno (&error_value);
  free (proccessed_path);
  free (filename);
  free (foldername);
  _set_errno (error_value);
  return fd;
}

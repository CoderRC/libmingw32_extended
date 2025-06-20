#include <windows.h>
#include <stdio.h>

static char dlerror_buffer[512] = { 0 };

char *
dlerror (void)
{
  DWORD err = GetLastError ();
  if (err == 0)
    {
      return NULL;
    }

  FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,
		  err,
		  MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		  dlerror_buffer, sizeof (dlerror_buffer), NULL);

  SetLastError (0);

  return dlerror_buffer;
}

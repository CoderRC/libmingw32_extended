#include <windows.h>
#include <stdio.h>

static
__declspec (thread)
     char dlerror_buffer[512] = { 0 };

char *
dlerror (void)
{
  DWORD err = GetLastError ();
  if (err == 0)
    {
      return NULL;
    }

  DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  DWORD length = FormatMessageA (flags,
				 NULL,
				 err,
				 MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
				 dlerror_buffer,
				 sizeof (dlerror_buffer),
				 NULL);

  if (length == 0)
    {
      // Failed to format the message, fallback to numeric error code string
      snprintf (dlerror_buffer, sizeof (dlerror_buffer),
		"Unknown error 0x%08lx", err);
    }
  else
    {
      // Trim trailing newline characters (CRLF)
      while (length > 0
	     && (dlerror_buffer[length - 1] == '\n'
		 || dlerror_buffer[length - 1] == '\r'))
	{
	  dlerror_buffer[--length] = '\0';
	}
    }

  SetLastError (0);

  return dlerror_buffer;
}

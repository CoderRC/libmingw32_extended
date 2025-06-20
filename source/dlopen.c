#include <dlfcn.h>
#include <windows.h>

static
__declspec (thread)
     char dlerror_buffer[512] = { 0 };

static void
set_dlerror_from_last_error (void)
{
  DWORD err = GetLastError ();
  if (err == 0)
    {
      dlerror_buffer[0] = '\0';
      return;
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
}

void *
dlopen (const char *__file, int __mode)
{
  if (!__file)
    {
      SetLastError (ERROR_INVALID_PARAMETER);
      set_dlerror_from_last_error ();
      return NULL;
    }

  DWORD dwFlags = 0;

  if (__mode & RTLD_LAZY)
    {
      dwFlags = DONT_RESOLVE_DLL_REFERENCES;
    }

  HMODULE module = LoadLibraryExA (__file, NULL, dwFlags);
  if (!module)
    {
      set_dlerror_from_last_error ();
      return NULL;
    }

  dlerror_buffer[0] = '\0';
  return (LPVOID) module;
}

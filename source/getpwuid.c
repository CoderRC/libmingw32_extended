#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <pwd.h>
#include <windows.h>
#include <dlfcn.h>
#include <lm.h>
#include <errno.h>
#include <stdio.h>

/* Typedef for NetUserGetInfo function pointer */
typedef NET_API_STATUS (WINAPI NetUserGetInfoFunc) (LPCWSTR servername,
						    LPCWSTR username,
						    DWORD level,
						    LPBYTE * bufptr);
NetUserGetInfoFunc NetUserGetInfo;

/* Typedef for NetApiBufferFree function pointer */
typedef NET_API_STATUS (WINAPI NetApiBufferFreeFunc) (LPVOID Buffer);
NetApiBufferFreeFunc NetApiBufferFree;

/* Windows stat always returns 0 for uid/gid */
#define STAT_UID 0
#define STAT_GID 0

/* Static storage for the returned passwd structure */
static struct passwd current_passwd = { 0 };

static char *
wcs_to_mbs_alloc (const WCHAR * wstr)
{
  if (!wstr)
    {
      return NULL;
    }
  int len = WideCharToMultiByte (CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
  if (len <= 0)
    {
      return NULL;
    }
  char *mbstr = (char *) malloc (len);
  if (mbstr)
    {
      WideCharToMultiByte (CP_ACP, 0, wstr, -1, mbstr, len, NULL, NULL);
    }
  return mbstr;
}

static char *
get_user_home_directory_alloc (void)
{
  char *buffer = NULL;
  DWORD len = 4096;		/* initial guess */
  DWORD required;

  /* Try environment variable USERPROFILE first */
  while (1)
    {
      char *newbuf = (char *) realloc (buffer, len);
      if (!newbuf)
	{
	  free (buffer);
	  return NULL;
	}
      buffer = newbuf;

      required = GetEnvironmentVariableA ("USERPROFILE", buffer, len);
      if (!required)
	{
	  /* Not found or error - break out to try registry */
	  break;
	}
      if (required < len)
	{
	  /* Success - buffer contains the string */
	  return buffer;	/* caller must free */
	}
      /* required >= len: buffer too small, required includes null */
      len = required + 1;	/* add one just in case for the future */
      /* loop again with larger buffer */
    }

  /* Try registry (Shell Folders\Personal) */
  HKEY hKey;
  LONG regErr = RegOpenKeyExA (HKEY_CURRENT_USER,
			       "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
			       0, KEY_READ, &hKey);
  if (regErr == ERROR_SUCCESS)
    {
      DWORD type;
      len = 4096;		/* reset guess */
      while (1)
	{
	  char *newbuf = (char *) realloc (buffer, len);
	  if (!newbuf)
	    {
	      free (buffer);
	      RegCloseKey (hKey);
	      return NULL;
	    }
	  buffer = newbuf;

	  DWORD dataSize = len;
	  regErr = RegQueryValueExA (hKey, "Personal", NULL, &type,
				     (LPBYTE) buffer, &dataSize);
	  if (regErr == ERROR_SUCCESS && type == REG_SZ)
	    {
	      RegCloseKey (hKey);
	      return buffer;	/* success */
	    }
	  if (regErr != ERROR_MORE_DATA)
	    {
	      /* Other error - break and fall back to default */
	      break;
	    }
	  /* dataSize contains required size (including null) */
	  len = dataSize;
	  /* loop again */
	}
      RegCloseKey (hKey);
    }

  /* Fallback to default directory */
  const char *default_dir = "C:\\Users\\Default";
  size_t default_len = strlen (default_dir) + 1;
  char *newbuf = (char *) realloc (buffer, default_len);
  if (!newbuf)
    {
      free (buffer);
      return NULL;
    }
  buffer = newbuf;
  memcpy (buffer, default_dir, default_len);
  return buffer;
}

static char *
get_user_shell_alloc (void)
{
  char *buffer = NULL;
  DWORD len = 4096;		/* initial guess */
  DWORD required;

  /* Try environment variable COMSPEC first */
  while (1)
    {
      char *newbuf = (char *) realloc (buffer, len);
      if (!newbuf)
	{
	  free (buffer);
	  return NULL;
	}
      buffer = newbuf;

      required = GetEnvironmentVariableA ("COMSPEC", buffer, len);
      if (!required)
	{
	  /* Not found - break to use default */
	  break;
	}
      if (required < len)
	{
	  /* Success */
	  return buffer;
	}
      /* Buffer too small */
      len = required + 1;
      /* loop again */
    }

  /* Fallback to default shell */
  const char *default_shell = "C:\\Windows\\System32\\cmd.exe";
  size_t default_len = strlen (default_shell) + 1;
  char *newbuf = (char *) realloc (buffer, default_len);
  if (!newbuf)
    {
      free (buffer);
      return NULL;
    }
  buffer = newbuf;
  memcpy (buffer, default_shell, default_len);
  return buffer;
}

static void
free_passwd_fields (struct passwd *pw)
{
  void *memory;
  if (!pw)
    {
      return;
    }

  if (pw->pw_name)
    {
      memory = pw->pw_name;
      pw->pw_name = NULL;
      free (memory);
    }
  if (pw->pw_dir)
    {
      memory = pw->pw_dir;
      pw->pw_dir = NULL;
      free (memory);
    }
  if (pw->pw_shell)
    {
      memory = pw->pw_shell;
      pw->pw_shell = NULL;
      free (memory);
    }
}

struct passwd *
getpwuid (uid_t __uid)
{
  LPUSER_INFO_4 user_info = NULL;
  NET_API_STATUS status;
  WCHAR *username_w = NULL;
  char *username_a = NULL;
  char *home_dir = NULL;
  char *shell = NULL;
  DWORD username_len;

  if (__uid != 0 && __uid != STAT_UID)
    {
      return NULL;
    }

  free_passwd_fields (__436F64657252436C69626D696E677733325F657874656E646564.
		      current_passwd);

  /* Dynamically allocate username_w, retrying if buffer too small */
  username_len = UNLEN + 1;	/* start with typical size */
  username_w = (WCHAR *) malloc (username_len * sizeof (*username_w));
  if (!username_w)
    {
      _set_errno (ENOMEM);
      return NULL;
    }

  if (!GetUserNameW (username_w, &username_len))
    {
      DWORD err = GetLastError ();
      if (err != ERROR_INSUFFICIENT_BUFFER)
	{
	  free (username_w);
	  _set_errno (ENOENT);
	  return NULL;
	}

      /* Buffer too small - username_len now contains required size */
      WCHAR *new_w =
	(WCHAR *) realloc (username_w, username_len * sizeof (*username_w));
      if (!new_w)
	{
	  free (username_w);
	  _set_errno (ENOMEM);
	  return NULL;
	}
      username_w = new_w;

      if (!GetUserNameW (username_w, &username_len))
	{
	  free (username_w);
	  _set_errno (ENOENT);
	  return NULL;
	}
    }

  /* Convert username to multibyte (allocate exact size) */
  int name_mb_len =
    WideCharToMultiByte (CP_ACP, 0, username_w, -1, NULL, 0, NULL, NULL);
  if (name_mb_len <= 0)
    {
      free (username_w);
      _set_errno (EIO);		/* conversion failure */
      return NULL;
    }
  username_a = (char *) malloc (name_mb_len * sizeof (*username_a));
  if (!username_a)
    {
      free (username_w);
      _set_errno (ENOMEM);
      return NULL;
    }
  WideCharToMultiByte (CP_ACP, 0, username_w, -1, username_a, name_mb_len,
		       NULL, NULL);

  /* Check if initialization is needed */
  if (!__436F64657252436C69626D696E677733325F657874656E646564.netapi32_handle)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_lock ();

      /* Re-check after acquiring lock */
      if (!__436F64657252436C69626D696E677733325F657874656E646564.
	  netapi32_handle)
	{
	  __436F64657252436C69626D696E677733325F657874656E646564.
	    netapi32_handle = dlopen ("netapi32.dll", RTLD_NOW);
	  if (!__436F64657252436C69626D696E677733325F657874656E646564.
	      netapi32_handle)
	    {
	      __436F64657252436C69626D696E677733325F657874656E646564_unlock
		();
	      return NULL;
	    }
	  __436F64657252436C69626D696E677733325F657874656E646564.
	    current_passwd = &current_passwd;
	}

      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
    }

  /* Check environment variables HOME and SHELL */
  home_dir = getenv ("HOME");
  shell = getenv ("SHELL");

  if (home_dir && *home_dir != '\0')
    {
      // Create a mutable copy of the HOME path (getenv returns read-only memory)
      char *path_copy = _strdup (home_dir);
      home_dir = path_copy;
      if (path_copy)
	{
	  // Remove any trailing slashes to simplify parent extraction
	  size_t len = strlen (path_copy);
	  while (len > 0
		 && (path_copy[len - 1] == '\\' || path_copy[len - 1] == '/'))
	    {
	      path_copy[--len] = '\0';
	    }

	  // Find the last slash (either backslash or forward slash)
	  char *last_slash = strrchr (path_copy, '\\');
	  char *alt_slash = strrchr (path_copy, '/');
	  if (alt_slash > last_slash)
	    {
	      last_slash = alt_slash;
	    }

	  home_dir = last_slash;
	  if (last_slash)
	    {
	      // Extract the parent directory (up to, but not including, the slash)
	      size_t parent_len = last_slash - path_copy;
	      path_copy[parent_len] = '\0';

	      // Build final path: parent + backslash + current username
	      size_t final_len = parent_len + 1 + strlen (username_a) + 1;
	      home_dir = (char *) malloc (final_len);
	      if (home_dir)
		{
		  snprintf (home_dir, final_len, "%s\\%s", path_copy, username_a);	// use this new path
		}
	    }
	  free (path_copy);
	}
    }
  else
    {
      home_dir = 0;		// HOME is empty or not set
    }

  if (shell && *shell != '\0')
    {
      shell = _strdup (shell);
    }
  else
    {
      shell = 0;
    }

  if (!home_dir || !shell)
    {
      NetUserGetInfoFunc *_NetUserGetInfo =
	dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	       netapi32_handle, "NetUserGetInfo");

      NetApiBufferFreeFunc *_NetApiBufferFree =
	dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	       netapi32_handle, "NetApiBufferFree");

      if (!_NetUserGetInfo || !_NetApiBufferFree)
	{
	  free (username_w);
	  username_w = 0;
	}

      if (username_w)
	{
	  status =
	    _NetUserGetInfo (NULL, username_w, 4, (LPBYTE *) & user_info);
	  free (username_w);
	  if (status != NERR_Success)
	    {
	      username_w = 0;
	    }
	}

      if (!home_dir)
	{
	  /* Obtain home directory (malloced) */
	  if (username_w && user_info->usri4_home_dir
	      && *user_info->usri4_home_dir)
	    {
	      home_dir = wcs_to_mbs_alloc (user_info->usri4_home_dir);
	    }
	  else
	    {
	      home_dir = get_user_home_directory_alloc ();
	    }
	  if (!home_dir)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      free (username_a);
	      _set_errno (ENOMEM);
	      return NULL;
	    }
	}

      if (!shell)
	{
	  /* Obtain shell (malloced) */
	  if (username_w && user_info->usri4_script_path
	      && *user_info->usri4_script_path)
	    {
	      shell = wcs_to_mbs_alloc (user_info->usri4_script_path);
	    }
	  else
	    {
	      shell = get_user_shell_alloc ();
	    }
	  if (!shell)
	    {
	      free (home_dir);
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      free (username_a);
	      _set_errno (ENOMEM);
	      return NULL;
	    }
	}

      if (username_w)
	{
	  _NetApiBufferFree (user_info);
	}
    }
  else
    {
      free (username_w);
    }

  /* Fill the static passwd structure */
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd->
    pw_name = username_a;
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd->
    pw_uid = STAT_UID;
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd->
    pw_gid = STAT_GID;
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd->
    pw_dir = home_dir;
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd->
    pw_shell = shell;

  return __436F64657252436C69626D696E677733325F657874656E646564.
    current_passwd;
}

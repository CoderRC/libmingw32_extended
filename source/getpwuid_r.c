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

static size_t
wcs_to_mbs_buf (const WCHAR * wstr, char *buf, size_t bufsize)
{
  int needed =
    WideCharToMultiByte (CP_ACP, 0, wstr, -1, buf, (int) bufsize, NULL, NULL);
  if (needed <= 0)
    {
      return 0;			// conversion error
    }
  return needed;		// bytes written (including null)
}

static size_t
get_user_home_directory_buf (char *buf, size_t bufsize)
{
  /* Try USERPROFILE first */
  DWORD needed =
    GetEnvironmentVariableA ("USERPROFILE", buf, (DWORD) bufsize);
  if (needed > 0)
    {
      if (needed < bufsize)
	return needed + 1;	// success, including null
      return (size_t) needed + 1;	// required size (includes null)
    }

  /* Try registry (Personal folder) */
  HKEY hKey;
  LONG regErr = RegOpenKeyExA (HKEY_CURRENT_USER,
			       "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
			       0, KEY_READ, &hKey);
  if (regErr == ERROR_SUCCESS)
    {
      DWORD type;
      DWORD dataSize = (DWORD) bufsize;
      regErr = RegQueryValueExA (hKey, "Personal", NULL, &type,
				 (LPBYTE) buf, &dataSize);
      RegCloseKey (hKey);
      if (regErr == ERROR_SUCCESS && type == REG_SZ)
	{
	  return strlen (buf) + 1;	// success
	}
      if (regErr == ERROR_MORE_DATA)
	{
	  return (size_t) dataSize;	// required size
	}
    }

  /* Fallback to default */
  const char *default_dir = "C:\\Users\\Default";
  size_t default_len = strlen (default_dir) + 1;
  if (default_len <= bufsize)
    {
      memcpy (buf, default_dir, default_len);
      return default_len;
    }
  return default_len;		// required size
}

static size_t
get_user_shell_buf (char *buf, size_t bufsize)
{
  /* Try COMSPEC */
  DWORD needed = GetEnvironmentVariableA ("COMSPEC", buf, (DWORD) bufsize);
  if (needed > 0)
    {
      if (needed < bufsize)
	return needed + 1;
      return (size_t) needed + 1;
    }

  /* Fallback to cmd.exe */
  const char *default_shell = "C:\\Windows\\System32\\cmd.exe";
  size_t default_len = strlen (default_shell) + 1;
  if (default_len <= bufsize)
    {
      memcpy (buf, default_shell, default_len);
      return default_len;
    }
  return default_len;
}

int
getpwuid_r (uid_t __uid,
	    struct passwd *__restrict __pwd,
	    char *__restrict __buf, size_t __buflen,
	    struct passwd **__restrict __result)
{
  LPUSER_INFO_4 user_info = NULL;
  NET_API_STATUS status;
  WCHAR *username_w = NULL;
  char *username_a = NULL;
  char *home_dir = NULL;
  char *shell = NULL;
  DWORD username_len;
  char *p = __buf;
  size_t remaining = __buflen;
  size_t len;
  size_t needed;

  if (!__pwd || !__result || !__buf)
    {
      _set_errno (EINVAL);
      return EINVAL;
    }

  *__result = NULL;

  if (__uid != 0 && __uid != STAT_UID)
    {
      return 0;
    }

  /* Dynamically allocate username_w, retrying if buffer too small */
  username_len = UNLEN + 1;	/* start with typical size */
  username_w = (WCHAR *) malloc (username_len * sizeof (*username_w));
  if (!username_w)
    {
      _set_errno (ENOMEM);
      return ENOMEM;
    }

  if (!GetUserNameW (username_w, &username_len))
    {
      DWORD err = GetLastError ();
      if (err != ERROR_INSUFFICIENT_BUFFER)
	{
	  free (username_w);
	  _set_errno (ENOENT);
	  return ENOENT;
	}

      /* Buffer too small - username_len now contains required size */
      WCHAR *new_w =
	(WCHAR *) realloc (username_w, username_len * sizeof (*username_w));
      if (!new_w)
	{
	  free (username_w);
	  _set_errno (ENOMEM);
	  return ENOMEM;
	}
      username_w = new_w;

      if (!GetUserNameW (username_w, &username_len))
	{
	  free (username_w);
	  _set_errno (ENOENT);
	  return ENOENT;
	}
    }

  /* Convert username to multibyte (allocate exact size) */
  int name_mb_len =
    WideCharToMultiByte (CP_ACP, 0, username_w, -1, NULL, 0, NULL, NULL);
  if (name_mb_len <= 0)
    {
      free (username_w);
      _set_errno (EIO);		/* conversion failure */
      return EIO;
    }
  len = name_mb_len * sizeof (*p);
  if (remaining < len)
    {
      _set_errno (ERANGE);
      return ERANGE;		// buffer too small
    }
  WideCharToMultiByte (CP_ACP, 0, username_w, -1, p, name_mb_len, NULL, NULL);
  __pwd->pw_name = p;
  username_a = p;
  p += len;
  remaining -= len;

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
	      return ENOEXEC;
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
	      len = parent_len + 1 + strlen (username_a) + 1;
	      if (remaining < len)
		{
		  _set_errno (ERANGE);
		  return ERANGE;	// buffer too small
		}
	      home_dir = p;
	      snprintf (p, len, "%s\\%s", path_copy, username_a);	// use this new path
	      p += len;
	      remaining -= len;
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
      len = strlen (shell) + 1;
      if (remaining < len)
	{
	  _set_errno (ERANGE);
	  return ERANGE;	// buffer too small
	}
      strcpy (p, shell);
      shell = p;
      p += len;
      remaining -= len;
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
	      if (status == NERR_UserNotFound)
		{
		  _set_errno (ENOENT);
		  *__result = NULL;
		  return ENOENT;
		}
	    }
	}

      if (!home_dir)
	{
	  /* Obtain home directory */
	  if (username_w && user_info->usri4_home_dir
	      && *user_info->usri4_home_dir)
	    {
	      // Wide char -> multi-byte: query required length
	      needed = wcs_to_mbs_buf (user_info->usri4_home_dir, NULL, 0);
	    }
	  else
	    {
	      needed = get_user_home_directory_buf (NULL, 0);
	    }
	  if (!needed)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      _set_errno (ENOMEM);
	      return ENOMEM;
	    }
	  if (remaining < needed)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      _set_errno (ERANGE);
	      return ERANGE;	// buffer too small
	    }
	  // Now write directly into p
	  if (username_w && user_info->usri4_home_dir
	      && *user_info->usri4_home_dir)
	    {
	      wcs_to_mbs_buf (user_info->usri4_home_dir, p, remaining);
	    }
	  else
	    {
	      get_user_home_directory_buf (p, remaining);
	    }
	  home_dir = p;
	  p += needed;
	  remaining -= needed;
	  if (!home_dir)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      if (shell)
		{
		  free (shell);
		}
	      _set_errno (ENOMEM);
	      return ENOMEM;
	    }
	}

      if (!shell)
	{
	  /* Obtain shell */
	  if (username_w && user_info->usri4_script_path
	      && *user_info->usri4_script_path)
	    {
	      needed = wcs_to_mbs_buf (user_info->usri4_script_path, NULL, 0);
	    }
	  else
	    {
	      needed = get_user_shell_buf (NULL, 0);
	    }
	  if (!needed)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      _set_errno (ENOMEM);
	      return ENOMEM;
	    }
	  if (remaining < needed)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      _set_errno (ERANGE);
	      return ERANGE;	// buffer too small
	    }
	  // Now write directly into p
	  if (username_w && user_info->usri4_script_path
	      && *user_info->usri4_script_path)
	    {
	      wcs_to_mbs_buf (user_info->usri4_script_path, p, remaining);
	    }
	  else
	    {
	      get_user_shell_buf (p, remaining);
	    }
	  shell = p;
	  p += needed;
	  remaining -= needed;
	  if (!shell)
	    {
	      if (username_w)
		{
		  _NetApiBufferFree (user_info);
		}
	      _set_errno (ENOMEM);
	      return ENOMEM;
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
  __pwd->pw_uid = STAT_UID;
  __pwd->pw_gid = STAT_GID;
  __pwd->pw_dir = home_dir;
  __pwd->pw_shell = shell;

  *__result = __pwd;
  return 0;
}

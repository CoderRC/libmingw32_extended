#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <pwd.h>
#include <dlfcn.h>
#include <stdlib.h>

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

void
endpwent ()
{
  void *netapi32_handle =
    __436F64657252436C69626D696E677733325F657874656E646564.netapi32_handle;
  struct passwd *pw;
  if (netapi32_handle)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_lock ();
      netapi32_handle =
	__436F64657252436C69626D696E677733325F657874656E646564.
	netapi32_handle;

      /* Re-check after acquiring lock */
      if (netapi32_handle)
	{
	  __436F64657252436C69626D696E677733325F657874656E646564.
	    netapi32_handle = 0;
	  dlclose (netapi32_handle);
	}

      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
    }
  pw = __436F64657252436C69626D696E677733325F657874656E646564.current_passwd;
  __436F64657252436C69626D696E677733325F657874656E646564.current_passwd = 0;
  free_passwd_fields (pw);
  __436F64657252436C69626D696E677733325F657874656E646564.passwd_index = 0;
}

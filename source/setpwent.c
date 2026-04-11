#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <pwd.h>
#include <dlfcn.h>
#include <stdlib.h>

/* Static storage for the returned passwd structure */
static struct passwd current_passwd = { 0 };

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
setpwent ()
{
  struct passwd *pw;
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
	      return;
	    }
	  __436F64657252436C69626D696E677733325F657874656E646564.
	    current_passwd = &current_passwd;
	}

      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
    }
  pw = __436F64657252436C69626D696E677733325F657874656E646564.current_passwd;
  free_passwd_fields (pw);
  __436F64657252436C69626D696E677733325F657874656E646564.passwd_index = 0;
}

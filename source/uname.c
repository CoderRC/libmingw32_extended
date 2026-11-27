#include <sys/utsname.h>
#include <windows.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* Typedef for RtlGetVersion function pointer (ntdll.dll). Unlike
   GetVersionEx it is not capped by the program's manifest. */
typedef LONG (WINAPI RtlGetVersionFunc) (OSVERSIONINFOEXW * info);

/* Typedef for IsWow64Process2 function pointer (kernel32.dll, Windows 10
   1709 and later). It reports the real machine even under emulation. */
typedef BOOL (WINAPI IsWow64Process2Func) (HANDLE process,
					   USHORT * process_machine,
					   USHORT * native_machine);

#ifndef IMAGE_FILE_MACHINE_ARM64
#define IMAGE_FILE_MACHINE_ARM64 0xAA64
#endif
#ifndef IMAGE_FILE_MACHINE_ARMNT
#define IMAGE_FILE_MACHINE_ARMNT 0x01c4
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

static void
copy_field (char *field, const char *value)
{
  snprintf (field, _UTSNAME_LENGTH, "%s", value);
}

static void
get_version (DWORD *major, DWORD *minor, DWORD *build)
{
  OSVERSIONINFOEXW info;
  void *ntdll = dlopen ("ntdll.dll", RTLD_NOW);

  *major = 0;
  *minor = 0;
  *build = 0;
  if (ntdll)
    {
      RtlGetVersionFunc *_RtlGetVersion = dlsym (ntdll, "RtlGetVersion");
      memset (&info, 0, sizeof (info));
      info.dwOSVersionInfoSize = sizeof (info);
      if (_RtlGetVersion && _RtlGetVersion (&info) == 0)
	{
	  *major = info.dwMajorVersion;
	  *minor = info.dwMinorVersion;
	  *build = info.dwBuildNumber;
	}
      dlclose (ntdll);
    }
}

/* Update build revision (the number after the build in winver), or 0 */
static DWORD
get_revision (void)
{
  HKEY key;
  DWORD revision = 0;
  DWORD size = sizeof (revision);
  DWORD type;

  if (RegOpenKeyExA (HKEY_LOCAL_MACHINE,
		     "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
		     KEY_READ, &key) == ERROR_SUCCESS)
    {
      if (RegQueryValueExA (key, "UBR", NULL, &type, (LPBYTE) & revision,
			    &size) != ERROR_SUCCESS || type != REG_DWORD)
	{
	  revision = 0;
	}
      RegCloseKey (key);
    }
  return revision;
}

static const char *
get_machine (void)
{
  SYSTEM_INFO info;
  USHORT process_machine = 0;
  USHORT native_machine = 0;
  void *kernel32 = dlopen ("kernel32.dll", RTLD_NOW);

  if (kernel32)
    {
      IsWow64Process2Func *_IsWow64Process2 =
	dlsym (kernel32, "IsWow64Process2");
      if (!_IsWow64Process2
	  || !_IsWow64Process2 (GetCurrentProcess (), &process_machine,
				&native_machine))
	{
	  native_machine = 0;
	}
      dlclose (kernel32);
    }

  switch (native_machine)
    {
    case IMAGE_FILE_MACHINE_AMD64:
      return "x86_64";
    case IMAGE_FILE_MACHINE_I386:
      return "i686";
    case IMAGE_FILE_MACHINE_ARM64:
      return "aarch64";
    case IMAGE_FILE_MACHINE_ARMNT:
      return "armv7l";
    }

  /* Older Windows: GetNativeSystemInfo sees through WOW64 */
  GetNativeSystemInfo (&info);
  switch (info.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
      return "x86_64";
    case PROCESSOR_ARCHITECTURE_INTEL:
      return "i686";
    case PROCESSOR_ARCHITECTURE_ARM64:
      return "aarch64";
    case PROCESSOR_ARCHITECTURE_ARM:
      return "armv7l";
    case PROCESSOR_ARCHITECTURE_IA64:
      return "ia64";
    }
  return "unknown";
}

int
uname (struct utsname *__name)
{
  DWORD initErrCode = GetLastError ();
  DWORD major, minor, build, revision;
  DWORD size;
  char text[_UTSNAME_LENGTH];

  if (!__name)
    {
      _set_errno (EFAULT);
      return -1;
    }

  /* Clear the padding after each string too, so no stack garbage is
     handed out and two calls compare equal with memcmp */
  memset (__name, 0, sizeof (*__name));

  /* Same name Windows puts in the OS environment variable */
  copy_field (__name->sysname, "Windows_NT");

  size = sizeof (text);
  if (!GetComputerNameExA (ComputerNameDnsHostname, text, &size))
    {
      size = sizeof (text);
      if (!GetComputerNameA (text, &size))
	{
	  text[0] = '\0';
	}
    }
  copy_field (__name->nodename, text);

  get_version (&major, &minor, &build);
  snprintf (__name->release, _UTSNAME_LENGTH, "%lu.%lu.%lu",
	    (unsigned long) major, (unsigned long) minor,
	    (unsigned long) build);

  revision = get_revision ();
  if (revision)
    {
      snprintf (__name->version, _UTSNAME_LENGTH, "%lu.%lu",
		(unsigned long) build, (unsigned long) revision);
    }
  else
    {
      snprintf (__name->version, _UTSNAME_LENGTH, "%lu",
		(unsigned long) build);
    }

  copy_field (__name->machine, get_machine ());

  SetLastError (initErrCode);
  return 0;
}

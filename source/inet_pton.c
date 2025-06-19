#include <arpa/inet.h>
#include <dlfcn.h>

// Define the function pointer type for inet_pton
typedef int (*inet_pton_t) (int af, const char *src, void *dst);

int
inet_pton (int af, const char *src, void *dst)
{
  // Load the ws2_32.dll library
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  if (!Ws2_32)
    {
      return -1;		// Return error if DLL cannot be loaded
    }

  // Get the inet_pton function from the DLL
  inet_pton_t _inet_pton = (inet_pton_t) dlsym (Ws2_32, "inet_pton");
  if (!_inet_pton)
    {
      dlclose (Ws2_32);		// Close the DLL if the function is not found
      return -1;
    }

  // Call the Windows inet_pton function
  int result = _inet_pton (af, src, dst);

  // Clean up
  dlclose (Ws2_32);
  return result;
}

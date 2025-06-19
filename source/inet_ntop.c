#include <arpa/inet.h>
#include <dlfcn.h>

// Define the function pointer type for inet_ntop
typedef const char *(*inet_ntop_t) (int af, const void *src, char *dst,
				    socklen_t size);

const char *
inet_ntop (int af, const void *src, char *dst, socklen_t size)
{
  // Load the ws2_32.dll library
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  if (!Ws2_32)
    {
      return NULL;		// Return NULL if DLL cannot be loaded
    }

  // Get the inet_ntop function from the DLL
  inet_ntop_t _inet_ntop = (inet_ntop_t) dlsym (Ws2_32, "inet_ntop");
  if (!_inet_ntop)
    {
      dlclose (Ws2_32);		// Close the DLL if the function is not found
      return NULL;
    }

  // Call the Windows inet_ntop function
  const char *result = _inet_ntop (af, src, dst, size);

  // Clean up
  dlclose (Ws2_32);
  return result;
}

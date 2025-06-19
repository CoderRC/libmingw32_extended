#include <arpa/inet.h>
#include <dlfcn.h>

// Define the function pointer type for inet_addr
typedef unsigned long (*inet_addr_t) (const char *cp);

in_addr_t
inet_addr (const char *cp)
{
  // Load the ws2_32.dll library
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  if (!Ws2_32)
    {
      return INADDR_NONE;	// Return error if DLL cannot be loaded
    }

  // Get the inet_addr function from the DLL
  inet_addr_t _inet_addr = (inet_addr_t) dlsym (Ws2_32, "inet_addr");
  if (!_inet_addr)
    {
      dlclose (Ws2_32);		// Close the DLL if the function is not found
      return INADDR_NONE;
    }

  // Call the Windows inet_addr function
  unsigned long result = _inet_addr (cp);

  // Clean up
  dlclose (Ws2_32);
  return (in_addr_t) result;
}

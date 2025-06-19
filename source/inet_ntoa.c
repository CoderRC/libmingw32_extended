#include <arpa/inet.h>
#include <dlfcn.h>

// Define the function pointer type for inet_ntoa
typedef char *(*inet_ntoa_t) (struct in_addr in);

char *
inet_ntoa (struct in_addr in)
{
  // Load the ws2_32.dll library
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  if (!Ws2_32)
    {
      return NULL;		// Return NULL if DLL cannot be loaded
    }

  // Get the inet_ntoa function from the DLL
  inet_ntoa_t _inet_ntoa = (inet_ntoa_t) dlsym (Ws2_32, "inet_ntoa");
  if (!_inet_ntoa)
    {
      dlclose (Ws2_32);		// Close the DLL if the function is not found
      return NULL;
    }

  // Call the Windows inet_ntoa function
  char *result = _inet_ntoa (in);

  // Clean up
  dlclose (Ws2_32);
  return result;
}

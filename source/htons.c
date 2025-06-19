#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void *HANDLE;
typedef HANDLE SOCKET;

uint16_t
htons (uint16_t hostshort)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  fn_htons_t _htons = (fn_htons_t) dlsym (Ws2_32, "htons");
  uint16_t result = _htons (hostshort);
  dlclose (Ws2_32);
  return result;
}

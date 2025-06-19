#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void *HANDLE;
typedef HANDLE SOCKET;

uint32_t
ntohl (uint32_t netlong)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  fn_ntohl_t _ntohl = (fn_ntohl_t) dlsym (Ws2_32, "ntohl");
  uint32_t result = _ntohl (netlong);
  dlclose (Ws2_32);
  return result;
}

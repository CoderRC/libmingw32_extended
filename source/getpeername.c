#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void *HANDLE;
typedef HANDLE SOCKET;

int
getpeername (int socket, struct sockaddr *address, socklen_t * address_len)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  int (*_getpeername) (SOCKET s, struct sockaddr * addr, int *namelen) =
    dlsym (Ws2_32, "getpeername");
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  int result = _getpeername (s, address, address_len);
  dlclose (Ws2_32);
  return result;
}

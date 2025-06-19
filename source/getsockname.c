#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void *HANDLE;
typedef HANDLE SOCKET;

int
getsockname (int socket, struct sockaddr *address, socklen_t * address_len)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  int (*_getsockname) (SOCKET s, struct sockaddr * addr, int *namelen) =
    dlsym (Ws2_32, "getsockname");
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  int result = _getsockname (s, address, address_len);
  dlclose (Ws2_32);
  return result;
}

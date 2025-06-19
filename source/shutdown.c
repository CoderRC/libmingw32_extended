#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void *HANDLE;
typedef HANDLE SOCKET;

int
shutdown (int socket, int how)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  int (*_shutdown) (SOCKET s, int how) = dlsym (Ws2_32, "shutdown");
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  int result = _shutdown (s, how);
  dlclose (Ws2_32);
  return result;
}

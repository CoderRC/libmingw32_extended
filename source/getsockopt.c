#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void *HANDLE;
typedef HANDLE SOCKET;

int
getsockopt (int socket, int level, int option_name, void *option_value,
	    socklen_t * option_len)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  int (*_getsockopt) (SOCKET s, int level, int optname, char *optval,
		      int *optlen) = dlsym (Ws2_32, "getsockopt");
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  int result = _getsockopt (s, level, option_name, option_value, option_len);
  dlclose (Ws2_32);
  return result;
}

#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void *HANDLE;
typedef HANDLE SOCKET;

int
setsockopt (int socket, int level, int option_name, const void *option_value,
	    socklen_t option_len)
{
  void *Ws2_32 = dlopen ("ws2_32.dll", RTLD_LAZY);
  int (*_setsockopt) (SOCKET s, int level, int optname, const char *optval,
		      int optlen) = dlsym (Ws2_32, "setsockopt");
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  int result = _setsockopt (s, level, option_name, option_value, option_len);
  //CloseHandle(s);
  dlclose (Ws2_32);
  return result;
}

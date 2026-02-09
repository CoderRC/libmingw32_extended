#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef unsigned short WORD;
typedef void *HANDLE;
typedef HANDLE SOCKET;

typedef struct _WSADATA
{
  WORD wVersion;
  WORD wHighVersion;
  char szDescription[256];
  char szSystemStatus[128];
  unsigned short iMaxSockets;
  unsigned short iMaxUdpDg;
  char *lpVendorInfo;
} WSADATA;

int
accept (int socket, struct sockaddr *address, socklen_t * address_len)
{
  /* Check if initialization is needed */
  if (!__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle)
    {
      __436F64657252436C69626D696E677733325F657874656E646564_lock ();

      /* Re-check after acquiring lock */
      if (!__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle)
	{
	  __436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle
	    = dlopen ("ws2_32.dll", RTLD_NOW);
	  if (!__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle)
	    {
	      __436F64657252436C69626D696E677733325F657874656E646564_unlock
		();
	      return -1;
	    }

	  /* Get WSAStartup function pointer */
	  int (*WSAStartup) (WORD wVersionRequested, WSADATA * lpWSAData) =
	    dlsym
	    (__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
	     "WSAStartup");

	  WSADATA wsaData;
	  int result = WSAStartup (0x202, &wsaData);
	  if (result)
	    {
	      dlclose
		(__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle);
	      __436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle
		= NULL;
	      __436F64657252436C69626D696E677733325F657874656E646564_unlock
		();
	      return -1;
	    }
	}

      __436F64657252436C69626D696E677733325F657874656E646564_unlock ();
    }

  /* Get accept function pointer using dlsym */
  SOCKET (*_accept) (SOCKET s, struct sockaddr * addr, int *addrlen) =
    dlsym
    (__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
     "accept");

  if (!_accept)
    {
      return -1;
    }

  SOCKET s = (SOCKET) _get_osfhandle (socket);
  SOCKET out = _accept (s, address, address_len);

  return _open_osfhandle ((intptr_t) out, O_RDWR);
}

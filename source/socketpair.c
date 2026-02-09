#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short WORD;
typedef void *HANDLE;
typedef HANDLE SOCKET;
#define INVALID_SOCKET (SOCKET)(-1)
#define SOCKET_ERROR (-1)
#define SD_BOTH 2

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
socketpair (int domain, int type, int protocol, int socket_vector[2])
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

  // Get required function pointers
  SOCKET (*_socket) (int domain, int type, int protocol) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "socket");
  int (*_bind) (SOCKET s, const struct sockaddr * name, int namelen) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "bind");
  int (*_listen) (SOCKET s, int backlog) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "listen");
  int (*_getsockname) (SOCKET s, struct sockaddr * name, int *namelen) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "getsockname");
  int (*_connect) (SOCKET s, const struct sockaddr * name, int namelen) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "connect");
  SOCKET (*_accept) (SOCKET s, struct sockaddr * addr, int *addrlen) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "accept");
  int (*_closesocket) (SOCKET s) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "closesocket");
  unsigned long (*_htonl) (unsigned long) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "htonl");
  unsigned short (*_htons) (unsigned short) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "htons");
  int (*_ioctlsocket) (SOCKET s, long cmd, unsigned long *argp) =
    dlsym (__436F64657252436C69626D696E677733325F657874656E646564.
	   ws2_32_handle, "ioctlsocket");

  if (!_socket || !_bind || !_listen || !_getsockname || !_connect ||
      !_accept || !_closesocket || !_htonl || !_htons || !_ioctlsocket)
    {
      return -1;
    }

  // Create listening socket
  SOCKET listener = _socket (domain, type, protocol);
  if (listener == INVALID_SOCKET)
    {
      return -1;
    }

  // Set up address structure
  struct sockaddr_in addr;
  memset (&addr, 0, sizeof (addr));
  addr.sin_family = domain;
  addr.sin_addr.s_addr = _htonl (INADDR_LOOPBACK);	// 127.0.0.1
  addr.sin_port = 0;		// Let OS choose port

  // Bind to localhost with random port
  if (_bind (listener, (struct sockaddr *) &addr, sizeof (addr)) ==
      SOCKET_ERROR)
    {
      _closesocket (listener);
      return -1;
    }

  // Get the assigned port
  int addrlen = sizeof (addr);
  if (_getsockname (listener, (struct sockaddr *) &addr, &addrlen) ==
      SOCKET_ERROR)
    {
      _closesocket (listener);
      return -1;
    }

  // Start listening
  if (_listen (listener, 1) == SOCKET_ERROR)
    {
      _closesocket (listener);
      return -1;
    }

  // Create client socket
  SOCKET client = _socket (domain, type, protocol);
  if (client == INVALID_SOCKET)
    {
      _closesocket (listener);
      return -1;
    }

  // On Windows, we need to set non-blocking for connect
  unsigned long mode = 1;	// Non-blocking
  if (_ioctlsocket (client, 0x8004667E /* FIONBIO */ , &mode) == SOCKET_ERROR)
    {
      _closesocket (client);
      _closesocket (listener);
      return -1;
    }

  // Connect to the listener (non-blocking)
  int connect_result =
    _connect (client, (struct sockaddr *) &addr, sizeof (addr));

  // Restore blocking mode
  mode = 0;
  _ioctlsocket (client, 0x8004667E /* FIONBIO */ , &mode);

  // Accept the connection
  SOCKET server = _accept (listener, NULL, NULL);
  if (server == INVALID_SOCKET)
    {
      _closesocket (client);
      _closesocket (listener);
      return -1;
    }

  // We don't need the listener anymore
  _closesocket (listener);

  // Convert sockets to file descriptors
  int fd1 = _open_osfhandle ((intptr_t) client, O_RDWR);
  int fd2 = _open_osfhandle ((intptr_t) server, O_RDWR);

  if (fd1 == -1 || fd2 == -1)
    {
      if (fd1 != -1)
	close (fd1);
      if (fd2 != -1)
	close (fd2);
      _closesocket (client);
      _closesocket (server);
      return -1;
    }

  socket_vector[0] = fd1;
  socket_vector[1] = fd2;

  return 0;
}

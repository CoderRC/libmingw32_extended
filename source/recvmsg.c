#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>

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

/* Windows-specific types needed for WSARecvMsg */
typedef struct _WSABUF
{
  unsigned long len;
  char *buf;
} WSABUF, *LPWSABUF;

typedef struct _WSAMSG
{
  void *name;			/* Socket address */
  int namelen;			/* Length of socket address */
  LPWSABUF lpBuffers;		/* Data buffer array */
  unsigned long dwBufferCount;	/* Number of elements in the array */
  WSABUF Control;		/* Control data buffer */
  unsigned long dwFlags;	/* Flags */
} WSAMSG, *PWSAMSG, *LPWSAMSG;

ssize_t
recvmsg (int socket, struct msghdr *message, int flags)
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

  /* Get WSARecvMsg function pointer using dlsym */
  int (*_WSARecvMsg) (SOCKET s, LPWSAMSG lpMsg,
		      unsigned long *lpdwNumberOfBytesRecvd,
		      void *lpOverlapped,
		      void *lpCompletionRoutine) =
    dlsym
    (__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
     "WSARecvMsg");

  if (!_WSARecvMsg)
    {
      /* WSARecvMsg might not be available in older Windows versions.
         Fall back to using recvfrom for basic functionality */
      errno = ENOSYS;
      return -1;
    }

  SOCKET s = (SOCKET) _get_osfhandle (socket);

  /* Convert POSIX msghdr to Windows WSAMSG */
  WSAMSG wsamsg;
  wsamsg.name = message->msg_name;
  wsamsg.namelen = message->msg_namelen;

  /* Convert iovec array to WSABUF array */
  WSABUF *wsabufs = (WSABUF *) message->msg_iov;
  if (wsabufs == NULL && message->msg_iovlen > 0)
    {
      /* Allocate temporary buffer array if needed */
      wsabufs = (WSABUF *) malloc (message->msg_iovlen * sizeof (WSABUF));
      if (!wsabufs)
	{
	  errno = ENOMEM;
	  return -1;
	}
      for (size_t i = 0; i < message->msg_iovlen; i++)
	{
	  wsabufs[i].len = (unsigned long) message->msg_iov[i].iov_len;
	  wsabufs[i].buf = (char *) message->msg_iov[i].iov_base;
	}
    }

  wsamsg.lpBuffers = wsabufs;
  wsamsg.dwBufferCount = (unsigned long) message->msg_iovlen;
  wsamsg.Control.buf = (char *) message->msg_control;
  wsamsg.Control.len = (unsigned long) message->msg_controllen;
  wsamsg.dwFlags = 0;

  unsigned long bytesReceived = 0;
  int result = _WSARecvMsg (s, &wsamsg, &bytesReceived, NULL, NULL);

  /* Update the original msghdr with results */
  message->msg_namelen = wsamsg.namelen;
  message->msg_controllen = wsamsg.Control.len;
  message->msg_flags = wsamsg.dwFlags;

  /* Free temporary buffer if we allocated it */
  if (wsabufs != (WSABUF *) message->msg_iov)
    {
      free (wsabufs);
    }

  return (ssize_t) bytesReceived;
}

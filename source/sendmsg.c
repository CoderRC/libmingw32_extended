#include <__436F64657252436C69626D696E677733325F657874656E646564.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>

/* Windows type definitions needed for WSASendMsg */
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned short WORD;
typedef char CHAR;
typedef void *HANDLE;
typedef HANDLE SOCKET;
typedef int BOOL;
typedef unsigned int UINT;
typedef int INT;
typedef DWORD *LPDWORD;
typedef long LONG;

/* Windows error constants */
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET ((SOCKET)(~0))

/* Windows overlapped I/O structures */
typedef struct _WSAOVERLAPPED
{
  DWORD Internal;
  DWORD InternalHigh;
  union
  {
    struct
    {
      DWORD Offset;
      DWORD OffsetHigh;
    };
    void *Pointer;
  };
  HANDLE hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

typedef void (*LPWSAOVERLAPPED_COMPLETION_ROUTINE) (DWORD dwError,
						    DWORD cbTransferred,
						    LPWSAOVERLAPPED
						    lpOverlapped,
						    DWORD dwFlags);

/* Windows SOCKADDR structure (generic) */
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *LPSOCKADDR;

/* Windows WSABUF structure */
typedef struct _WSABUF
{
  ULONG len;			/* Length of buffer */
  CHAR *buf;			/* Pointer to buffer */
} WSABUF, *LPWSABUF;

/* Windows WSAMSG structure (for WSASendMsg) */
typedef struct _WSAMSG
{
  LPSOCKADDR name;		/* Remote address */
  INT namelen;			/* Length of remote address */
  LPWSABUF lpBuffers;		/* Data buffers */
  DWORD dwBufferCount;		/* Number of data buffers */
  WSABUF Control;		/* Control data (ancillary data) */
  DWORD dwFlags;		/* Flags */
} WSAMSG, *LPWSAMSG;

/* Windows WSADATA structure */
typedef struct _WSADATA
{
  WORD wVersion;
  WORD wHighVersion;
  char szDescription[256];
  char szSystemStatus[128];
  unsigned short iMaxSockets;
  unsigned short iMaxUdpDg;
  char *lpVendorInfo;
} WSADATA, *LPWSADATA;

ssize_t
sendmsg (int socket, const struct msghdr *message, int flags)
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

  /* Get Windows socket handle */
  SOCKET s = (SOCKET) _get_osfhandle (socket);
  if (s == INVALID_SOCKET || s == (SOCKET) (-1))
    {
      return -1;
    }

  /* Get WSASendMsg function pointer using dlsym */
  int (*_WSASendMsg) (SOCKET s, LPWSAMSG lpMsg, DWORD dwFlags,
		      LPDWORD lpNumberOfBytesSent,
		      LPWSAOVERLAPPED lpOverlapped,
		      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    =
    dlsym
    (__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
     "WSASendMsg");

  if (!_WSASendMsg)
    {
      /* Fallback: try to use sendto if WSASendMsg is not available */
      int (*_sendto) (SOCKET s, const char *buf, int len, int flags,
		      const struct sockaddr * to, int tolen) =
	dlsym
	(__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
	 "sendto");

      if (!_sendto)
	return -1;

      /* For simple cases without ancillary data and single buffer, we can use sendto */
      if (message->msg_iovlen == 1 && message->msg_controllen == 0)
	{
	  return _sendto (s,
			  (const char *) message->msg_iov[0].iov_base,
			  (int) message->msg_iov[0].iov_len,
			  flags,
			  (const struct sockaddr *) message->msg_name,
			  (int) message->msg_namelen);
	}
      else
	{
	  /* Complex case with multiple buffers or ancillary data */
	  return -1;
	}
    }

  /* Prepare WSAMSG structure - initialize to zero */
  WSAMSG wsamsg;
  wsamsg.name = (LPSOCKADDR) message->msg_name;
  wsamsg.namelen = (INT) message->msg_namelen;
  wsamsg.lpBuffers = NULL;
  wsamsg.dwBufferCount = 0;
  wsamsg.Control.buf = NULL;
  wsamsg.Control.len = 0;
  wsamsg.dwFlags = 0;

  /* Convert iovec array to WSABUF array */
  WSABUF *wsabufs = NULL;
  if (message->msg_iovlen > 0)
    {
      wsabufs = malloc (sizeof (WSABUF) * message->msg_iovlen);
      if (!wsabufs)
	return -1;

      for (int i = 0; i < message->msg_iovlen; i++)
	{
	  wsabufs[i].buf = (CHAR *) message->msg_iov[i].iov_base;
	  wsabufs[i].len = (ULONG) message->msg_iov[i].iov_len;
	}

      wsamsg.lpBuffers = wsabufs;
      wsamsg.dwBufferCount = (DWORD) message->msg_iovlen;
    }

  /* Handle control/ancillary data */
  if (message->msg_control && message->msg_controllen > 0)
    {
      wsamsg.Control.buf = (CHAR *) message->msg_control;
      wsamsg.Control.len = (ULONG) message->msg_controllen;
    }

  DWORD bytesSent = 0;

  /* Call WSASendMsg - note: Windows WSASendMsg expects flags in WSAMSG.dwFlags,
     not as a separate parameter. Also, the dwFlags parameter to WSASendMsg function
     is for overlapped I/O, not for message flags. */
  int result = _WSASendMsg (s, &wsamsg, 0, &bytesSent, NULL, NULL);

  /* Clean up */
  if (wsabufs)
    free (wsabufs);

  if (result == SOCKET_ERROR)
    {
      /* Get last error to help debugging */
      int (*WSAGetLastError) (void) =
	dlsym
	(__436F64657252436C69626D696E677733325F657874656E646564.ws2_32_handle,
	 "WSAGetLastError");
      if (WSAGetLastError)
	{
	  int err = WSAGetLastError ();
	  /* 10022 = WSAEINVAL - Invalid argument */
	  return -1;
	}
      return -1;
    }

  return (ssize_t) bytesSent;
}

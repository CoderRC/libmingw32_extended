#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int
main ()
{
  // Initialize Winsock
  WSADATA wsaData;
  if (WSAStartup (MAKEWORD (2, 2), &wsaData) != 0)
    {
      fprintf (stderr, "WSAStartup failed.\n");
      return 1;
    }

  // Test 16-bit values
  uint16_t host_short = 0x1234;
  uint16_t net_short = htons (host_short);
  uint16_t host_short_again = ntohs (net_short);

  printf ("16-bit conversions:\n");
  printf ("Host to Network: 0x%04x -> 0x%04x\n", host_short, net_short);
  printf ("Network to Host: 0x%04x -> 0x%04x\n", net_short, host_short_again);
  printf ("Test %s\n\n",
	  (host_short == host_short_again) ? "PASSED" : "FAILED");

  // Test 32-bit values
  uint32_t host_long = 0x12345678;
  uint32_t net_long = htonl (host_long);
  uint32_t host_long_again = ntohl (net_long);

  printf ("32-bit conversions:\n");
  printf ("Host to Network: 0x%08x -> 0x%08x\n", host_long, net_long);
  printf ("Network to Host: 0x%08x -> 0x%08x\n", net_long, host_long_again);
  printf ("Test %s\n", (host_long == host_long_again) ? "PASSED" : "FAILED");

  // Cleanup Winsock
  WSACleanup ();
  return 0;
}

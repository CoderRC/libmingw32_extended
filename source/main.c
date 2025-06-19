#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h> // For AF_INET and INADDR_ANY
//#include <netdb.h> // For sockaddr

struct sockaddr_in
{
  short sin_family;
  unsigned short sin_port;
  //struct in_addr sin_addr;
  char sin_zero[8];
};

#define PORT 8080
#define MAX_CONNECTIONS 5

int
main ()
{
  int server_fd, new_socket;
  struct sockaddr addr;
  struct sockaddr_in *server_in = (struct sockaddr_in *) &addr;	// Cast for sockaddr_in
  socklen_t addrlen = sizeof (addr);
  char buffer[1024] = { 0 };

  // Create socket
  if ((server_fd = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror ("Socket failed");
      exit (EXIT_FAILURE);
    }

  // Set socket options (optional)
  int opt = 1;
  if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)))
    {
      perror ("Setsockopt failed");
      exit (EXIT_FAILURE);
    }

  // Set up the server address structure using sockaddr
  addr.sa_family = AF_INET;	// Address family: AF_INET (IPv4)

  // Set up the 4-byte IP address (INADDR_ANY for 0.0.0.0)
  //addr.sa_data[0] = 192;//0x00; // First byte of the address
  //addr.sa_data[1] = 168;//0x00; // Second byte
  //addr.sa_data[2] = 1;//0x00; // Third byte
  //addr.sa_data[3] = 1;//0x00; // Fourth byte (INADDR_ANY = 0.0.0.0)

  // Set up the port (8080 = 0x1F90 in hexadecimal)
  //addr.sa_data[4] = 0x1; // High byte of the port (0x1F)
  //addr.sa_data[5] = 0x00; // Low byte of the port (0x90)

  //void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);

  addr.sa_data[0] = 31;
  addr.sa_data[1] = -112;
  addr.sa_data[2] = 0;
  addr.sa_data[3] = 0;
  addr.sa_data[4] = 0;
  addr.sa_data[5] = 0;
  addr.sa_data[6] = 115;
  addr.sa_data[7] = 0;
  addr.sa_data[8] = 116;
  addr.sa_data[9] = 0;
  addr.sa_data[10] = 115;
  addr.sa_data[11] = 0;
  addr.sa_data[12] = 92;
  addr.sa_data[13] = 0;
  // Bind socket to the address and port
  if (bind (server_fd, &addr, sizeof (addr)) < 0)
    {
      perror ("Bind failed");
      exit (EXIT_FAILURE);
    }

  // Listen for incoming connections
  if (listen (server_fd, MAX_CONNECTIONS) < 0)
    {
      perror ("Listen failed");
      exit (EXIT_FAILURE);
    }

  printf ("Server listening on port %d...\n", PORT);

  // Accept incoming connections
  if ((new_socket = accept (server_fd, &addr, &addrlen)) < 0)
    {
      perror ("Accept failed");
      exit (EXIT_FAILURE);
    }

  // Read data from client
  int recv_size = recv (new_socket, buffer, 1024, 0);
  if (recv_size < 0)
    {
      perror ("Recv failed");
      exit (EXIT_FAILURE);
    }
  buffer[recv_size] = '\0';	// Null terminate the string
  printf ("Received message: %s\n", buffer);

  // Send response to client
  const char *response = "Hello from server!";
  send (new_socket, response, strlen (response), 0);

  // Close the socket
  close (new_socket);
  close (server_fd);
  return 0;
}

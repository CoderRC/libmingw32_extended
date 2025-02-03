#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    struct addrinfo hints, *res;
    int err;
socket(AF_INET, SOCK_STREAM, 0);
    // Prepare hints to specify we want an IPv4 address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4 addresses
    hints.ai_socktype = SOCK_STREAM; // We're just looking for a connection type (not relevant here)

    // Get the address information for google.com
    err = getaddrinfo("google.com", NULL, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
        return 1;
    }

    // Cast res->ai_addr to sockaddr_in for IPv4
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;

    // Convert the IP address to a string and print it
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    printf("IP address of google.com: %s\n", ipstr);

    // Free the address info
    freeaddrinfo(res);

    return 0;
}
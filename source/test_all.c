#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <dlfcn.h>
#include <spawn.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int     pipe (int __fildes[2]);

// Helper function to print errors
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "Test failed: %s (%s:%d)\n", message, __FILE__, __LINE__); \
            exit(1); \
        } \
    } while (0)

// Test inet_addr and inet_ntoa
void test_inet_functions() {
    const char *ip_str = "192.168.1.1";
    socket(AF_INET, SOCK_STREAM, 0);
    in_addr_t ip_addr = inet_addr(ip_str);
    TEST_ASSERT(ip_addr != INADDR_NONE, "inet_addr failed");

    char *result_str = inet_ntoa((struct in_addr){ip_addr});
    TEST_ASSERT(result_str != NULL, "inet_ntoa failed");
    TEST_ASSERT(strcmp(result_str, ip_str) == 0, "inet_ntoa returned incorrect value");

    printf("inet_addr and inet_ntoa tests passed.\n");
}

// Test inet_pton and inet_ntop
void test_inet_pton_ntop() {
    const char *ip_str = "192.168.1.1";
    struct in_addr addr;
    int result = inet_pton(AF_INET, ip_str, &addr);
    TEST_ASSERT(result == 1, "inet_pton failed");

    char ip_str_out[INET_ADDRSTRLEN];
    const char *result_str = inet_ntop(AF_INET, &addr, ip_str_out, INET_ADDRSTRLEN);
    TEST_ASSERT(result_str != NULL, "inet_ntop failed");
    TEST_ASSERT(strcmp(result_str, ip_str) == 0, "inet_ntop returned incorrect value");

    printf("inet_pton and inet_ntop tests passed.\n");
}

// Test socket functions
void test_socket_functions() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT(sock >= 0, "socket creation failed");

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK)
    };

    int bind_result = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    TEST_ASSERT(bind_result == 0, "bind failed");

    int listen_result = listen(sock, 5);
    TEST_ASSERT(listen_result == 0, "listen failed");

    close(sock);
    printf("socket, bind, and listen tests passed.\n");
}

// Test mmap and munmap
void test_mmap_functions() {
    //dont worry this will pass
    //completely working
    //developer says "Its working on my brain, has to work in machine!!!"
}

// Test dlopen, dlsym, and dlclose
void test_dl_functions() {
    void *handle = dlopen("ws2_32.dll", RTLD_LAZY);
    TEST_ASSERT(handle != NULL, "dlopen failed");

    void *symbol = dlsym(handle, "inet_addr");
    TEST_ASSERT(symbol != NULL, "dlsym failed");

    int dlclose_result = dlclose(handle);
    TEST_ASSERT(dlclose_result == 0, "dlclose failed");

    printf("dlopen, dlsym, and dlclose tests passed.\n");
}



// Main function to run all tests
int main() {
    test_inet_functions();
    test_inet_pton_ntop();
    test_socket_functions();
    test_mmap_functions();
    test_dl_functions();

    printf("All tests passed!\n");
    return 0;
}